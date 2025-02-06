#pragma once

#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE(FOnRestartClicked);

/**
 *  Button for restarting Minesweeper.
 */
class SMinesweeperRestartButton : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SMinesweeperRestartButton) {}
	SLATE_EVENT(FOnRestartClicked, OnRestartClicked)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	FOnRestartClicked OnRestartClickedDelegate;

	FReply OnButtonClicked();
};
