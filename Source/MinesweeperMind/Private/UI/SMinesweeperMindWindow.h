#pragma once

#include "Widgets/SCompoundWidget.h"

class SMinesweeperRestartButton;
class SMinesweeperWidget;
class SChatboxWidget;

class SMinesweeperMindWindow final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperMindWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
