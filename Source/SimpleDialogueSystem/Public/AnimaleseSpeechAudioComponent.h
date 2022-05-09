// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "AnimaleseSpeechAudioComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLEDIALOGUESYSTEM_API UAnimaleseSpeechAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
	

	UAnimaleseSpeechAudioComponent();

public:

	UPROPERTY(EditAnywhere)
		float VoicePitch = 1;

	UPROPERTY(EditAnywhere)
		float VoiceVolume = 1;
	//발음중에 두글자 이상으로 판정되는거도 있어서 스트링으로 처리함.
	UPROPERTY()
		TMap<FString, int> LetterSoundMap;

public:
	UPROPERTY()
		FString CurrentTransliteratedCharacters;

	UPROPERTY()
		int InternalIndexCounter = 0;

	UPROPERTY()
		FTimerHandle InternalIndexTimerhandle;

	UFUNCTION()
		void PlaySoundForString(FString String, float Duration, int32 CurrentIndex);

	UFUNCTION()
		void PlaySoundForPreparedWord();



};

