#include "SChatboxWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

void SChatboxWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.MinDesiredWidth(500)
		.MaxDesiredWidth(800)
		.MinDesiredHeight(200)
		.MaxDesiredHeight(200)
		[
			SNew(SBorder)
			.Padding(10)
			[
				SNew(SVerticalBox)

				// Log Area
				+ SVerticalBox::Slot()
				.FillHeight(0.3f)
				.MaxHeight(200)
				.Padding(10)
				[
					SNew(SBox)
					.MinDesiredWidth(500)
					.MaxDesiredWidth(800)
					[
						SNew(SBorder)
						.Padding(5)
						[
							SAssignNew(ChatScrollBox, SScrollBox)
							.ScrollBarVisibility(EVisibility::Visible)
						]
					]
				]

				// Input Area
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10, 5)
				[
					SNew(SBox)
					.MinDesiredWidth(500)
					.MaxDesiredWidth(800)
					[
						SNew(SHorizontalBox)

						// Editable text box
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SAssignNew(ChatInput, SEditableTextBox)
							.MinDesiredWidth(400) // Prevents shrinking
							.HintText(FText::FromString(TEXT("Ask the AI Mind.")))
							.OnTextCommitted(this, &SChatboxWidget::OnChatTextCommitted)
							.ClearKeyboardFocusOnCommit(false)
						]

						// Send Button
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5, 0)
						[
							SNew(SButton)
							.Text(FText::FromString(TEXT("Send")))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked(this, &SChatboxWidget::OnSendButtonClicked)
						]
					]
				]
			]
		]
	];
}

FReply SChatboxWidget::OnSendButtonClicked()
{
	if (ChatInput.IsValid())
	{
		FText InputText = ChatInput->GetText();
		if (!InputText.IsEmpty())
		{
			OnChatTextCommitted(InputText, ETextCommit::OnEnter);
		}
		// TODO: Improve hacky method to regain focus on chat after clicking send.
		return FReply::Handled().SetUserFocus(ChatInput.ToSharedRef());
	}

	return FReply::Handled();
}

void SChatboxWidget::OnChatTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter && !InText.IsEmpty())
	{
		FString Message = InText.ToString();
		AddChatMessage(Message);

		if (ChatInput.IsValid())
		{
			ChatInput->SetText(FText::GetEmpty());

			if (ChatScrollBox.IsValid())
			{
				ChatScrollBox->ScrollToEnd();
			}
		}
	}
}

void SChatboxWidget::AddChatMessage(const FString& Message)
{
	ChatMessages.Add(MakeShareable(new FString(Message)));

	if (ChatScrollBox.IsValid())
	{
		ChatScrollBox->AddSlot()
		             .Padding(2)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Message))
			.AutoWrapText(true)
		];

		ChatScrollBox->ScrollToEnd();
	}
}
