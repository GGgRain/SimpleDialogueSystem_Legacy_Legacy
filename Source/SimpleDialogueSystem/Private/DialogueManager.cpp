// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueManager.h"
#include "UObject/Object.h"

UDialogueUnitBase* UDialogueManager::GetDialogueUnit(FName DialogueUnitKey) {
	if (DialogueUnitMap.Contains(DialogueUnitKey)) {
		return DialogueUnitMap[DialogueUnitKey];
	}
	else {
		return nullptr;
	}
}

bool UDialogueManager::HasDialogueUnit(FName DialogueUnitKey) {
	return DialogueUnitMap.Contains(DialogueUnitKey);
}

#if WITH_EDITOR

UDialogueUnitBase* UDialogueManager::AddDialogueUnit() {

	UDialogueUnitBase* NewUnit = NewObject<UDialogueUnitBase>(this, UDialogueUnitBase::StaticClass());
	if (NewUnit) {

		NewUnit->ContextText = FText::ChangeKey(FString::FromInt(this->GetUniqueID()), NewUnit->GetName(), FText::GetEmpty());
		DialogueUnitMap.Add(NAME_None, NewUnit);
		this->MarkPackageDirty();
		return NewUnit;
	}


	return nullptr;
}

UDialogueUnitBase* UDialogueManager::AddSelectDialogueUnit() {

	UDialogueUnit_Select* NewUnit = NewObject<UDialogueUnit_Select>(this, UDialogueUnit_Select::StaticClass());
	if (NewUnit) {
		
		DialogueUnitMap.Add(NAME_None, NewUnit);
		this->MarkPackageDirty();
		return NewUnit;
	}

	return nullptr;
}

UDialogueUnitBase* UDialogueManager::AddSwitchDialogueUnit() {

	UDialogueUnit_Switch* NewUnit = NewObject<UDialogueUnit_Switch>(this, UDialogueUnit_Switch::StaticClass());
	if (NewUnit) {

		DialogueUnitMap.Add(NAME_None, NewUnit);
		this->MarkPackageDirty();
		return NewUnit;
	}

	return nullptr;
}
#endif



#if WITH_EDITOR

void UDialogueUnitBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	ContextText = FText::ChangeKey("DialogueManagerID:" + FString::FromInt(this->GetOuter()->GetUniqueID()), this->GetName(), ContextText);
}

void UDialogueUnit_Select::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	ContextText = FText::ChangeKey("DialogueManagerID:" + FString::FromInt(this->GetOuter()->GetUniqueID()), this->GetName(), ContextText);
}

void UDialogueUnit_Select::AddEmptyOption() {

	UDialogueUnit_Select_Option* NewUnit = NewObject<UDialogueUnit_Select_Option>(this, UDialogueUnit_Select_Option::StaticClass());

	if (NewUnit) {

		Options.Add(NewUnit);
		this->MarkPackageDirty();
	}
}

void UDialogueUnit_Select_Option::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	AnswerContextText = FText::ChangeKey("DialogueManagerID:" + FString::FromInt(this->GetOuter()->GetOuter()->GetUniqueID()), FString::FromInt(this->GetOuter()->GetUniqueID()) + "_" + this->GetName(), AnswerContextText);
}

void UDialogueUnit_Switch::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	ContextText = FText::ChangeKey("DialogueManagerID:" + FString::FromInt(this->GetOuter()->GetUniqueID()), this->GetName(), ContextText);
}

#endif


void UDialogueManager::GetNotifiedOnDialogueUnitBegin(FName DialogueUnitKey) {
	if (DialogueUnitMap.Contains(DialogueUnitKey)) {
		//PlayingDialogueUnit = DialogueUnitName;
	}
}

void UDialogueManager::GetNotifiedOnDialogueUnitEnd(FName DialogueUnitKey) {
	if (DialogueUnitMap.Contains(DialogueUnitKey)) {
		if (DialogueUnitMap.Contains(DialogueUnitMap[DialogueUnitKey]->NextDialogueKey)) {
			DialogueUnitMap[DialogueUnitMap[DialogueUnitKey]->NextDialogueKey];
		}
	}
}
