// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeechBubbleHandleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEDIALOGUESYSTEM_API USpeechBubbleHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpeechBubbleHandleComponent();

	virtual void BeginPlay() override;


public:

	UPROPERTY(EditAnywhere , Category = "SpeechBubble")
		FName IDName; //대상의 이름입니다. 다이얼로그 호출에서 식별등에서도 사용됩니다.

	UPROPERTY(EditAnywhere , Category = "SpeechBubble")
		FText DisplayName; //Npc의 이름입니다. 표기에 사용되는 이름입니다.


public:
	//Dialogue Properties

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		TSubclassOf<UUserWidget> SpeechBubbleWidgetClass;
	//다이얼로그를 이용한 대화에서 사용되는 위젯의 레퍼런스입니다.
	UPROPERTY()
		class UDialogueWidgetBase* SpeechBubbleWidgetPtr;
	//다이얼로그 대화에 사용할 대화 매니저입니다.
	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		class UDialogueManager* DialogueManager;

	//대화에 사용할 컴포넌트입니다.
	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		class UAnimaleseSpeechAudioComponent* AnimaleseSpeechAudioComponent;

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		FVector WidgetOffset;

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		FName WidgetAttachTargetComponentName;


public:

	UFUNCTION(BlueprintCallable)
		void SetDialogueAsset(UDialogueManager* InDialogueManager);

	UFUNCTION(BlueprintCallable)
		void ClearDialogueAsset();

public:

	UFUNCTION(BlueprintCallable)
		void StartDialogue();


	UFUNCTION(BlueprintCallable)
		void StartTransientDialogue(TMap<FName, USpeechBubbleHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent = true);
		

	UFUNCTION(BlueprintCallable)
		void EndDialogue();

	//Immediately CloseDialogue
	UFUNCTION(BlueprintCallable)
		void CloseDialogue();

public:

	UFUNCTION(BlueprintCallable)
		bool IsDialoguePlaying();

public:

	//위젯에서 텍스트가 업데이트 되었을때 호출됩니다.
	UFUNCTION()
		virtual void OnStringPlayed(UDialogueWidgetBase* Widget, FString String, float Duration, int32 CurrentIndex);

	UFUNCTION()
		virtual void OnStringListened(UDialogueWidgetBase* Widget);

	//지정 커맨드를 실행할때 호출됩니다.
	UFUNCTION()
		virtual void OnCustomCommandCalled(FString Command);


};
