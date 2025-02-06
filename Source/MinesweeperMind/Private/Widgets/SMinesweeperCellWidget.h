#pragma once

#include "Widgets/SCompoundWidget.h"

class SButton;
class STextBlock;

/**
 * Clickable cell in Minesweeper grid.
 */
class SMinesweeperCellWidget final : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SMinesweeperCellWidget) {}
	SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_EVENT(FOnClicked, OnRightClicked)
	SLATE_ATTRIBUTE(FText, CellText)
	SLATE_ATTRIBUTE(FSlateColor, TextColor)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);
	void UpdateCell(const FText& InText, const FSlateColor& InColor);
	void SetBackgroundColor(const FLinearColor& NewColor);

private:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	TSharedPtr<SButton> CellButton;
	TSharedPtr<STextBlock> CellTextBlock;

	FOnClicked OnClicked;
	FOnClicked OnRightClicked;
};
