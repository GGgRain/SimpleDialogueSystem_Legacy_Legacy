// Fill out your copyright notice in the Description page of Project Settings.


#include "DerivedWidget/SpeechBubbleWidget_Handled.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Border.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpringArmComponent.h"


#include "Particles/ParticleSystem.h"
#include "AnimaleseSpeechAudioComponent.h"

#include "UObject/ConstructorHelpers.h"


#include "HandleComponent/SpeechBubbleHandleComponent.h"

#include "Kismet/GameplayStatics.h"


#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif


USpeechBubbleWidget_Handled::USpeechBubbleWidget_Handled() {

}

void USpeechBubbleWidget_Handled::UpdateText() {
	Super::UpdateText();

	for (FName Name : CurrentPlayingDialogueUnit->SpeakerNames) {
		if (ConversationComponents.Contains(Name)) {

			ConversationComponents[Name]->OnStringPlayed(this,ContextBlockText.ToString(), TextUpdateFrequency, NumberOfShowingChar - 1);

		}
	}

	for (FName Name : CurrentPlayingDialogueUnit->ListenerNames) {
		if (ConversationComponents.Contains(Name)) {

			ConversationComponents[Name]->OnStringListened(this);

		}
	}
}


void USpeechBubbleWidget_Handled::CallCommand(FString Command) {

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

FReply USpeechBubbleWidget_Handled::TryCallCommand_Dialogue(FString Command) {
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
FReply USpeechBubbleWidget_Handled::TryCallCommand_Handler(FString Command) {

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
FReply USpeechBubbleWidget_Handled::TryCallCommand_Game(FString Command) {

	FReply Result = FReply::Unhandled();

	FString ArrangedCammandLine = Command;
	ArrangedCammandLine = ArrangedCammandLine.Replace(TEXT("="), TEXT(" "), ESearchCase::IgnoreCase);
	ArrangedCammandLine = ArrangedCammandLine.Replace(TEXT(","), TEXT(" "), ESearchCase::IgnoreCase);
	UKismetSystemLibrary::ExecuteConsoleCommand(this, ArrangedCammandLine);

	return Result;

}


void USpeechBubbleWidget_Handled::SetPlayerCameraBetweenTalkingComponents() {
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

					}

				}
			}
		}
	}
}

void USpeechBubbleWidget_Handled::SetPlayerCameraLocationToLastLoc() {
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

void USpeechBubbleWidget_Handled::AdjustRenderingLocation() {
	if (CurrentPlayingDialogueUnit) {

		TArray<FTransform> TFs;

		for (FName& TestKey : CurrentPlayingDialogueUnit->SpeakerNames) {

			if (ConversationComponents.Contains(TestKey)) {

				if (AActor* Owner = ConversationComponents[TestKey]->GetOwner()) {

					bool bFound = false;

					if (ConversationComponents[TestKey]->WidgetAttachTargetComponentName != NAME_None) {

						TArray<UActorComponent*> Comps = Owner->GetComponentsByClass(USceneComponent::StaticClass());

						for (UActorComponent* TestComp : Comps) {

							if (TestComp->GetFName() == ConversationComponents[TestKey]->WidgetAttachTargetComponentName) {
								FTransform TF = Cast<USceneComponent>(TestComp)->GetComponentTransform();
								TF.AddToTranslation(ConversationComponents[TestKey]->WidgetOffset);
								TFs.Add(TF);

								bFound = true;
								break;
							}
						}
					}

					if(!bFound) {
						FTransform TF = Owner->GetActorTransform();
						TF.AddToTranslation(ConversationComponents[TestKey]->WidgetOffset);
						TFs.Add(TF);
					}
				}
			}

		}

		FTransform WidgetTransform;

		int Num = TFs.Num();

		if (Num > 0) {
			for (int Index = 0; Index < Num - 1; ++Index) {
				TFs[Index + 1].BlendWith(TFs[Index], 0.5);
			}

			WidgetTransform = TFs[Num - 1];

			FVector2D ScreenLocation;

			UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this,0),WidgetTransform.GetLocation(), ScreenLocation,false);
			int ViewportSizeX, ViewportSizeY;
			UGameplayStatics::GetPlayerController(this, 0)->GetViewportSize(ViewportSizeX, ViewportSizeY);

			//UE_LOG(LogTemp, Log, TEXT("%f,%f"), ScreenLocation.X, ScreenLocation.Y);
			
			/*
			ScreenLocation.Set(
				FMath::Clamp<int>(int(ScreenLocation.X), 0 + DisplayMargin * (ViewportSizeX /2), (ViewportSizeX / 2) - DisplayMargin * (ViewportSizeX / 2)),
				FMath::Clamp<int>(int(ScreenLocation.Y), 0 + DisplayMargin * (ViewportSizeY / 2), (ViewportSizeY / 2) - DisplayMargin * (ViewportSizeY / 2))
			);
			*/
		
			ScreenLocation.Set(
				FMath::Clamp<int>(int(ScreenLocation.X), 0 + DisplayMargin * ViewportSizeX, (ViewportSizeX) - DisplayMargin * ViewportSizeX),
				FMath::Clamp<int>(int(ScreenLocation.Y), 0 + DisplayMargin * ViewportSizeY, (ViewportSizeY) - DisplayMargin * ViewportSizeY)
			);
			


			SetRenderTransform(FWidgetTransform((ScreenLocation - FVector2D(ViewportSizeX / 2, ViewportSizeY / 2)) * 2, FVector2D(1,1),FVector2D(0,0),0));

		}
		else {

			UE_LOG(LogTemp, Warning, TEXT("Widget %s : There were no speakers in this dialogue! "), *this->GetFName().ToString());
		}

	}
}

void USpeechBubbleWidget_Handled::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	AdjustRenderingLocation();

	Super::NativeTick(MyGeometry, InDeltaTime);
}


void USpeechBubbleWidget_Handled::CollectLastPlayerCameraLocation() {
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

void USpeechBubbleWidget_Handled::TryAddConversationComponents(TMap<FName, USpeechBubbleHandleComponent*> InConversationComponents) {
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


void USpeechBubbleWidget_Handled::OnDialogueWidgetBegin() {

	Super::OnDialogueWidgetBegin();

	AdjustParentNameBorderWidget();
}

void USpeechBubbleWidget_Handled::AdjustParentNameBorderWidget() {
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

void USpeechBubbleWidget_Handled::OnNextDialoguePlayed() {

	Super::OnNextDialoguePlayed();

	AdjustParentNameBorderWidget();
}

void USpeechBubbleWidget_Handled::OnDialogueWidgetEnd() {

	Super::OnDialogueWidgetEnd();
}



void USpeechBubbleWidget_Handled::OpenAction() {
	if (AnimationHandle.IsBound()) {
		AnimationHandle.Broadcast(FName("OpenAnim"));
	}
}

void USpeechBubbleWidget_Handled::CloseAction() {
	Super::CloseAction();

	if (AnimationHandle.IsBound()) {
		AnimationHandle.Broadcast(FName("CloseAnim"));
	}
}

void USpeechBubbleWidget_Handled::UpdateSpeakerBlockTextBasedOnCurrentDialogue() {
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
