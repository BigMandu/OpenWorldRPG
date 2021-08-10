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


//단발, 점사, 연사를 구분
UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8
{
	EWFM_Safe		UMETA(DisplayName = "Safe"),
	EWFM_SemiAuto	UMETA(DisplayName = "SemiAuto"),
	EWFM_Burst		UMETA(DisplayName = "Burst"),
	EWFM_FullAuto	UMETA(DisplayName = "FullAuto"),

	EWFM_MAX		UMETA(DisplayName = "DefaultMAX")
};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

	//Magazine의 최대 탄알수, 연발 속도
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 AmmoPerMag;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float RateofFire;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;

	/* FWeaponStat Defaults */
	FWeaponStat()
	{
		AmmoPerMag = 30;
		RateofFire = 0.2;
		WeaponRange = 500.f;
	}

};

UCLASS()//Abstract, Blueprintable)
class OPENWORLDRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* SKMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USoundCue* EquippedSound;

	UEquipmentComponent* OwningEquipment;
	AMainCharacter* OwningPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FWeaponStat WeaponStat;

	bool bIsFiring;

	int32 FireCount;

	void SetOwningPlayer(AMainCharacter* Main);

	void Equip(AActor* Char);
	
	void GunAttachToMesh(AMainCharacter* Main);

	bool CheckSendToInventory(AMainCharacter* Main);

	virtual void Drop() override;
	
	void SetWeaponFiringMode();
	
	/* from Player input */
	void StartFire();
	void StopFire();

	void Firing();
	void ReFiring();

	virtual void BulletOut();// PURE_VIRTUAL(AWeapon::BulletOut, );
	
	virtual FTransform GetCamLocRot();

};
