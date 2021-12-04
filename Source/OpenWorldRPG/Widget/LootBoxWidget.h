// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LootBoxWidget.generated.h"

/**
 * 
 */

class AMainCharacter;
class UInventoryComponent;
class UUserWidget;


UCLASS()
class OPENWORLDRPG_API ULootBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	class ALootBox* InteractBox;

	AMainCharacter* Main;
	
	TWeakObjectPtr<UInventoryComponent>BoxInventory;

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	UUserWidget* InventoryItemsWidget;

	virtual bool Initialize() override;

	UFUNCTION()
	void InitLootBox();
};
