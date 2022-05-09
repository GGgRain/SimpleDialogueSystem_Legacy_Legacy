// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DialogueManagerEditorActions.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "DialogueManager.h"
#include "Styling/SlateStyle.h"

#define LOCTEXT_NAMESPACE "DialogueManagerEditorActions"


/* FTextAssetActions constructors
 *****************************************************************************/

FDialogueManagerEditorActions::FDialogueManagerEditorActions(const TSharedRef<ISlateStyle>& InStyle)
	: Style(InStyle)
{ }


/* FAssetTypeActions_Base overrides
 *****************************************************************************/

bool FDialogueManagerEditorActions::CanFilter()
{
	return true;
}


void FDialogueManagerEditorActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);

	auto Assets = GetTypedWeakObjectPtrs<UDialogueManager>(InObjects);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DialogueManager_AddDialogueUnit", "Add Base Dialogue Unit"),
		LOCTEXT("DialogueManager_AddDialogueUnitToolTip", "Add Base dialogue unit in the dialogue unit map"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([=]{
				for (auto& Asset : Assets)
				{
					if (Asset.IsValid()/* && !Asset->Text.IsEmpty() */)
					{
						Asset->AddDialogueUnit();
					}
				}
			})
		)
	);


	MenuBuilder.AddMenuEntry(
		LOCTEXT("DialogueManager_AddSelectDialogueUnit", "Add Select Dialogue Unit"),
		LOCTEXT("DialogueManager_AddSelectDialogueUnitToolTip", "Add Select dialogue unit in the dialogue unit map"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([=] {
				for (auto& Asset : Assets)
				{
					if (Asset.IsValid()/* && !Asset->Text.IsEmpty() */)
					{
						Asset->AddSelectDialogueUnit();
					}
				}
				})
		)
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DialogueManager_AddSelectDialogueUnit", "Add Switch Dialogue Unit"),
		LOCTEXT("DialogueManager_AddSelectDialogueUnitToolTip", "Add Switch dialogue unit in the dialogue unit map"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([=] {
				for (auto& Asset : Assets)
				{
					if (Asset.IsValid()/* && !Asset->Text.IsEmpty() */)
					{
						Asset->AddSwitchDialogueUnit();
					}
				}
				})
		)
	);
}


uint32 FDialogueManagerEditorActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}


FText FDialogueManagerEditorActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DialogueManagerEditor", "Dialogue Manager Editor");
}


UClass* FDialogueManagerEditorActions::GetSupportedClass() const
{
	return UDialogueManager::StaticClass();
}


FColor FDialogueManagerEditorActions::GetTypeColor() const
{
	return FColor::Blue;
}


bool FDialogueManagerEditorActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

/*

void FDialogueManagerEditorActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{

	UE_LOG(LogTemp, Warning, TEXT("FDialogueManagerEditorActions / OpenAssetEditor Called!"));

	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
		? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Asset = Cast<UPaperDungeonRoom>(*ObjIt);

		if (Asset != nullptr)
		{
			TSharedRef<FDialogueManagerEditorEditorToolkit> EditorToolkit = MakeShareable(new FDialogueManagerEditorEditorToolkit(Style));
			EditorToolkit->Initialize(Asset, Mode, EditWithinLevelEditor);
		}
	}
}


*/


#undef LOCTEXT_NAMESPACE
