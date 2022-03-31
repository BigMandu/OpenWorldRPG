// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "NewItemObject.generated.h"

/**
 * 
 */
class AItem;
class UTexture2D;
class UTextBlock;
class UNewInventoryGrid;

//UENUM(BlueprintType)
//enum class EEquipType : uint8
//{
//	EET_Rifle		UMETA(DisplayName = "Rifle"),
//	EET_Pistol		UMETA(DisplayName = "Pistol"),
//	EET_Helmet		UMETA(DisplayName = "Helmet"),
//	EET_Plate		UMETA(Displayname = "Plate Carrier"),
//	EET_Vest		UMETA(DisplayName = "Vest"),
//	EET_Backpack	UMETA(DisplayName = "Backpack"),
//
//	EET_MAX			UMETA(DisplayName = "DefaltMAX")
//};

UCLASS()
class OPENWORLDRPG_API UNewItemObject : public UObject, public IItemInterface
{
	GENERATED_BODY()

public:
	UNewItemObject();
	AItem* item;
	bool bCanRotated;
	bool bRotated;

	FIntPoint itemsize;

	FText itemName;
	FText itemDesc;

	UMaterialInterface* icon;
	UMaterialInterface* iconRotated;

	UNewInventoryGrid* MotherContainer;

	int32 TopLeftIndex;

	UFUNCTION(BlueprintCallable)
	FIntPoint GetItemSize();

	UClass* GetItemClass();
	void ItemRotate();

	UMaterialInterface* GetItemIcon();

	UNewInventoryGrid* GetMotherContainer();

	EEquipmentType EquipmentType;
	EInteractType InteractType;
	bool bCanEquip;
};
