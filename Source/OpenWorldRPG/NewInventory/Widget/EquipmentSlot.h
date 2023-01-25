// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "EquipmentSlot.generated.h"


/* 
* 단순히, Image와 Border색상만을 변경한다.

*/
class UImage;
class UBorder;
class UNewItemObject;

class ABaseCharacter;

//for WeaponPartsWidget
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeaponParts, UNewItemObject*, EquipPartsObj);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnEquipWeaponParts, UNewItemObject*, UnEquipPartsObj);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshWidget);


UCLASS()
class OPENWORLDRPG_API UEquipmentSlot : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()

public:

	//for WeaponPartsWidget	
	FOnEquipWeaponParts OnEquipWeaponParts;
	FOnUnEquipWeaponParts OnUnEquipWeaponParts;
	FOnRefreshWidget OnRefreshWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | EquipSlot")
	EEquipmentType SlotType;
	
	//RifleSlot기능일때
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | RifleSlot")
	bool bIsforRifleSlot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | RifleSlot", meta = (EditCondition = "bIsforRifleSlot"))
	ERifleSlot RifleSlotType;

	//Weapon Parts Slot 기능일 때 ( for WeaponPartsWidget)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | WeaponParts")
	bool bIsforWeaponParts = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | WeaponParts", meta = (EditCondition = "bIsforWeaponParts"))
	EWeaponPartsType WeaponPartsType;

	TWeakObjectPtr<UNewItemObject> OwnerWeaponObj = nullptr;
	

	UPROPERTY(meta = (BindWidget))
	UBorder* BGBorder;

	UNewItemObject* SettedObj;

	ABaseCharacter* LootedChar_Owner;

private:

	bool IsEmpty();
	AEquipment* SpawnEquipment(UNewItemObject* Obj);


protected:

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

	void PaintBGBorder(UNewItemObject* ItemObj = nullptr);
	bool IsSupportedEquip(UNewItemObject* ItemObj);
	

	/* Equipment를 Spawn하여 장착 시도한다. */
	bool TrySlotEquip(UNewItemObject* Var_ItemObj);

	/* Parts를 WeaponPDA에 등록 시킨다. */
	bool TrySlotParts(UNewItemObject* PartsObj);
	
	/* 등록된 Parts를 지운다. */
	void RemoveParts(UNewItemObject* PartsObj);

};
