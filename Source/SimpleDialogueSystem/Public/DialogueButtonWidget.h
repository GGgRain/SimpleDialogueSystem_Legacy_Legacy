// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "DialogueManager.h"
#include "DialogueWidgetBase.h"
#include "DialogueButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLEDIALOGUESYSTEM_API UDialogueButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		class UButton* Button;

	UPROPERTY(EditAnywhere)
		class URichTextBlock* ContextTextBlock;


	void InitializeDialogueButtonWidget(UDialogueUnit_Select_Option* InOption, UDialogueWidgetBase* InParentWidget);

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere)
	UDialogueUnit_Select_Option* Option;

	UPROPERTY(EditAnywhere)
	UDialogueWidgetBase* ParentWidget;

	UFUNCTION(BlueprintCallable)
		void NotifyOnParentWidget();

	
};
