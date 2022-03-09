// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LootBoxWidget.generated.h"

/**
 * 
 */

class AMainCharacter;
class ALootBox;
class UInventoryComponent;
class UNewInventoryGrid;
class UUserWidget;



UCLASS()
class OPENWORLDRPG_API ULootBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryGrid* LootBoxGridwidget;

	void InitLootBoxWidget(AActor* actor);

	ALootBox* LootBox;
//	class ALootBox* InteractBox;
//
//	AMainCharacter* Main;
//	
//	TWeakObjectPtr<UInventoryComponent>BoxInventory;
//
////	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
//	UUserWidget* InventoryItemsWidget;
//
	
	
//
//	UFUNCTION()
//	void InitLootBox();
};
