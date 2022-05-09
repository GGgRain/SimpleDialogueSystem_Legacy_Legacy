// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueManager.h"
#include "DialogueHandleComponent.generated.h"

//다이얼로그가 재생되는데에 필요한 설정과 다이얼로그의 진행상황을 넘겨받고 처리하는 곳에 사용되는 컴포넌트입니다.
//

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEDIALOGUESYSTEM_API UDialogueHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueHandleComponent();

	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditAnywhere, Category = Dialogue)
		FName IDName; //대상의 이름입니다. 다이얼로그 호출에서 식별등에서도 사용됩니다.

	UPROPERTY(EditAnywhere, Category = Dialogue)
		FText DisplayName; //Npc의 이름입니다. 표기에 사용되는 이름입니다.


public:
	//Dialogue Properties

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> DialogueWidgetClass;

	//다이얼로그를 이용한 대화에서 사용되는 위젯의 레퍼런스입니다.
	UPROPERTY()
		class UDialogueWidgetBase* DialogueWidgetPtr;
	//다이얼로그 대화에 사용할 대화 매니저입니다.
	UPROPERTY(EditAnywhere, Category = Dialogue)
		class UDialogueManager* DialogueManager;

	//대화에 사용할 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere)
		class UAnimaleseSpeechAudioComponent* AnimaleseSpeechAudioComponent;

public:

	UFUNCTION(BlueprintCallable)
		void SetDialogueAsset(UDialogueManager* InDialogueManager);

	UFUNCTION(BlueprintCallable)
		void ClearDialogueAsset();

public:

	//보유하고 있는 다이얼로그를 시작합니다.
	UFUNCTION(BlueprintCallable)
		void StartDialogue();

	UFUNCTION(BlueprintCallable)
		void StartTransientDialogue(TMap<FName, UDialogueHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent = true);

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
