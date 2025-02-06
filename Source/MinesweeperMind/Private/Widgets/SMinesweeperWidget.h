#pragma once

#include "Widgets/SCompoundWidget.h"

#define DEFAULT_MINESWEEPER_NUM_MINES 15
#define DEFAULT_MINESWEEPER_NUM_ROWS 10
#define DEFAULT_MINESWEEPER_NUM_COLUMNS 10

class SMinesweeperRestartButton;
class SMinesweeperCellWidget;
class SUniformGridPanel;
class SButton;
class STextBlock;

/**
 * Generates a grid of buttons, places mines randomly,
 * calculates adjacent number of mines, and updates cells when revealed
 */
class SMinesweeperWidget final : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SMinesweeperWidget)
		: _NumRows(DEFAULT_MINESWEEPER_NUM_ROWS)
		, _NumColumns(DEFAULT_MINESWEEPER_NUM_COLUMNS)
		, _NumMines(DEFAULT_MINESWEEPER_NUM_MINES)
	{}
	SLATE_ARGUMENT(int32, NumRows)
	SLATE_ARGUMENT(int32, NumColumns)
	SLATE_ARGUMENT(int32, NumMines)
SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void RestartGame();

private:
	TSharedPtr<SMinesweeperRestartButton> RestartButton;
	bool GameOver = false;
	bool GameWon = false;         
	int32 SafeCellsRevealed = 0;

	int32 NumMines = DEFAULT_MINESWEEPER_NUM_MINES;
	int32 NumRows = DEFAULT_MINESWEEPER_NUM_ROWS;
	int32 NumColumns = DEFAULT_MINESWEEPER_NUM_COLUMNS;

	TSharedPtr<SUniformGridPanel> GridPanel;
	TArray<TArray<TSharedPtr<SMinesweeperCellWidget>>> Cells;

	TArray<TArray<bool>> CellsWithMines;
	TArray<TArray<bool>> CellsRevealed;
	TArray<TArray<bool>> CellsFlagged;
	TArray<TArray<int32>> AdjacentMines;

	void GenerateGrid(int32 RowSize, int32 ColumnSize, int32 BombCount);
	void PlaceMines(int32 BombCount);
	void CalculateAdjacency();

	FReply OnCellClicked(int32 Row, int32 Col);
	FReply OnCellRightClicked(int32 Row, int32 Col);
	void RevealCell(int32 RowIndex, int32 ColumnIndex);
	void RevealAllMines();

	FSlateColor GetNumberColor(int32 Number) const;

	void InitializeGameState();
	void ResetGameState();
	void SetupGridPanel();
	void CreateCellWidgets();
	void UpdateCellAppearance(int32 Row, int32 Col, const FString& Text, const FSlateColor& Color);
	void CheckWinCondition();
	void RecursivelyRevealNeighbors(int32 RowIndex, int32 ColIndex);
	void HighlightAllSafeCells();
};