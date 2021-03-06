// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInterface.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


//class AWeapon;
class AEquipment;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UEquipmentComponent : public UActorComponent, public IItemInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	//UPROPERTY(BlueprintAssignable, Category = Equipment)
	FOnEquipmentUpdated OnEquipmentUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TArray<AEquipment*> EquipmentItems;

	//Pickup일때 해당 bool변수가 체크 되어있으면
	//해당 장비의 Inventory부터 채워나가기 위함.
	bool bHasBackpack;
	bool bHasVest;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	bool AddEquipment(AEquipment* Equip);
	bool RemoveEquipment(AEquipment* Equip);

	//Weapon파라미터와 같은 타입의 장비가 이미 있는지 확인
	bool IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);
	
	//Weapon파라미터와 같은 타입의 장비를 리턴
	AEquipment* GetEquippedWeaponSameType(EEquipmentType EquipType, AEquipment* Equip = nullptr, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);

	bool SwapEquipment(AEquipment* Before, AEquipment* Next);
};
