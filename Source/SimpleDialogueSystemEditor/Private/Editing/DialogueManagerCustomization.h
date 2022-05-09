// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class UDialogueManager;

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

class FDialogueManagerCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	UDialogueManager* GetFirstSelectedAsset() const;

protected:

	
	UFUNCTION()
		FReply AddDialogueUnit();

	UFUNCTION()
		FReply AddSelectUnit();

	UFUNCTION()
		FReply AddSwitchUnit();

	

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

};
