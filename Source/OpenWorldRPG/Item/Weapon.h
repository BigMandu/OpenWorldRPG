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
#define COLLISION_WEAPON_INST	ECC_GameTraceChannel1

class USoundCue;
class UParticleSystem;
class UParticleSystemComponent;
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


//�ܹ�, ����, ���縦 ����
UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8
{
	EWFM_Safe		UMETA(DisplayName = "Safe"), //����
	EWFM_SemiAuto	UMETA(DisplayName = "SemiAuto"), //�ܹ�
	EWFM_Burst		UMETA(DisplayName = "Burst"), //����
	EWFM_FullAuto	UMETA(DisplayName = "FullAuto"), //����

	EWFM_MAX		UMETA(DisplayName = "DefaultMAX")
};

//Weapon�� ����
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Idle		UMETA(DisplayName = "Idle"),
	EWS_Reloading	UMETA(DisplayName = "Reloading"),
	EWS_Firing		UMETA(DisPlayName = "Firing"),
	
	EWS_MAX			UMETA(DisplayName = "DefaultsMAX")
};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

	//Magazine�� �ִ� ź�˼�, ���� �ӵ�
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 AmmoPerMag;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float RateofFire;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	bool bHasBurstMode;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 BurstRound;
	/* FWeaponStat Defaults */
	FWeaponStat()
	{
		bHasBurstMode = true;
		BurstRound = 3;

		WeaponRange = 5000.f;
		AmmoPerMag = 30;
		//m4a1�� �д� 700~950��.�д� 950���� ��� �д� 15.8���� ����. 0.06�ʴ� �ѹ߾�.
		RateofFire = 0.06; 
		
	}

};

UCLASS(Abstract)
class OPENWORLDRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	UEquipmentComponent* OwningEquipment;
	AActor* OwningPlayer;
	
	bool bIsAiming; //Main���� ���� �ܼ��� �־��ֱ⸸ �Ѵ�.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* SKMesh;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	FWeaponStat WeaponStat;

	/* FPS Aim��� �϶� ��ġ�� ����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimLocation")
	FTransform CharFPMeshTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimLocation")
	FTransform WeapSKMeshTransform;

	/* Enums */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponState CurrentWeaponState;

	/* FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* EquippedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* FireMuzzleEffect;
	
	//UParticleSystemComponent* MuzzleEffectComp;

	FName MuzzleFlashSocketName;
	
	
protected:

	//for gun spread
	int32 FireCount;

	bool bLMBDown;

	bool bIsFiring;

	float LastFireTime;

	FTimerHandle FiringTimer;

	
	
public:

	void SetOwningPlayer(AActor * Actor);

	void Equip(AActor* Char);
	
	void GunAttachToMesh(AMainCharacter* Main);

	void FPS_AimAttachToMesh(AMainCharacter* Main);

	bool CheckSendToInventory(AMainCharacter* Main);

	virtual void Drop() override;
	
	void ChangeSafetyLever();
	
	/* Attack */
	void StartFire();
	void StopFire();

	void ControlFiring();
	void Firing();
	void ReFiring();
	void EndFiring();

	bool CheckRefire();

	bool CanFire();

	virtual void BulletOut() PURE_VIRTUAL(AWeapon::BulletOut);
	
	FVector GetAimRotation();
	FVector GetTraceStartLocation(FVector& Dir);
	FHitResult BulletTrace(FVector& StartTrace, FVector& EndTrace);
	/* Setting New Weapon State, And Call SetWeaponState func */
	void TempNewWeaponState();
	
	/* Compare Preview State, And Setting State , And Call Firing func */
	void SetWeaponState(EWeaponState NewState);

	void WeaponFX();
};
