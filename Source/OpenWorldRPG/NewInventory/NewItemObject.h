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

	UPROPERTY()
	FItemSetting ItemInfo;
	UPROPERTY()
	FIntPoint ItemRotateSize;
	UPROPERTY()
	EEquipmentType EquipmentType;
	UPROPERTY()
	EInteractType InteractType;


	//EquipComp, InvenComp에서 사용한다. (편하게 하기 위함...)
	AItem* item;
	AEquipment* Equipment;

	UPROPERTY()
	int32 TopLeftIndex;
	UItemStorageObject* MotherStorage;
	UEquipmentComponent* MotherEquipComp;
	class UWeaponPDA* OwnerWeaponPDA;
	
	UEquipmentSlot* SettedSlot;

	UPROPERTY()
	ERifleSlot RifleAssign;

	UPROPERTY()
	class UWeaponPartsManagerObject* WeaponPartsManager;

	//for Weapon
	UPROPERTY()
	int32 AmmoLeftInMag = 0;

	/* boolean value */

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

	/** for CoreUsableItem.
	* this class is Undeleted item. e.g Compass, Map.
	* when true, if using this item again then detach from actor.
	* when false, if using this item then attach to actor.
	*/
	bool bIsUsing = false;








	////////////////////////////////////////////////////////////
	/************************** Func **************************/
	////////////////////////////////////////////////////////////


	//virtual void Serialize(FArchive& Ar) override;

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
	
	FORCEINLINE void SetAmmoLeftInMag(int32 AmmoCnt) { AmmoLeftInMag = AmmoCnt; }
	FORCEINLINE int32 GetAmmoLeftInMag() { return AmmoLeftInMag; }

	
};
