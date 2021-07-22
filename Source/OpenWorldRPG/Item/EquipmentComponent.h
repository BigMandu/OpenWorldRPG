// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class AWeapon;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TArray<AActor*> EquipmentItems;

	UPROPERTY(BlueprintAssignable, Category = Equipment)
	FOnEquipmentUpdated OnEquipmentUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	bool AddEquipment(AWeapon* Weapon);
	bool RemoveEquipment(AWeapon* Weapon);

	//Weapon�Ķ���Ϳ� ���� Ÿ���� ��� �̹� �ִ��� Ȯ��
	bool IsWeaponExist(AWeapon* Weapon);
	
	//Weapon�Ķ���Ϳ� ���� Ÿ���� ��� ����
	AWeapon* GetBeforeWeapon(AWeapon* Weapon);
};
