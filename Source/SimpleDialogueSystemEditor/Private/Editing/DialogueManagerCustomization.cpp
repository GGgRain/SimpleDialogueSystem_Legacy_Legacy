// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/DialogueManagerCustomization.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Application/SlateWindowHelper.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "IDetailsView.h"

#include "DialogueManager.h"




#define LOCTEXT_NAMESPACE "DialogueManagerEditor"

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

TSharedRef<IDetailCustomization> FDialogueManagerCustomization::MakeInstance()
{
	
	return MakeShareable(new FDialogueManagerCustomization);
}

void FDialogueManagerCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	
	IDetailCategoryBuilder& EditingCategory = DetailBuilder.EditCategory("Dialogue Editing");

	const FText AddDialogueUnitText = LOCTEXT("Add Dialogue Unit", "Add Dialogue Unit");
	const FText AddSelectUnitText = LOCTEXT("Add Select Unit", "Add Select Unit");
	const FText AddSwitchUnitText = LOCTEXT("Add Switch Unit", "Add Switch Unit");

	// Cache set of selected things
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	
	EditingCategory.AddCustomRow(AddDialogueUnitText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(AddDialogueUnitText)
		.OnClicked(this, &FDialogueManagerCustomization::AddDialogueUnit)
		.Content()
		[
			SNew(STextBlock)
			.Text(AddDialogueUnitText)
		]
		];

	//------------------------------

	EditingCategory.AddCustomRow(AddSelectUnitText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(AddSelectUnitText)
		.OnClicked(this, &FDialogueManagerCustomization::AddSelectUnit)
		.Content()
		[
			SNew(STextBlock)
			.Text(AddSelectUnitText)
		]
		];



	EditingCategory.AddCustomRow(AddSwitchUnitText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(AddSwitchUnitText)
		.OnClicked(this, &FDialogueManagerCustomization::AddSwitchUnit)
		.Content()
		[
			SNew(STextBlock)
			.Text(AddSwitchUnitText)
		]
		];


	
	//------------------------------

}

UDialogueManager* FDialogueManagerCustomization::GetFirstSelectedAsset() const
{
	// Find first selected valid ProcMeshComp
	UDialogueManager* Asset = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		UDialogueManager* TestAsset = Cast<UDialogueManager>(Object.Get());
		// See if this one is good
		if (TestAsset != nullptr && !TestAsset->IsTemplate())
		{
			Asset = TestAsset;
			break;
		}
	}

	return Asset;
}


FReply FDialogueManagerCustomization::AddDialogueUnit() {

	UDialogueManager* Asset = GetFirstSelectedAsset();
	if (Asset) {
		Asset->AddDialogueUnit();
	}

	return FReply::Handled();
}


FReply FDialogueManagerCustomization::AddSelectUnit() {
	UDialogueManager* Asset = GetFirstSelectedAsset();
	if (Asset) {
		Asset->AddSelectDialogueUnit();
	}

	return FReply::Handled();
}


FReply FDialogueManagerCustomization::AddSwitchUnit() {
	UDialogueManager* Asset = GetFirstSelectedAsset();
	if (Asset) {
		Asset->AddSwitchDialogueUnit();
	}

	return FReply::Handled();
}




#undef LOCTEXT_NAMESPACE
