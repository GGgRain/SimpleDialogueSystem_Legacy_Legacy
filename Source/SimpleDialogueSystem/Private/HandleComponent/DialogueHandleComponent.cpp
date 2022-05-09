// Fill out your copyright notice in the Description page of Project Settings.


#include "HandleComponent/DialogueHandleComponent.h"
#include "DerivedWidget/DialogueWidget_Handled.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UDialogueHandleComponent::UDialogueHandleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UDialogueHandleComponent::SetDialogueAsset(UDialogueManager* InDialogueManager) {

	if (IsDialoguePlaying()) {
		return;
	}

	if (!InDialogueManager) {
		return;
	}

	DialogueManager = InDialogueManager;
}


void UDialogueHandleComponent::ClearDialogueAsset() {

	if (IsDialoguePlaying()) {
		return;
	}

	DialogueManager = nullptr;
}

void UDialogueHandleComponent::BeginPlay() {
	Super::BeginPlay();
}

void UDialogueHandleComponent::StartDialogue() {
	if (DialogueManager != nullptr) {
		DialogueWidgetPtr = CreateWidget<UDialogueWidget_Handled>(GetWorld(), DialogueWidgetClass);
		if (DialogueWidgetPtr != nullptr) {

			UDialogueWidget_Handled* CastedWidget = Cast<UDialogueWidget_Handled>(DialogueWidgetPtr); 
			if (CastedWidget) {
				CastedWidget->InitializeDialogueWidget(DialogueManager);

				CastedWidget->NativeConstruct();
				TMap<FName, UDialogueHandleComponent*> Map;

				TArray<FName> UnitKeys;
				DialogueManager->DialogueUnitMap.GetKeys(UnitKeys);

				TArray<FName> ConversationCompName;
				for (FName UnitName : UnitKeys) {
					for (FName Name : DialogueManager->DialogueUnitMap[UnitName]->SpeakerNames) {
						if (!ConversationCompName.Contains(Name)) {
							ConversationCompName.Add(Name);
						}
					}
					for (FName Name : DialogueManager->DialogueUnitMap[UnitName]->ListenerNames) {
						if (!ConversationCompName.Contains(Name)) {
							ConversationCompName.Add(Name);
						}
					}
				}

				UWorld* World = GetWorld();
				for (TObjectIterator<UDialogueHandleComponent> Itr; Itr; ++Itr) {
					if (Itr->GetWorld() == World) {
						if (ConversationCompName.Contains(Itr->IDName)) {
							Map.Add(Itr->IDName, *Itr);
						}
					}
				}

				CastedWidget->TryAddConversationComponents(Map);

				CastedWidget->SetFirstDialogueUnitBasedOnCurrentDialogue();

				CastedWidget->AddToViewport_Implemented();

			}
		}
	}
}

void UDialogueHandleComponent::StartTransientDialogue(TMap<FName, UDialogueHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent) {
	if (!DialogueManager) {
		return;
	}

	DialogueManager = DuplicateObject<UDialogueManager>(DialogueManager, this);

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


bool UDialogueHandleComponent::IsDialoguePlaying() {
	if (DialogueWidgetPtr) {
		return true;
	}

	return false;
}

void UDialogueHandleComponent::OnStringPlayed(UDialogueWidgetBase* Widget, FString String, float Duration, int32 CurrentIndex) {
	if (AnimaleseSpeechAudioComponent) {

		AnimaleseSpeechAudioComponent->PlaySoundForString(String, Duration, CurrentIndex);

	}
}

void UDialogueHandleComponent::OnStringListened(UDialogueWidgetBase* Widget) {

}

void UDialogueHandleComponent::OnCustomCommandCalled(FString Command) {

}