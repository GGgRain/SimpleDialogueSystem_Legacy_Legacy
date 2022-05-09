// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueManagerFactory.h"
#include "DialogueManager.h"


UDialogueManagerFactory::UDialogueManagerFactory(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDialogueManager::StaticClass();
}

UObject* UDialogueManagerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDialogueManager* NewDialogueManager = NewObject<UDialogueManager>(InParent, Class, Name, Flags | RF_Transactional);

	return NewDialogueManager;
}

bool UDialogueManagerFactory::ShouldShowInNewMenu() const {
	return true;
}
