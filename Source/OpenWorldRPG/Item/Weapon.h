// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "EquipmentComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */

class USoundCue;
class USkeletalMeshComponent;
class AMainCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle	UMETA(DisplayName = "Rifle"),
	EWT_Pistol	UMETA(DisplayName = "Pistol"),
	EWT_Helmet	UMETA(DisplayName = "Helmet"),
	EWT_Vest	UMETA(DisplayName = "Vest"),

	EWT_MAX		UMETA(DisplayName = "DefaltMAX")
};


UCLASS()
class OPENWORLDRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
private:
	void GunAttachToMesh(AMainCharacter* Main);

public:
	AWeapon();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	USkeletalMeshComponent* SKMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Enums")
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon")
	USoundCue* EquippedSound;


	UEquipmentComponent* OwningEquipment;

	void Equip(AActor* Char);

	bool CheckSendToInventory(AMainCharacter* Main);
	
};
