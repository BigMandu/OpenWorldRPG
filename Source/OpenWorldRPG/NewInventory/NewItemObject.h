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
class AEquipment;
class UTexture2D;
class UTextBlock;
class UNewInventoryGrid;
class UNewInventoryComponent;
class UEquipmentSlot;

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

private:
	
	UNewInventoryGrid* MotherContainer;
	UEquipmentSlot* MotherEquipSlot;

public:
	UNewItemObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UNewInventoryComponent* InvComp;

	FIntPoint itemsize;
	EEquipmentType EquipmentType;
	EInteractType InteractType;

	AItem* item;
	UMaterialInterface* icon;
	UMaterialInterface* iconRotated;

	bool bCanRotated;
	bool bRotated;
	bool bCanEquip;
	bool bIsDestoryed;

	//when you Dragging NewItemWidget this Var turn True.
	//And Dropping NewItemWidget, This var turn False.
	//Use at NewInventoryGrid, EquipmentSlot, NewInventory
	bool bIsDragging;

	int32 TopLeftIndex;

	FText itemName;
	FText itemDesc;


	//UFUNCTION(BlueprintCallable)
	FIntPoint GetItemSize();

	UClass* GetItemClass();
	void ItemRotate();

	UMaterialInterface* GetItemIcon();

	UNewInventoryComponent* GetItemInvComp();
	void SetItemInvComp(UNewInventoryComponent* Var_InvCmp);

	UNewInventoryGrid* GetMotherContainer();
	void SetMotherContainer(UNewInventoryGrid* Var_InvGrid);

	UEquipmentSlot* GetMotherEquipSlot();
	void SetMotherEquipSlot(UEquipmentSlot* Var_EquipSlot);

	


	

	
};
