// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
class AMainCharacter;
class AWeapon;
class ALootBox;
class UWidget;
class UInventoryComponent;
class UEquipmentComponent;

UCLASS()
class OPENWORLDRPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	AMainCharacter* Main;
	TWeakObjectPtr<UInventoryComponent>InventoryComp;
	TWeakObjectPtr<UEquipmentComponent>EquipmentComp;

	ALootBox* LootBox;
	//UInventoryComponent* InventoryComp;
	//UEquipmentComponent* EquipmentComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	UUserWidget* InventoryItemsWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	UUserWidget* ChestWidget;

protected:
	UWidget* Primary;
	UWidget* Sub;
	UWidget* Pistol;
	UWidget* Helmet;
	UWidget* Vest;

	
public:
	virtual bool Initialize() override;

	UFUNCTION()
	void InitInventory();

	UFUNCTION()
	void InitEquipment();

	bool CheckSlotIsValid();

	bool CheckEquipSlotIsSet(UWidget* EquipSlot);

	void SetEquipmentSlot(UWidget* EquipSlot, AWeapon* Weap);
	
	void SetLootBox();
};
