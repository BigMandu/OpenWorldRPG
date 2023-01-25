// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
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
class UEquipmentComponent;
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
	
	
	bool bTempRotate = false;
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
	class UWeaponPDA* OwnerWeaponPDA;

	TWeakObjectPtr<class UWeaponPartsManagerObject> WeaponPartsManager;
	//class UWeaponPartsWidget* MotherWeaponPartsWidget;
	UEquipmentSlot* SettedSlot;

	ERifleSlot RifleAssign;

	//GridWidget에서 On-Drop시에 Rotate값을 확정한다. (TempRotate값을 Rotate값으로 넣어주기만 한다.)
	void SetItemRotate();

	//DragDefaultVisual만 회전시킨다.
	void ItemRotate();

	UMaterialInterface* GetItemIcon();
	UMaterialInterface* GetTempItemIcon();
	FIntPoint GetItemSize();
	FIntPoint GetTempItemSize();


	UClass* GetItemClass();
	

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
