// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftSystemWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UCraftSystemWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<UDataTable> CraftRecipeTable;

	//TWeakObjectPtr<UBasePDA> ClickedCraftItem;

	TWeakObjectPtr<UCraftItemWidget> ClickedCraftWidget;

	TWeakObjectPtr<UCraftSystemComponent> PlayerCraftComp;

public:
	UPROPERTY(meta = (bindwidget))
	class UWrapBox* CraftGridPanel;
	//class UUniformGridPanel* CraftGridPanel;
	

	UPROPERTY(meta = (bindwidget))
	class UTextBlock* CraftItemName;

	UPROPERTY(meta = (bindwidget))
	class UTextBlock* CraftItemDescription;

	UPROPERTY(meta = (bindwidget))
	UWrapBox* RecipeBox;

	UPROPERTY(meta = (bindwidget))
	class UButton* CraftButton;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCraftItemWidget> WCraftItemWidget;



protected:
	virtual void NativeConstruct() override;

public:
	
	void SettingCraftWidget();//UDataTable* CraftDT);

	UFUNCTION()
	void SettingRecipe(UCraftItemWidget* NewClickedWidget);
	UFUNCTION()
	void OnDropAtIngridentWidget(UCraftItemWidget* OnWidget, UCustomDDOperation* CusDDOper);

	bool bIsSameClickedItem(UCraftItemWidget* NewClickedWidget);


	void TwoIngredientRecipe(FCraftRecipeTable* Row);
	void ThreeIngredientRecipe(FCraftRecipeTable* Row);
	void FourIngredientRecipe(FCraftRecipeTable* Row);

	//Call after SettingRecipe or OnDropIngWidget or Click CraftButton
	bool CheckCanCraft();

	UFUNCTION()
	void CraftItem();

	//void SaveTwoIngredientStatus(TArray<int32>& SaveIngCountArray);



};
