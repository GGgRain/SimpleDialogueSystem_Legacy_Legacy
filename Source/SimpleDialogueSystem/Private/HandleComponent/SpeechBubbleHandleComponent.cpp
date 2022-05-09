// Fill out your copyright notice in the Description page of Project Settings.


#include "HandleComponent/SpeechBubbleHandleComponent.h"
#include "DerivedWidget/SpeechBubbleWidget_Handled.h"
#include "Components/WidgetComponent.h"



// Sets default values for this component's properties
USpeechBubbleHandleComponent::USpeechBubbleHandleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


}


void USpeechBubbleHandleComponent::BeginPlay() {
	Super::BeginPlay();
}

void USpeechBubbleHandleComponent::SetDialogueAsset(UDialogueManager* InDialogueManager) {

	if (IsDialoguePlaying()) {
		return;
	}

	if (!InDialogueManager) {
		return;
	}

	DialogueManager = InDialogueManager;
}


void USpeechBubbleHandleComponent::ClearDialogueAsset() {

	if (IsDialoguePlaying()) {
		return;
	}

	DialogueManager = nullptr;
}


void USpeechBubbleHandleComponent::StartTransientDialogue(TMap<FName, USpeechBubbleHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent) {
	if (!DialogueManager) {
		return;
	}

	DialogueManager = DuplicateObject<UDialogueManager>(DialogueManager,this);

#if WITH_EDITOR
	ensure(DialogueManager);
#endif

	TArray<FName> UnitKeys;
	DialogueManager->DialogueUnitMap.GetKeys(UnitKeys);

	for (const FName& UnitName : UnitKeys) {
		for (FName& SwapTargetName : DialogueManager->DialogueUnitMap[UnitName]->SpeakerNames) {
			if (SwapMap.Contains(SwapTargetName)) {
				SwapTargetName = SwapMap[SwapTargetName]->IDName;
			}

			if (bShouldSwapSelfToThisComponent) {
				if (SwapTargetName.ToString().ToLower() == "self") {
					SwapTargetName = this->IDName;
				}
			}
		}

		for (FName& SwapTargetName : DialogueManager->DialogueUnitMap[UnitName]->ListenerNames) {
			if (SwapMap.Contains(SwapTargetName)) {
				SwapTargetName = SwapMap[SwapTargetName]->IDName;
			}

			if (bShouldSwapSelfToThisComponent) {
				if (SwapTargetName.ToString().ToLower() == "self") {
					SwapTargetName = this->IDName;
				}
			}
		}
	}

	StartDialogue();
	
}

void USpeechBubbleHandleComponent::StartDialogue() {
	if (SpeechBubbleWidgetPtr != nullptr) {
		CloseDialogue(); //기존의 다이얼로그를 강제로 제거합니다. (애니메이션 x, 즉시 삭제됨)
	}

	if (DialogueManager != nullptr) {

		SpeechBubbleWidgetPtr = CreateWidget<USpeechBubbleWidget_Handled>(GetWorld(), SpeechBubbleWidgetClass);
		if (SpeechBubbleWidgetPtr != nullptr) {

			USpeechBubbleWidget_Handled* CastedWidget = Cast<USpeechBubbleWidget_Handled>(SpeechBubbleWidgetPtr);
			if (CastedWidget) {
				CastedWidget->InitializeDialogueWidget(DialogueManager);

				CastedWidget->NativeConstruct();
				TMap<FName, USpeechBubbleHandleComponent*> Map;

				TArray<FName> UnitKeys;
				DialogueManager->DialogueUnitMap.GetKeys(UnitKeys);

				TArray<FName> ConversationCompName;
				for (FName UnitName : UnitKeys) {
					for (FName Name : DialogueManager->DialogueUnitMap[UnitName]->SpeakerNames) {
						if (!ConversationCompName.Contains(Name)) {
							ConversationCompName.Add(Name);
						}
					}
				}


				UWorld* World = GetWorld();
				for (TObjectIterator<USpeechBubbleHandleComponent> Itr; Itr; ++Itr) {
					if (Itr->GetWorld() == World) {
						if (ConversationCompName.Contains(Itr->IDName)) {
							Map.Add(Itr->IDName, *Itr);
						}
					}
				}

				CastedWidget->TryAddConversationComponents(Map);

				CastedWidget->SetFirstDialogueUnitBasedOnCurrentDialogue();

				CastedWidget->AddToViewport_Implemented();

				//CastedWidget->SetDesiredSizeInViewport(FVector2D(1000, 1000));
			}
		}
	}
}


void USpeechBubbleHandleComponent::EndDialogue() {
	if (SpeechBubbleWidgetPtr != nullptr) {
		if (SpeechBubbleWidgetPtr->IsValidLowLevel()) {
			SpeechBubbleWidgetPtr->CloseAction();
		}
	}
}


void USpeechBubbleHandleComponent::CloseDialogue() {
	if (SpeechBubbleWidgetPtr != nullptr) {
		if (SpeechBubbleWidgetPtr->IsValidLowLevel()) {
			SpeechBubbleWidgetPtr->RemoveFromParent();

			SpeechBubbleWidgetPtr = nullptr;
		}
	}
}


bool USpeechBubbleHandleComponent::IsDialoguePlaying() {
	if (SpeechBubbleWidgetPtr) {
		return true;
	}

	return false;
}


void USpeechBubbleHandleComponent::OnStringPlayed(UDialogueWidgetBase* Widget, FString String, float Duration, int32 CurrentIndex) {
	if (AnimaleseSpeechAudioComponent) {

		AnimaleseSpeechAudioComponent->PlaySoundForString(String, Duration, CurrentIndex);

	}
}


void USpeechBubbleHandleComponent::OnStringListened(UDialogueWidgetBase* Widget) {

}

void USpeechBubbleHandleComponent::OnCustomCommandCalled(FString Command) {

}