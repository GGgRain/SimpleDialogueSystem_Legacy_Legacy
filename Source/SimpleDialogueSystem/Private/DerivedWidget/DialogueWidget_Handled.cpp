// Fill out your copyright notice in the Description page of Project Settings.


#include "DerivedWidget/DialogueWidget_Handled.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Border.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpringArmComponent.h"


#include "Particles/ParticleSystem.h"
#include "AnimaleseSpeechAudioComponent.h"


#include "UObject/ConstructorHelpers.h"

#include "HandleComponent/DialogueHandleComponent.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif


UDialogueWidget_Handled::UDialogueWidget_Handled() {
	
}

void UDialogueWidget_Handled::UpdateText() {
	Super::UpdateText();

	for (FName Name : CurrentPlayingDialogueUnit->SpeakerNames) {
		if (ConversationComponents.Contains(Name)) {
			
			ConversationComponents[Name]->OnStringPlayed(this,ContextBlockText.ToString(), TextUpdateFrequency , NumberOfShowingChar - 1);

		}
	}


	for (FName Name : CurrentPlayingDialogueUnit->ListenerNames) {
		if (ConversationComponents.Contains(Name)) {

			ConversationComponents[Name]->OnStringListened(this);

		}
	}
}


void UDialogueWidget_Handled::CallCommand(FString Command) {

	if (!TryCallCommand_Dialogue(Command).IsEventHandled()) {
		if (!TryCallCommand_Handler(Command).IsEventHandled()) {

			TryCallCommand_Game(Command).IsEventHandled();

#if WITH_EDITOR
			const FText notificationText = FText::FromString(FString("Unknown Command Line " + Command + " Called In Dialogue! - We passed it to console, please check whether it has been executed correctly"));
			FNotificationInfo info(notificationText);
			info.bFireAndForget = true;
			info.FadeInDuration = 0.5f;
			info.FadeOutDuration = 1.0f;
			info.ExpireDuration = 3.0f;

			FSlateNotificationManager::Get().AddNotification(info);
#endif

		}
	}
}

FReply UDialogueWidget_Handled::TryCallCommand_Dialogue(FString Command) {
	FReply Result = FReply::Unhandled();

	if (Command.StartsWith("Speed=")) {
		FString Str = "0";
		Command.Split("=", nullptr, &Str);
		TextUpdateFrequency = FCString::Atof(*Str.TrimQuotes());
		StartUpdateText();

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("Sound=")) { // 얘는 그냥 남겨둠
		FString Str = "0";
		Command.Split("=", nullptr, &Str);
		USoundCue* SoundCue = LoadObject<USoundCue>(this, *Str);
		UGameplayStatics::PlaySound2D(this, SoundCue);

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("StartSelect")) {
		if (CurrentPlayingDialogueUnit != nullptr) {
			UDialogueUnit_Select* Unit = Cast<UDialogueUnit_Select>(CurrentPlayingDialogueUnit);
			if (Unit != nullptr) {
				AddSelectOptionButtonOnWidget();
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("%s->%s->{{StartSelect}} Keyword Failed! | The unit '%s' is not compatible with 'UDialogueUnit_Select'"), *this->GetFName().ToString(), *this->CurrentPlayingDialogueUnit->GetFName().ToString(), *this->CurrentPlayingDialogueUnit->GetFName().ToString());
			}
		}

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("StopDialogue")) {
		this->RemoveFromParent();

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("CloseDialogue")) {
		CloseAction();

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("Event=")) {

		FString Str = "0";
		Command.Split("=", nullptr, &Str);

		if (!CurrentPlayingDialogueUnit) {
			return Result;
		}

		if (CurrentPlayingDialogueUnit->Events.Contains(FName(Str))) {
			CurrentPlayingDialogueUnit->Events[FName(Str)]->ReceiveEventTrigger();
		}

		Result = FReply::Handled();
	}
	else if (Command.StartsWith("//")) {
		bShouldAppendEndDecorator = !bShouldAppendEndDecorator;

		Result = FReply::Handled();
	}

	return Result;

}
FReply UDialogueWidget_Handled::TryCallCommand_Handler(FString Command) {

	FReply Result = FReply::Unhandled();

	if (Command.StartsWith("ID:")) { //ID:'아이디',명령어이름,명령줄인수1,명령줄인수2... -> 핸들 컴포넌트에서 처리해야하는 커맨드를 넘겨줌.
		FString ID = "NULL";
		FString CommandText = "NULL";

		Command.Split("ID:", nullptr, &ID);
		ID.Split(",", &ID, &CommandText);

		if (ConversationComponents.Contains(FName(ID))) {
			ConversationComponents[FName(ID)]->OnCustomCommandCalled(CommandText);
		}

		Result = FReply::Handled();
	}

	return Result;
}
FReply UDialogueWidget_Handled::TryCallCommand_Game(FString Command) {

	FReply Result = FReply::Unhandled();

	FString ArrangedCammandLine = Command;
	ArrangedCammandLine = ArrangedCammandLine.Replace(TEXT("="), TEXT(" "), ESearchCase::IgnoreCase);
	ArrangedCammandLine = ArrangedCammandLine.Replace(TEXT(","), TEXT(" "), ESearchCase::IgnoreCase);
	UKismetSystemLibrary::ExecuteConsoleCommand(this, ArrangedCammandLine);

	return Result;
	
}


void UDialogueWidget_Handled::SetPlayerCameraBetweenTalkingComponents() {
	if (ConversationComponents.Contains("Player")) {
		if (ConversationComponents["Player"] != nullptr) {
			if (ConversationComponents["Player"]->IsValidLowLevel()) {
				
				AActor* Player = ConversationComponents["Player"]->GetOwner();
				if (Player != nullptr) {
					FVector CameraLocation = FVector(0, 0, 0);
					FVector PlayerLoc = Player->GetActorLocation();
					float CameraDistance = 0;

					for (FName Name : CurrentPlayingDialogueUnit->SpeakerNames) {

						if (ConversationComponents.Contains(Name)) {
							if (ConversationComponents[Name] != nullptr) {
								if (ConversationComponents[Name]->IsValidLowLevel()) {
									AActor* Owner = ConversationComponents[Name]->GetOwner();
									if (Owner != nullptr) {
										FVector Loc = (Owner->GetActorLocation() - PlayerLoc) / 2;
										CameraDistance += Loc.Size();
										CameraLocation += Loc;
									}
								}
							}
						}
					}

					for (FName Name : CurrentPlayingDialogueUnit->ListenerNames) {

						if (ConversationComponents.Contains(Name)) {
							if (ConversationComponents[Name] != nullptr) {
								if (ConversationComponents[Name]->IsValidLowLevel()) {
									AActor* Owner = ConversationComponents[Name]->GetOwner();
									if (Owner != nullptr) {
										FVector Loc = (Owner->GetActorLocation() - PlayerLoc) / 2;
										CameraDistance += Loc.Size();
										CameraLocation += Loc;
									}
								}
							}
						}
					}
					UActorComponent* SpringArmComp_Uncasted = Player->GetComponentByClass(USpringArmComponent::StaticClass());
					if (SpringArmComp_Uncasted != nullptr) {
						USpringArmComponent* SpringArmComp_Pure = Cast<USpringArmComponent>(SpringArmComp_Uncasted);

						SpringArmComp_Pure->SetRelativeLocation(
							CameraLocation
							+
							FVector(0, (CameraDistance * 2), 0)
						);

						FVector AdjLoc = CameraLocation + Player->GetActorLocation();

						/*
						for (FName Name : CurrentPlayingDialogueUnit->SpeakerNames) {

							if (ConversationComponents.Contains(Name)) {
								if (ConversationComponents[Name] != nullptr) {
									if (ConversationComponents[Name]->IsValidLowLevel()) {
										
										AActor* Owner = ConversationComponents[Name]->GetOwner();
										if (Owner != nullptr) {
											if (ConversationComponents[Name]->bShouldTurnOverToTalkingLocation) {
												if (ConversationComponents[Name]->ComponentToTurnOver != nullptr) {
													FVector Loc = AdjLoc - Owner->GetActorLocation();
													(Loc.X < 0) ? ConversationComponents[Name]->ComponentToTurnOver->SetRelativeRotation(FRotator(0, 180, 0)) : ConversationComponents[Name]->ComponentToTurnOver->SetRelativeRotation(FRotator(0, 0, 0));
												}
											}
										}
									}
								}
							}
						}

						for (FName Name : CurrentPlayingDialogueUnit->ListenerNames) {

							if (ConversationComponents.Contains(Name)) {
								if (ConversationComponents[Name] != nullptr) {
									if (ConversationComponents[Name]->IsValidLowLevel()) {

										AActor* Owner = ConversationComponents[Name]->GetOwner();
										if (Owner != nullptr) {
											if (ConversationComponents[Name]->bShouldTurnOverToTalkingLocation) {
												if (ConversationComponents[Name]->ComponentToTurnOver != nullptr) {
													FVector Loc = AdjLoc - Owner->GetActorLocation();
													(Loc.X < 0) ? ConversationComponents[Name]->ComponentToTurnOver->SetRelativeRotation(FRotator(0, 180, 0)) : ConversationComponents[Name]->ComponentToTurnOver->SetRelativeRotation(FRotator(0, 0, 0));
												}
											}
										}
									}
								}
							}
						}

						*/
					}
					
				}
			}
		}
	}
}

void UDialogueWidget_Handled::SetPlayerCameraLocationToLastLoc() {
	if (ConversationComponents.Contains("Player")) {

		if (ConversationComponents["Player"] != nullptr) {

			if (ConversationComponents["Player"]->IsValidLowLevel()) {

				AActor* Player = ConversationComponents["Player"]->GetOwner();
				if (Player != nullptr) {

					UActorComponent* SpringArmComp_Uncasted = Player->GetComponentByClass(USpringArmComponent::StaticClass());
					if (SpringArmComp_Uncasted != nullptr) {
						USpringArmComponent* SpringArmComp_Pure = Cast<USpringArmComponent>(SpringArmComp_Uncasted);

						SpringArmComp_Pure->SetRelativeLocation(LastPlayerCameraBoomLocation);
					}
				}
			}
		}
	}
}

void UDialogueWidget_Handled::CollectLastPlayerCameraLocation() {
	if (ConversationComponents.Contains("Player")) {

		if (ConversationComponents["Player"] != nullptr) {

			if (ConversationComponents["Player"]->IsValidLowLevel()) {

				AActor* Player = ConversationComponents["Player"]->GetOwner();
				if (Player != nullptr) {

					UActorComponent* SpringArmComp_Uncasted = Player->GetComponentByClass(USpringArmComponent::StaticClass());
					if (SpringArmComp_Uncasted != nullptr) {
						USpringArmComponent* SpringArmComp_Pure = Cast<USpringArmComponent>(SpringArmComp_Uncasted);

						LastPlayerCameraBoomLocation = SpringArmComp_Pure->GetRelativeLocation();
					}
				}
			}
		}
	}
}

void UDialogueWidget_Handled::TryAddConversationComponents(TMap<FName, UDialogueHandleComponent*> InConversationComponents) {
	TArray<FName> Arr; InConversationComponents.GetKeys(Arr);
	for (FName NpcName : Arr) {
		if (InConversationComponents.Contains(NpcName) && !ConversationComponents.Contains(NpcName)) {
			if (InConversationComponents[NpcName] != nullptr) {
				if (InConversationComponents[NpcName]->IsValidLowLevel()) {
					this->ConversationComponents.Add(NpcName, InConversationComponents[NpcName]);
				}
			}
		}
	}
}


void UDialogueWidget_Handled::OnDialogueWidgetBegin() {

	Super::OnDialogueWidgetBegin();

	CollectLastPlayerCameraLocation();
	SetPlayerCameraBetweenTalkingComponents();
	AdjustParentNameBorderWidget();
}

void UDialogueWidget_Handled::AdjustParentNameBorderWidget() {
	UBorder* ParentBorder = Cast<UBorder>(ContextBlock->GetParent());

	if (ParentBorder) {
		if (ContextBlock->GetText().ToString() == FString()) {
			ParentBorder->SetRenderOpacity(0);
		}
		else {
			ParentBorder->SetRenderOpacity(1);
		}
	}
}

void UDialogueWidget_Handled::OnNextDialoguePlayed() {

	Super::OnNextDialoguePlayed();

	SetPlayerCameraBetweenTalkingComponents();
	AdjustParentNameBorderWidget();
}

void UDialogueWidget_Handled::OnDialogueWidgetEnd() {

	SetPlayerCameraLocationToLastLoc();

	Super::OnDialogueWidgetEnd();
}



void UDialogueWidget_Handled::OpenAction() {
	if (AnimationHandle.IsBound()) {
		AnimationHandle.Broadcast(FName("OpenAnim"));
	}
}

void UDialogueWidget_Handled::CloseAction() {
	Super::CloseAction();

	if (AnimationHandle.IsBound()) {
		AnimationHandle.Broadcast(FName("CloseAnim"));
	}
}

void UDialogueWidget_Handled::UpdateSpeakerBlockTextBasedOnCurrentDialogue() {
	if (CurrentPlayingDialogueUnit != nullptr) {
		if (CurrentPlayingDialogueUnit->IsValidLowLevel()) {
			TArray<FName> SpeakerNames;
			SpeakerNames = CurrentPlayingDialogueUnit->SpeakerNames;
			FString FullNameString = FString();
			for (int i = 0; i < SpeakerNames.Num(); i++) {

				if (ConversationComponents.Contains(SpeakerNames[i])) {
					if (ConversationComponents[SpeakerNames[i]]->DisplayName.ToString() != FString()) {
						FullNameString.Append(ConversationComponents[SpeakerNames[i]]->DisplayName.ToString());

						if (i != 0) {
							FullNameString.Append(",");
						}
					}
				}
			}
			OwnerNameBlock->SetText(FText::FromString(FullNameString));
		}
	}
}
