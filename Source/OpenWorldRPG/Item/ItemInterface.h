// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_Rifle		UMETA(DisplayName = "Rifle"),
	EET_Pistol		UMETA(DisplayName = "Pistol"),
	EET_Helmet		UMETA(DisplayName = "Helmet"),
	EET_Plate		UMETA(Displayname = "Plate Carrier"),
	EET_Vest		UMETA(DisplayName = "Vest"),
	EET_Backpack	UMETA(DisplayName = "Backpack"),

	EET_MAX			UMETA(DisplayName = "DefaltMAX")
};


UENUM(BlueprintType)
enum class EInteractType : uint8
{
	EIT_Gizmo		UMETA(DisplayName = "EIT_Gizmo"),
	EIT_Item		UMETA(DisplayName = "EIT_Item"),
	EIT_Equipment	UMETA(DisplayName = "EIT_Equipment"),
	EIT_LootBox		UMETA(DisplayName = "EIT_LootBox"),

	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLDRPG_API IItemInterface
{
	GENERATED_BODY()
public:
	bool bCanDrop = false;
	//virtual bool CanDrop() = 0;

	//virtual bool SpawnEquip(class UNewItemObject* ItemObj);

	//FORCEINLINE bool SpawnEquip(class UNewItemObject* ItemObj)
	//{
	//	if (ItemObj->bIsDestoryed)
	//	{
	//		class AEquipment* T_Equipment = Cast<AEquipment>(UWorld::GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
	//		if (T_Equipment)
	//		{
	//			ItemObj->bIsDestoryed = false;
	//			//T_Equipment = Equipment;
	//			T_Equipment->EquipInventoryComp = Equipment->EquipInventoryComp;
	//			T_Equipment->ReInitialize(ItemObj);

	//			Equipment = T_Equipment;

	//			ItemObj->item = T_Equipment;



	//		}
	//		//Equipment->SetItemState(EItemState::EIS_Pickup);
	//	}
	//}
};
