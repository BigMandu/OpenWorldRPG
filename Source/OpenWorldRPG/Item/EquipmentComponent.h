// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


//class AWeapon;
class AEquipment;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	//UPROPERTY(BlueprintAssignable, Category = Equipment)
	FOnEquipmentUpdated OnEquipmentUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TArray<AEquipment*> EquipmentItems;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	bool AddEquipment(AEquipment* Equip);
	bool RemoveEquipment(AEquipment* Equip);

	//Weapon�Ķ���Ϳ� ���� Ÿ���� ��� �̹� �ִ��� Ȯ��
	bool IsWeaponExist(AEquipment* Equip);
	
	//Weapon�Ķ���Ϳ� ���� Ÿ���� ��� ����
	AEquipment* GetBeforeWeapon(AEquipment* Equip);

	bool SwapEquipment(AEquipment* Before, AEquipment* Next);
};
