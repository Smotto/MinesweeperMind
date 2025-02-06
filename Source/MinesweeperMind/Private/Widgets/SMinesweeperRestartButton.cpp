#include "SMinesweeperRestartButton.h"

void SMinesweeperRestartButton::Construct(const FArguments& InArgs)
{
	OnRestartClickedDelegate = InArgs._OnRestartClicked;

	ChildSlot
	[
		SNew(SButton)
		.OnClicked(this, &SMinesweeperRestartButton::OnButtonClicked)
		.ButtonColorAndOpacity(FLinearColor::Gray)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("🔄")))
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 30))
			.Justification(ETextJustify::Center)
		]
	];
}

FReply SMinesweeperRestartButton::OnButtonClicked()
{
	if (OnRestartClickedDelegate.IsBound())
	{
		OnRestartClickedDelegate.Execute();
	}

	return FReply::Handled();
}
