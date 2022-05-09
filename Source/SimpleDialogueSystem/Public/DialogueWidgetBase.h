// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"

#include "TimerManager.h"

#include "DialogueManager.h"

#include "DialogueWidgetBase.generated.h"

/**
 * 
 */
class UDialogueButtonWidget;


//다이얼로그 위젯의 기본 형태입니다. 커맨드 콜과 같은 내용이 전혀 없이, UUserWidget의 형태와 내부 동작만을 정의합니다.
//추상 클래스로서, 반드시 오버라이드 하셔서 사용하셔야합니다.


UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetOpen);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetNext);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetClose);

USTRUCT()
struct FCustomTagStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FString> CustomTags;

	FCustomTagStruct() {}

	FCustomTagStruct(TArray<FString> Arr) {
		CustomTags.Append(Arr);
	}

	FCustomTagStruct(FString Str) {
		CustomTags.Add(Str);
	}

};


UCLASS(Abstract)
class SIMPLEDIALOGUESYSTEM_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

//Main Property

	UPROPERTY(EditAnywhere)
		class URichTextBlock* ContextBlock;

	UPROPERTY(EditAnywhere)
		class URichTextBlock* OwnerNameBlock;

	UPROPERTY(EditAnywhere)
		class UScrollBox* SeletorScrollBox;

	UPROPERTY(EditAnywhere)
		class UButton* SkipButton;

public:

	void InitializeDialogueWidget(UDialogueManager* InBaseDialogueManager);

public:

	UPROPERTY(EditAnywhere)
	TMap<int32, FCustomTagStruct> CustomTagMap; //태그 컨테이너입니다. 해당 태그가 시작되는 위치와 태그의 내용을 전부 저장합니다.

	FText ContextBlockText; 

	FTimerManager* TimerManagerRef;

	FTimerHandle TextUpdateHandle;

public:

	UPROPERTY(EditAnywhere)
		class UDialogueManager* BaseDialogueManager; //플레이 할 다이얼로그 매니저

	UPROPERTY(EditAnywhere)
		class UDialogueUnitBase* CurrentPlayingDialogueUnit; //플레이 중이라고 마킹된 다이얼로그

public:

	FTimerHandle AutoSkipHandle;

	void SkipCurrentUnit();


public:

//Event

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	//명시적으로 파괴해야할때 사용합니다. 주로 RemoveFromParent를 이용해 레퍼런스를 제거해 GC에 의한 디스트럭션의 이점을 받을수 없을때 사용합니다.
	UFUNCTION(BlueprintCallable)
		void ExplictDestruct();

//Function

	UFUNCTION(BlueprintCallable)
		void InitializeBasedOnCurrentDialogue(); // 현재 실행중인 다이얼로그 위젯에 기반하여 초반 텍스트와 이름, 텍스트 표기수등을 재설정합니다.

	UFUNCTION(BlueprintCallable)
		void SetFirstDialogueUnitBasedOnCurrentDialogue();

	UFUNCTION(BlueprintCallable)
		void UpdateContextBlockTextBasedOnCurrentDialogue();

	UFUNCTION(BlueprintCallable)
		virtual void UpdateSpeakerBlockTextBasedOnCurrentDialogue(); // 현지화 때문에 오버라이드가 가능하게 만듦.

	UFUNCTION(BlueprintCallable)
		void CollectCustomTagsFromCurrentContextBlockText();

	int NumberOfShowingChar = 0;

	bool bShouldAppendEndDecorator = false;

	void SkipDecorator();
	
	UFUNCTION(BlueprintCallable)
		void SkipContext();

	void CheckAndPlaySkippedCommand();

	float TextUpdateFrequency = 0.05;

	UFUNCTION(BlueprintCallable)
	void StartUpdateText();

	UFUNCTION(BlueprintCallable)
	virtual void UpdateText(); // 기본형이 주어지나, 원한다면 상속하여 추가적인 동작을 정의할 수 있습니다. 편한대로 ㄲ

	UFUNCTION(BlueprintCallable)
	virtual void StopUpdateText();

	void TryStartNextDialogue(); //다음 다이얼로그가 있다면 다음 다이얼로그를 실행시키고, 아니라면 위젯을 제거합니다.

	virtual void CallCommand(FString Command) {};


	UFUNCTION(BlueprintCallable)
	void AddToViewport_Implemented();

	virtual void OpenAction(); //애니메이션을 재생하고 나서 업데이트를 시작하게 한다던지, 여러가지 입맛대로 위젯 생성을 제어할 수 있는 코드입니다.

	virtual void CloseAction();

//selector Function

	UPROPERTY(EditAnywhere, Category = Widget)
		TSubclassOf<UDialogueButtonWidget> DialogueButtonWidgetClass;

	UFUNCTION(BlueprintCallable)
		void AddSelectOptionButtonOnWidget();

	UFUNCTION(BlueprintCallable)
		void CallBack_OnSelectOptionButtonPressed(UDialogueUnit_Select_Option* Option);

	UFUNCTION(BlueprintCallable)
		bool CheckSwitchUnitAndRunSwitch();

	UPROPERTY()
		bool bIsSelectorCalled = false;

//custom event

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetOpen WidgetOpenDele;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetNext WidgetNextDele;
	
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetClose WidgetCloseDele;

	virtual void OnDialogueWidgetBegin() { WidgetOpenDele.Broadcast(); } // 위젯이 뷰포트에 처음 그려질 때 호출됩니다.

	virtual void OnNextDialoguePlayed() { WidgetNextDele.Broadcast(); } // 다음 다이얼로그가 화면에 처음 그려질 때 호출됩니다.

	virtual void OnDialogueWidgetEnd() { WidgetCloseDele.Broadcast();} // 위젯이 뷰포트에서 완전히 사라질 때 호출됩니다.

};
