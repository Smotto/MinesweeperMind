#include "SMinesweeperCellWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

void SMinesweeperCellWidget::Construct(const FArguments& InArgs)
{
	OnClicked = InArgs._OnClicked;
	OnRightClicked = InArgs._OnRightClicked;
	
	ChildSlot
	[
		SAssignNew(CellButton, SButton)
		.OnClicked(InArgs._OnClicked)
		.ContentPadding(FMargin(0))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(CellTextBlock, STextBlock)
			.Text(InArgs._CellText)
			.ColorAndOpacity(InArgs._TextColor)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 24)) // Adjust font size here
			.Justification(ETextJustify::Center)
		]
	];
}

void SMinesweeperCellWidget::UpdateCell(const FText& InText, const FSlateColor& InColor)
{
	if (CellTextBlock.IsValid())
	{
		FString TextString = InText.ToString();
		int32 FontSize = 24;
		if (TextString == TEXT("💣") || TextString == TEXT("🚩"))
		{
			FontSize = 14;
		}

		CellTextBlock->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), FontSize));
		CellTextBlock->SetText(InText);
		CellTextBlock->SetColorAndOpacity(InColor);
		CellTextBlock->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

void SMinesweeperCellWidget::SetBackgroundColor(const FLinearColor& NewColor)
{
	if (CellButton.IsValid())
	{
		CellButton->SetBorderBackgroundColor(NewColor);
		CellButton->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

FReply SMinesweeperCellWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && OnRightClicked.IsBound())
	{
		if (ensure(OnRightClicked.IsBound()))
		{
			return OnRightClicked.Execute();
		}
	}
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && OnClicked.IsBound())
	{
		if (ensure(OnClicked.IsBound()))
		{
			return OnClicked.Execute();
		}
	}

	return FReply::Unhandled();
}

