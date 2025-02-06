#include "SMinesweeperWidget.h"

#include "SMinesweeperCellWidget.h"
#include "SMinesweeperRestartButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Math/UnrealMathUtility.h"
#include "Logging/LogMacros.h"

void SMinesweeperWidget::Construct(const FArguments& InArgs)
{
    NumRows = InArgs._NumRows;
    NumColumns = InArgs._NumColumns;
    NumMines = InArgs._NumMines;

    const int32 TotalCells = NumRows * NumColumns;
    NumMines = FMath::Min(TotalCells, NumMines); // Can't have more mines than land.

    ResetGameState();

    ChildSlot
    [
        SNew(SOverlay)

        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SVerticalBox)

            // Restart Button
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .Padding(0.f, 10.f) // Add spacing above
            [
                SAssignNew(RestartButton, SMinesweeperRestartButton)
                .OnRestartClicked(FOnRestartClicked::CreateSP(this, &SMinesweeperWidget::RestartGame))
            ]

            // Grid Panel
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            .HAlign(HAlign_Center)
            [
                SNew(SBox)
                .WidthOverride(400.f)
                .HeightOverride(400.f)
                [
                    GridPanel.ToSharedRef()
                ]
            ]
        ]
    ];
}

void SMinesweeperWidget::RestartGame()
{
    ResetGameState();
    GenerateGrid(NumRows, NumColumns, NumMines);
}

void SMinesweeperWidget::GenerateGrid(int32 Rows, int32 Columns, int32 Bombs)
{
    SetupGridPanel();
    CreateCellWidgets();
    PlaceMines(Bombs);
    CalculateAdjacency();
}

void SMinesweeperWidget::PlaceMines(int32 Mines)
{
    TArray<FIntPoint> AvailablePositions;
    for (int32 RowIndex = 0; RowIndex < NumRows; ++RowIndex)
    {
        for (int32 ColumnIndex = 0; ColumnIndex < NumColumns; ++ColumnIndex)
        {
            AvailablePositions.Add(FIntPoint(RowIndex, ColumnIndex));
        }
    }

    for (int32 MineIndex = 0; MineIndex < NumMines && !AvailablePositions.IsEmpty(); ++MineIndex)
    {
        const int32 RandomPositionIndex = FMath::RandRange(0, AvailablePositions.Num() - 1);
        const FIntPoint MinePosition = AvailablePositions[RandomPositionIndex];

        AvailablePositions.RemoveAtSwap(RandomPositionIndex, 1, EAllowShrinking::No); // Updated API

        CellsWithMines[MinePosition.X][MinePosition.Y] = true;
    }
}

FReply SMinesweeperWidget::OnCellClicked(int32 Row, int32 Col)
{
    if (GameOver || CellsRevealed[Row][Col])
    {
        return FReply::Handled();
    }

    if (CellsWithMines[Row][Col])
    {
        UpdateCellAppearance(Row, Col, TEXT("💣"), FLinearColor::Red);
        RevealAllMines();
        GameOver = true;
    }
    else
    {
        RevealCell(Row, Col);
    }

    return FReply::Handled();
}

FReply SMinesweeperWidget::OnCellRightClicked(int32 Row, int32 Col)
{
    if (GameOver || CellsRevealed[Row][Col])
    {
        return FReply::Handled();
    }
    
    CellsFlagged[Row][Col] = !CellsFlagged[Row][Col];
    FString CellText = CellsFlagged[Row][Col] ? TEXT("🚩") : TEXT(" ");
    UpdateCellAppearance(Row, Col, CellText, FSlateColor(FLinearColor::White));

    return FReply::Handled();
}

void SMinesweeperWidget::RevealAllMines()
{
    for (int32 RowIndex = 0; RowIndex < NumRows; ++RowIndex)
    {
        for (int32 ColumnIndex = 0; ColumnIndex < NumColumns; ++ColumnIndex)
        {
            if (!CellsRevealed[RowIndex][ColumnIndex])
            {
                if (CellsWithMines[RowIndex][ColumnIndex])
                {
                    UpdateCellAppearance(RowIndex, ColumnIndex, TEXT("💣"), FLinearColor::Red);
                }
                Cells[RowIndex][ColumnIndex]->SetBackgroundColor(FLinearColor::Red);
            }
        }
    }
}

void SMinesweeperWidget::CalculateAdjacency()
{
    for (int32 RowIndex = 0; RowIndex < NumRows; ++RowIndex)
    {
        for (int32 ColIndex = 0; ColIndex < NumColumns; ++ColIndex)
        {
            if (CellsWithMines[RowIndex][ColIndex])
            {
                AdjacentMines[RowIndex][ColIndex] = -1;
                continue;
            }

            int32 MineCounter = 0;
            for (int32 OffsetRow = -1; OffsetRow <= 1; ++OffsetRow)
            {
                for (int32 OffsetCol = -1; OffsetCol <= 1; ++OffsetCol)
                {
                    int32 NeighborRow = RowIndex + OffsetRow;
                    int32 NeighborCol = ColIndex + OffsetCol;
                    
                    if (NeighborRow >= 0 && NeighborRow < NumRows &&
                        NeighborCol >= 0 && NeighborCol < NumColumns &&
                        CellsWithMines[NeighborRow][NeighborCol])
                    {
                        ++MineCounter;
                    }
                }
            }
            AdjacentMines[RowIndex][ColIndex] = MineCounter;
        }
    }
}

void SMinesweeperWidget::RevealCell(int32 RowIndex, int32 ColIndex)
{
    if (CellsRevealed[RowIndex][ColIndex])
    {
        return;
    }

    FString CellText = (AdjacentMines[RowIndex][ColIndex] > 0)
                           ? FString::FromInt(AdjacentMines[RowIndex][ColIndex])
                           : TEXT(" ");
    FSlateColor CellTextColor = (AdjacentMines[RowIndex][ColIndex] > 0)
                                    ? GetNumberColor(AdjacentMines[RowIndex][ColIndex])
                                    : FSlateColor(FLinearColor::Black);

    UpdateCellAppearance(RowIndex, ColIndex, CellText, CellTextColor);
    CellsRevealed[RowIndex][ColIndex] = true;
    Cells[RowIndex][ColIndex]->SetBackgroundColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f)); // Light Gray

    ++SafeCellsRevealed;
    CheckWinCondition();

    if (AdjacentMines[RowIndex][ColIndex] == 0)
    {
        RecursivelyRevealNeighbors(RowIndex, ColIndex);
    }
}

FSlateColor SMinesweeperWidget::GetNumberColor(int32 Number) const
{
    switch (Number)
    {
    case 1: return FSlateColor(FLinearColor::Blue);
    case 2: return FSlateColor(FLinearColor::Green);
    case 3: return FSlateColor(FLinearColor::Red);
    case 4: return FSlateColor(FLinearColor(0.5f, 0.f, 0.5f)); // Purple
    case 5: return FSlateColor(FLinearColor(0.5f, 0.f, 0.f));   // Maroon
    case 6: return FSlateColor(FLinearColor(0.25f, 0.88f, 0.81f)); // Turquoise
    case 7: return FSlateColor(FLinearColor::Black);
    case 8: return FSlateColor(FLinearColor::Gray);
    default: return FSlateColor(FLinearColor::Black);
    }
}

void SMinesweeperWidget::InitializeGameState()
{
    GameOver = false;
    GameWon = false;
    SafeCellsRevealed = 0;

    CellsFlagged.Empty();
    CellsRevealed.Empty();
    CellsWithMines.Empty();
    AdjacentMines.Empty();
    Cells.Empty();
}

void SMinesweeperWidget::ResetGameState()
{
    InitializeGameState();
    GenerateGrid(NumRows, NumColumns, NumMines);
}

void SMinesweeperWidget::SetupGridPanel()
{
    if (!GridPanel.IsValid())
    {
        GridPanel = SNew(SUniformGridPanel);
    }
    GridPanel->ClearChildren();
}

void SMinesweeperWidget::CreateCellWidgets()
{
    CellsFlagged.SetNum(NumRows);
    CellsRevealed.SetNum(NumRows);
    CellsWithMines.SetNum(NumRows);
    AdjacentMines.SetNum(NumRows);
    Cells.SetNum(NumRows);

    for (int32 Row = 0; Row < NumRows; ++Row)
    {
        CellsFlagged[Row].SetNumZeroed(NumColumns);
        CellsRevealed[Row].SetNumZeroed(NumColumns);
        CellsWithMines[Row].SetNumZeroed(NumColumns);
        AdjacentMines[Row].SetNumZeroed(NumColumns);
        Cells[Row].SetNum(NumColumns);

        for (int32 Col = 0; Col < NumColumns; ++Col)
        {
            Cells[Row][Col] =
                SNew(SMinesweeperCellWidget)
                .OnClicked(FOnClicked::CreateSP(this, &SMinesweeperWidget::OnCellClicked, Row, Col))
                .OnRightClicked(FOnClicked::CreateSP(this, &SMinesweeperWidget::OnCellRightClicked, Row, Col))
                .CellText(FText::FromString(TEXT(" ")))
                .TextColor(FSlateColor(FLinearColor::Black));

            GridPanel->AddSlot(Row, Col)
            [
                Cells[Row][Col].ToSharedRef()
            ];
        }
    }
}

void SMinesweeperWidget::UpdateCellAppearance(int32 Row, int32 Col, const FString& Text, const FSlateColor& Color)
{
    Cells[Row][Col]->UpdateCell(FText::FromString(Text), Color);
}

void SMinesweeperWidget::CheckWinCondition()
{
    const int32 TotalSafeCells = (NumRows * NumColumns) - NumMines;
    if (SafeCellsRevealed == TotalSafeCells)
    {
        GameWon = true;
        GameOver = true;
        UE_LOG(LogTemp, Log, TEXT("Congratulations! You win!"));
        HighlightAllSafeCells();
    }
}

void SMinesweeperWidget::RecursivelyRevealNeighbors(int32 RowIndex, int32 ColIndex)
{
    for (int32 OffsetRow = -1; OffsetRow <= 1; ++OffsetRow)
    {
        for (int32 OffsetCol = -1; OffsetCol <= 1; ++OffsetCol)
        {
            int32 NeighborRow = RowIndex + OffsetRow;
            int32 NeighborCol = ColIndex + OffsetCol;
            
            if (NeighborRow >= 0 && NeighborRow < NumRows &&
                NeighborCol >= 0 && NeighborCol < NumColumns &&
                !CellsRevealed[NeighborRow][NeighborCol])
            {
                RevealCell(NeighborRow, NeighborCol);
            }
        }
    }
}

void SMinesweeperWidget::HighlightAllSafeCells()
{
    for (int32 Row = 0; Row < NumRows; ++Row)
    {
        for (int32 Col = 0; Col < NumColumns; ++Col)
        {
            if (!CellsWithMines[Row][Col])
            {
                Cells[Row][Col]->SetBackgroundColor(FLinearColor::Green);
            }
        }
    }
}
