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

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginHighReady);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndHighReady);

class UMatineeCameraShake;
class UCameraShake;
class USphereComponent;
class UCapsuleComponent;
class USoundCue;
class UParticleSystem;
class UParticleSystemComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class AMainCharacter;
class UCurveFloat;
class UWeaponPDA;


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

//UENUM()
//enum class ERifleAssign : uint8
//{
//	ERA_Primary		UMETA(Displayname = "Primary"),
//	ERA_Sub			UMETA(Displayname = "Sub"),
//	ERA_MAX			UMETA(Displayname = "DefaultsMAX")
//};


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

/*
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

	//Weapon�� ��Ÿ�
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;

	// ź�� ������� ����.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float HipBulletSpread;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float AimBulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 AmmoPerMag;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	bool bHasBurstMode;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 BurstRound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float FireRatePerMin;
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float FireRatePerSec;
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float SecondPerBullet;

	// Recoil 
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_X;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_Y;

	// FWeaponStat Defaults
	FWeaponStat()
	{
		WeaponRange = 5000.f;
		HipBulletSpread = 1.5f;
		AimBulletSpread = 1.f;

		AmmoPerMag = 30;

		bHasBurstMode = true;
		BurstRound = 3;	
		

		//m4a1�� �д� 700~950��.�д� 950���� ��� �ʴ� 15.8���� ����. 0.06�ʴ� �ѹ߾�.
		FireRatePerMin = 950;

		//FireRatePerSec�̳� SecondPerBullet��
		//FireRatePerMin�� Weapon���� ���� �� ������.
		//Equip�Լ����� ����Ͽ� �����Ѵ�.
		 
		
	}
};
*/

UCLASS(Abstract)
class OPENWORLDRPG_API AWeapon : public AEquipment
{
	GENERATED_BODY()
protected:
	
public:
	AWeapon();
	
	//UEquipmentComponent* OwningEquipment;
	//AActor* OwningPlayer;

	//FOnBeginHighReady OnBeginHighReady;
	//FOnEndHighReady OnEndHighReady;


	//StepEquip�� �����Ѵ�.
	//������ ���� ���� ����ϱ� ���� Cast�صд�.
	UWeaponPDA* WeaponDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	FTransform OriginalWeaponTransform;

	bool bIsFiring;
	bool bIsAiming; //Main���� ���� �ܼ��� �־��ֱ⸸ �Ѵ�.

	//Clipping�� �ϰ������� Aim�� ����, BulletOut�� Trace�� �����ϱ� ���� boolean
	bool bIsHighReady;

	FName MuzzleFlashSocketName;

	//PDA�� ������.
	/*UPROPERTY(EditAnywhere, Category = "WeaponStat")
	FWeaponStat WeaponStat;*/

	/* FPS Aim��� �϶� ��ġ�� ���� -> PDA�� ������.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform CharFPMeshTransform;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform WeapSKMeshTransform;


	/* Enums */

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ERifleAssign RifleAssign;*/

	//Primary, Sub�� �����Ѵ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ERifleSlot RifleAssign;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponState CurrentWeaponState;

	/* FX  -> �Ʒ� Sound,Effect, Shake, Animation���� PDA�� �ű�. */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	//USoundCue* EquippedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* FireMuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> CamShake;

	/* Animation */
	/*UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim FireAnimaton;*/

	//////////////////////////////////
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

	
	float WeaponClippingLength;

	FTimerHandle FiringTimer;

	//WeaponClipping�� ���� Hand�� Muzzle�� Relative��.
	FTransform MuzzleRelative;
	

	FVector WorldAimPosition;
	//FVector WeaponMuzzleLocation;


	//�Ʒ� 3���� FTransform�� PDA�� �ű�.
	//FPMesh�� WeaponGrip ���Ͽ� ���� Weapon�� Transform��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform FPMeshAttachTransform;

	//TPMesh�� Weapon�� Attach�� ������ Transform��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform PrimaryWeaponAttachTransform;

	//TPMesh�� Weapon�� Attach�� ������ Transform��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform SubWeaponAttachTransform;



	/* Aim Initialize���� ��� */
	FRotator StartFiringRotation;
	FRotator EndFiringRotation;

	float RecoilTime; //Curve float���� ���.

	float Time;

	FVector PreviousSpread;
	FVector NextSpread;

	

private:
	void UpdateAim();
	void WeaponClipping();

protected:
	virtual bool StepEquip(AActor* Char, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX) override;
public:

	//void SetOwningPlayer(AActor * Actor);
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	
	
	void GunAttachToMesh(AActor* Actor);

	//void FPS_AimAttachToMesh(AActor* Actor);

	FTransform GetSightSocketTransform();

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

	virtual void Old_BulletOut() PURE_VIRTUAL(AWeapon::BulletOut);
	virtual void New_BulletOut() PURE_VIRTUAL(AWeapon::New_BulletOut);

	/*
	UFUNCTION()
	void OnCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCollisionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	*/


	//FVector GetAimLocation_TEST();
	void AimInitialize();
	FTransform GetAimPosition();
	FVector GetTraceStartLocation(FVector Dir = FVector::ZeroVector);
	FVector GetTraceEndLocation(FVector StartVector = FVector::ZeroVector, FVector Dir = FVector::ZeroVector);
	FHitResult BulletTrace(FVector StartTrace, FVector EndTrace);
	
	/* Setting New Weapon State, And Call SetWeaponState func */
	void TempNewWeaponState();
	
	/* Compare Preview State, And Setting State , And Call Firing func */
	void SetWeaponState(EWeaponState NewState);

	void SettingRifleAssign(ABaseCharacter* BChar, ERifleSlot RifleSlot);

	void WeaponFX();
	//void PlayWeaponAnimAndCamShake(FWeaponAnim& Anim);

	

	virtual void Remove() override;
};
