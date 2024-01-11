// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class AWeapon;
class AEquipment;
class UNewItemObject;
class UItemStorageObject;

//for EquipWidget Refresh
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUpdated);

//for QuickSlot weaponSet
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSetSlot, UNewItemObject*, EquipWeaponObj);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponRemoveSlot, ERifleSlot, EquipWeaponSlot);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENWORLDRPG_API UEquipmentComponent : public UActorComponent//, public IItemInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	//UPROPERTY(BlueprintAssignable, Category = Equipment)
	FOnEquipmentUpdated OnEquipmentUpdated;
	FOnWeaponSetSlot OnWeaponSetSlot;
	FOnWeaponRemoveSlot OnWeaponRemoveSlot;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TArray<AEquipment*> EquipmentItems;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TArray<UNewItemObject*> EquipmentItems;

	//Pickup일때 해당 bool변수가 체크 되어있으면
	//해당 장비의 Inventory부터 채워나가기 위함.
	//TEnumRange로 대체해서 더이상 쓰이지 않음.
	/*bool bHasBackpack;
	bool bHasVest;*/

	UItemStorageObject* BackpackObj = nullptr;
	UItemStorageObject* VestObj = nullptr;

	
private:
	//WeaponPartsManager를 생성 , data를 넘겨준다.
	//void SetWeaponPartsManager(AEquipment* WantToEquip, UNewItemObject* WeaponObj);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	//bool AddEquipment(AEquipment* Equip);
	//bool RemoveEquipment(AEquipment* Equip);
	
	
	//Weapon파라미터와 같은 타입의 장비를 리턴
	//AEquipment* GetEquippedWeaponSameType(EEquipmentType EquipType, AEquipment* Equip = nullptr, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);

	//bool SwapEquipment(AEquipment* Before, AEquipment* Next);

	/* New version */
	UItemStorageObject* GetEquipStorage(EEquipmentType Type);
	bool AddEquipment(struct FItemSetting ItemSetting, AEquipment* WantToEquip);
	bool RemoveEquipment(UNewItemObject* EquipObj);

	//Equip의 파라미터와 같은 타입의 장비가 이미 있는지 확인
	bool IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);

	UNewItemObject* GetEquippedWeaponSameType(EEquipmentType EquipType, UNewItemObject* Object, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);

	void UpdateEquipment(TArray<UNewItemObject*>& SavedEquipment);

	void RemoveAllEquip();

};
