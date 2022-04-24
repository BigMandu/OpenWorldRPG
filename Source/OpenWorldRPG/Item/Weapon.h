// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Equipment.h"
//#include "Components/SkeletalMeshComponent.h"
//#include "EquipmentComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */
#define COLLISION_WEAPON_INST	ECC_GameTraceChannel1

class UMatineeCameraShake;
class UCameraShake;
class USoundCue;
class UParticleSystem;
class UParticleSystemComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class AMainCharacter;
class UCurveFloat;


//UENUM(BlueprintType)
//enum class EWeaponType : uint8
//{
//	EWT_Rifle	UMETA(DisplayName = "Rifle"),
//	EWT_Pistol	UMETA(DisplayName = "Pistol"),
//	EWT_Helmet	UMETA(DisplayName = "Helmet"),
//	EWT_Vest	UMETA(DisplayName = "Vest"),
//
//	EWT_MAX		UMETA(DisplayName = "DefaltMAX")
//};

UENUM()
enum class ERifleAssign : uint8
{
	ERA_Primary		UMETA(Displayname = "Primary"),
	ERA_Sub			UMETA(Displayname = "Sub"),
	ERA_MAX			UMETA(Displayname = "DefaultsMAX")
};


//단발, 점사, 연사를 구분
UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8
{
	EWFM_Safe		UMETA(DisplayName = "Safe"), //안전
	EWFM_SemiAuto	UMETA(DisplayName = "SemiAuto"), //단발
	EWFM_Burst		UMETA(DisplayName = "Burst"), //점사
	EWFM_FullAuto	UMETA(DisplayName = "FullAuto"), //연발

	EWFM_MAX		UMETA(DisplayName = "DefaultMAX")
};

//Weapon의 상태
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Idle		UMETA(DisplayName = "Idle"),
	EWS_Reloading	UMETA(DisplayName = "Reloading"),
	EWS_Firing		UMETA(DisPlayName = "Firing"),
	
	EWS_MAX			UMETA(DisplayName = "DefaultsMAX")
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FPSAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TPSAnim;
};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

	//Magazine의 최대 탄알수, 연발 속도
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 AmmoPerMag;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float FireRatePerMin;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	bool bHasBurstMode;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 BurstRound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float FireRatePerSec;
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float SecondPerBullet;

	/* Recoil */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_X;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_Y;

	/* FWeaponStat Defaults */
	FWeaponStat()
	{
		bHasBurstMode = true;
		BurstRound = 3;

		WeaponRange = 5000.f;
		AmmoPerMag = 30;

		//m4a1은 분당 700~950발.분당 950으로 잡고 초당 15.8발을 쏘면됨. 0.06초당 한발씩.
		FireRatePerMin = 950;
		
		
	}
};

UCLASS(Abstract)
class OPENWORLDRPG_API AWeapon : public AEquipment
{
	GENERATED_BODY()
	
public:
	AWeapon();

	//UEquipmentComponent* OwningEquipment;
	//AActor* OwningPlayer;


	bool bIsFiring;
	bool bIsAiming; //Main에서 값을 단순히 넣어주기만 한다.

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//USkeletalMeshComponent* SKMesh;

	UPROPERTY(EditAnywhere, Category = "WeaponStat")
	FWeaponStat WeaponStat;

	FName MuzzleFlashSocketName;

	/* FPS Aim모드 일때 위치값 저장*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform CharFPMeshTransform;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform WeapSKMeshTransform;

	

	/* Enums */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ERifleAssign RifleAssign;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponState CurrentWeaponState;

	/* FX */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	//USoundCue* EquippedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* FireMuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> CamShake;

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim FireAnimaton;

	float AlphaTime;
	float RecoilAlphaTime;
	FTimerHandle AimInitHandle;
	FTimerHandle RecoilHandle;
	
protected:

	//for gun spread
	int32 FireCount;

	bool bDetectLookInput;

	bool bLMBDown;	

	float LastFireTime;

	FTimerHandle FiringTimer;

	/* Aim Initialize에서 사용 */
	FRotator StartFiringRotation;
	FRotator EndFiringRotation;

	float RecoilTime; //Curve float에서 사용.

	float Time;

	FVector PreviousSpread;
	FVector NextSpread;
	
	
	
public:

	//void SetOwningPlayer(AActor * Actor);

	virtual void Equip(AActor* Char) override;
	
	void GunAttachToMesh(AActor* Actor);

	void FPS_AimAttachToMesh(AActor* Actor);

	//bool CheckSendToInventory(AMainCharacter* Main);

	//virtual void Drop() override;
	
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

	bool CanEndFire();

	virtual void BulletOut() PURE_VIRTUAL(AWeapon::BulletOut);
	virtual void New_BulletOut() PURE_VIRTUAL(AWeapon::New_BulletOut);

	FVector GetAimRotation();
	FVector GetTraceStartLocation(FVector& Dir);
	FHitResult BulletTrace(FVector& StartTrace, FVector& EndTrace);
	
	/* Setting New Weapon State, And Call SetWeaponState func */
	void TempNewWeaponState();
	
	/* Compare Preview State, And Setting State , And Call Firing func */
	void SetWeaponState(EWeaponState NewState);

	void WeaponFX();
	void PlayWeaponAnimAndCamShake(FWeaponAnim& Anim);

	void AimInitialize();

	virtual void Remove() override;
};
