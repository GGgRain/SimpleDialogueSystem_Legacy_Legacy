// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimaleseSpeechAudioComponent.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"
#include "CharTransliterator.h"

UAnimaleseSpeechAudioComponent::UAnimaleseSpeechAudioComponent() {
	//keys = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'th', 'sh', ' ', '.']
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundSet(TEXT("SoundCue'/SimpleDialogueSystem/Animalese/SpeechSoundSet.SpeechSoundSet'"));
	if (SoundSet.Succeeded()) {
		this->SetSound(SoundSet.Object);
	}
	LetterSoundMap = { //맵핑된 데이터입니다. 최적화가 문제가 된다면 다른 방법으로 맵을 정의하세요.
		{"a",0},
		{"b",1},
		{"c",2},
		{"d",3},
		{"e",4},
		{"f",5},
		{"g",6},
		{"h",7},
		{"i",8},
		{"j",9},
		{"k",10},
		{"l",11},
		{"m",12},
		{"n",13},
		{"o",14},
		{"p",15},
		{"q",16},
		{"r",17},
		{"s",18},
		{"t",19},
		{"u",20},
		{"v",21},
		{"w",22},
		{"x",23},
		{"y",24},
		{"z",25},
		{"th",26},
		{"sh",27},
		{".",28},
	};

	this->bAutoActivate = false;
}

void UAnimaleseSpeechAudioComponent::PlaySoundForString(FString String, float Duration, int32 CurrentIndex) {
	if (String.IsValidIndex(CurrentIndex)) {
		CharTransliterator Trans = CharTransliterator();
		
		FString LeftCharacter = (String.IsValidIndex(CurrentIndex - 1)) ? String.Mid(CurrentIndex - 1, 1) : FString();
		FString CurrentCharacter = (String.IsValidIndex(CurrentIndex)) ? String.Mid(CurrentIndex, 1) : FString();
		FString RightCharacter = (String.IsValidIndex(CurrentIndex + 1)) ? String.Mid(CurrentIndex + 1, 1) : FString();

		LeftCharacter = Trans.TransliterateString(LeftCharacter);
		CurrentCharacter = Trans.TransliterateString(CurrentCharacter);
		RightCharacter = Trans.TransliterateString(RightCharacter);

		InternalIndexCounter = LeftCharacter.Len(); // 이전에 검사했던 글자 이후 순서부터 검사합니다.

		CurrentTransliteratedCharacters = LeftCharacter + CurrentCharacter + RightCharacter;

		//UE_LOG(LogTemp, Log, TEXT("CurrentTransliteratedCharacters : %s + %s + %s"), *LeftCharacter, *CurrentCharacter ,*RightCharacter);

		FTimerManager* ManagerRef = &GetWorld()->GetTimerManager();
		if (ManagerRef != nullptr) {
			int CurrentSpeechLength = CurrentCharacter.Len();
			ManagerRef->ClearTimer(InternalIndexTimerhandle);
			//단어가 3글자면 음성 실행은 전체
			ManagerRef->SetTimer(InternalIndexTimerhandle, this, &UAnimaleseSpeechAudioComponent::PlaySoundForPreparedWord, Duration / float(CurrentSpeechLength), 0, 0);
		}
	}
}

void UAnimaleseSpeechAudioComponent::PlaySoundForPreparedWord() {
	int32 Len = CurrentTransliteratedCharacters.Len();

	if (InternalIndexCounter < Len) {

		const FString LeftCharacter = (CurrentTransliteratedCharacters.IsValidIndex(InternalIndexCounter - 1)) ? CurrentTransliteratedCharacters.Mid(InternalIndexCounter - 1, 1) : FString();
		const FString CurrentCharacter = (CurrentTransliteratedCharacters.IsValidIndex(InternalIndexCounter)) ? CurrentTransliteratedCharacters.Mid(InternalIndexCounter, 1) : FString();
		const FString RightCharacter = (CurrentTransliteratedCharacters.IsValidIndex(InternalIndexCounter + 1)) ? CurrentTransliteratedCharacters.Mid(InternalIndexCounter + 1, 1) : FString();

		LeftCharacter.ToLower();
		CurrentCharacter.ToLower();
		RightCharacter.ToLower();

		float TempVoicePitch = VoicePitch;
		float TempVoiceVolume = VoiceVolume;

		bool bShouldUpdateSound = true;

		if (LeftCharacter == CurrentCharacter) {
			bShouldUpdateSound = false;
		}
		else if (CurrentCharacter == "s" && LeftCharacter == "h") {//'sh' sound
			this->SetIntParameter(FName("Num"), LetterSoundMap["sh"]);
		}
		else if (CurrentCharacter == "t" && (LeftCharacter == "h" || LeftCharacter == "H")) {//'th' sound
			this->SetIntParameter(FName("Num"), LetterSoundMap["th"]);
		}
		else if (CurrentCharacter == "h" && (RightCharacter == "t" || RightCharacter == "s")) {//if previous letter was 's' or 's' and current letter is 'h'
			//사운드 안냄.
			bShouldUpdateSound = false;
		}
		else if (CurrentCharacter == "," || CurrentCharacter == "?") {
			//사운드 안냄.
			bShouldUpdateSound = false;
		}
		else {
			if (LetterSoundMap.Contains(CurrentCharacter)) {
				this->SetIntParameter(FName("Num"), LetterSoundMap[CurrentCharacter]);
			}
			else {
				bShouldUpdateSound = false;
			}
		}

		if (RightCharacter == "?" && CurrentCharacter != FString()) {
			//UE_LOG(LogTemp, Log, TEXT("'?' token Detected!"));
			TempVoicePitch = TempVoicePitch * 1.1;
			TempVoiceVolume = TempVoiceVolume * 1.1;
		}


		this->SetPitchMultiplier(TempVoicePitch);
		this->SetVolumeMultiplier(TempVoiceVolume);

		if (bShouldUpdateSound) {
			this->Play();
		}

		InternalIndexCounter++;
	}
	else {
		FTimerManager* ManagerRef = &GetWorld()->GetTimerManager();
		if (ManagerRef != nullptr) {
			ManagerRef->ClearTimer(InternalIndexTimerhandle);
		}
	}
}