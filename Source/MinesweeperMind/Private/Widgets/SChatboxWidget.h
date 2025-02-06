#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 *  Large Language Model user-interaction chat.
 */
class SChatboxWidget final : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SChatboxWidget) {}
SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SBorder> ChatContainer;
    TSharedPtr<SEditableTextBox> ChatInput;
    TSharedPtr<SScrollBox> ChatScrollBox;
    TArray<TSharedPtr<FString>> ChatMessages;

    void OnChatTextCommitted(const FText& InText, ETextCommit::Type CommitType);
    FReply OnSendButtonClicked();
    void AddChatMessage(const FString& Message);
};
