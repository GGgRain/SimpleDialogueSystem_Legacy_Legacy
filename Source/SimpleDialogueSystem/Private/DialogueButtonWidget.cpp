// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueButtonWidget.h"

void UDialogueButtonWidget::NativeConstruct() {
	UUserWidget::NativeConstruct();

	Button = Cast<UButton>(GetWidgetFromName(TEXT("ButtonChild")));
	ContextTextBlock = Cast<URichTextBlock>(GetWidgetFromName(TEXT("ContextTextBlockChild")));

	if (ContextTextBlock != nullptr) {
		ContextTextBlock->SetText(Option->AnswerContextText);
	}
}

void UDialogueButtonWidget::NativeDestruct() {
	UUserWidget::NativeDestruct();
}


void UDialogueButtonWidget::InitializeDialogueButtonWidget(UDialogueUnit_Select_Option* InOption, UDialogueWidgetBase* InParentWidget) {
	this->Option = InOption;
	this->ParentWidget = InParentWidget;

	if (ContextTextBlock != nullptr) {
		ContextTextBlock->SetText(Option->AnswerContextText);
	}

}


void UDialogueButtonWidget::NotifyOnParentWidget() {
	if (ParentWidget != nullptr) {
		ParentWidget->CallBack_OnSelectOptionButtonPressed(Option);
	}
}