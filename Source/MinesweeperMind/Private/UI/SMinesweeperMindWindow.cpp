#include "SMinesweeperMindWindow.h"

#include "Widgets/Layout/SBox.h"

#include "Widgets/SMinesweeperWidget.h"
#include "Widgets/SChatboxWidget.h"
#include "Widgets/Layout/SScaleBox.h"

void SMinesweeperMindWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		// Minesweeper
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.f)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			[
				SNew(SMinesweeperWidget)
			]
		]
		
		// Chatbox
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		.Padding(10.f)
		[
			SNew(SChatboxWidget)
		]
	];
}

