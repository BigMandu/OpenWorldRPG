// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "NewItemObject.generated.h"

/**
 * 
 */
class AItem;
class AEquipment;

class UTexture2D;
class UTextBlock;
class UNewInventoryGrid;
class UNewInventoryComponent;
class UItemStorageObject;
class UEquipmentSlot;
class UCustomPDA;

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


	FItemSetting ItemInfo;

	FIntPoint ItemRotateSize;
	EEquipmentType EquipmentType;
	EInteractType InteractType;

	AItem* item;
	UMaterialInterface* icon;
	UMaterialInterface* iconRotated;

	bool bCanRotated;
	bool bRotated = false;
	bool bCanEquip;
	bool bIsDestoryed;

	bool bIsRegQuickSlot;

	bool bIsPendingDelete = false;

	//when you Dragging NewItemWidget this Var turn True.
	//And Dropping NewItemWidget, This var turn False.
	//Use at NewInventoryGrid, EquipmentSlot, NewInventory
	bool bIsDragging;

	int32 TopLeftIndex;

	FText itemName;
	FText itemDesc;

	AEquipment* Equipment;

	UItemStorageObject* MotherStorage;
	UEquipmentComponent* MotherEquipComp;
	UEquipmentSlot* SettedSlot;

	ERifleSlot RifleAssign;


	FIntPoint GetItemSize();

	UClass* GetItemClass();
	void ItemRotate();

	UMaterialInterface* GetItemIcon();

	FORCEINLINE void SetLinkSlot(UEquipmentSlot* WantToLinkSlot) { SettedSlot = WantToLinkSlot; }
	void RemoveLinkSlot();

	const UItemStorageObject* GetMotherStorage();
	void SetMotherStorage(UItemStorageObject* Var_MotherStorage);

	const UEquipmentComponent* GetMotherEquipComp();
	void SetMotherEquipComp(UEquipmentComponent* Var_MotherEquipComp);

	void AddCount(int32 Cnt);
	void RemoveCount(int32 Cnt);

	void UseItem(UWorld* World);
	

	
};
