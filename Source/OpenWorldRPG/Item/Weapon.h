// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Equipment.h"
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
class UItemStorageObject;


//�ܹ�, ����, ���縦 ����
UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8
{
	EWFM_Safe		UMETA(DisplayName = "Safe"),	//����
	EWFM_SemiAuto	UMETA(DisplayName = "SemiAuto"),//�ܹ�
	EWFM_Burst		UMETA(DisplayName = "Burst"),	//����
	EWFM_FullAuto	UMETA(DisplayName = "FullAuto"),//����

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


UCLASS(Abstract)
class OPENWORLDRPG_API AWeapon : public AEquipment
{
	GENERATED_BODY()
protected:
	FName SightSocketName = FName("AimPos");
	FName MuzzleFlashSocketName = FName("muzzleflash");


	//for gun spread
	int32 FireCount;

	bool bDetectLookInput;

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


public:
	AWeapon();
	
	//UEquipmentComponent* OwningEquipment;
	//AActor* OwningPlayer;

	//FOnBeginHighReady OnBeginHighReady;
	//FOnEndHighReady OnEndHighReady;


	//StepEquip�� �����Ѵ�.
	//������ ���� ���� ����ϱ� ���� Cast�صд�.
	UWeaponPDA* WeaponDataAsset;

	UPROPERTY()
	class UWeaponPartsManagerObject* WeaponPartsManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	FTransform OriginalWeaponTransform;

	

	bool bLMBDown;

	bool bIsFiring;
	bool bIsAiming; //Main���� ���� �ܼ��� �־��ֱ⸸ �Ѵ�.

	//Clipping�� �ϰ������� Aim�� ����, BulletOut�� Trace�� �����ϱ� ���� boolean
	bool bIsHighReady;

	float AlphaTime;
	float RecoilAlphaTime;



	/* FPS Aim��� �϶� ��ġ�� ���� -> PDA�� ������.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform CharFPMeshTransform;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform WeapSKMeshTransform;


	//Primary, Sub�� �����Ѵ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ERifleSlot RifleAssign;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponState CurrentWeaponState;


	FTimerHandle AimInitHandle;
	FTimerHandle RecoilHandle;


	/********* Ammo *********/
	//�Ҹ��� ź ����
	int32 ConsumeAmmoCnt = 0;

	//źâ�� ���� ź���� ����
	int32 AmmoLeftInMag = 0;

	//Inv�� ���� ���� Ÿ�� ź���� �� ���� forWeaponStatusWidget.
	int32 CntAmmoSameType = 0;

	//////////////////////////////////


private:
	
	void WeaponClipping();
	bool CheckAmmo();

	void UseAmmo();



	/* Setting New Weapon State, And Call SetWeaponState func */
	void TempNewWeaponState();

	/* Compare Preview State, And Setting State , And Call Firing func */
	void SetWeaponState(EWeaponState NewState);
	void ControlFiring();
	void Firing();
	void ReFiring();
	void EndFiring();

	bool CheckRefire();
	bool CanFire();
	bool CanEndFire();


	void CapsuleAdjust();
	

protected:	
	void UpdateAim();
	virtual void BeginPlay() override;
	virtual bool StepEquip(AActor* Char, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX) override;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
		
	void GunAttachToMesh(AActor* Actor);
	void GunAttachToSubSocket(AActor* Actor);

	FTransform GetSightSocketTransform();
	void CheckWeaponPartsManager();
	UFUNCTION()
	void UpdateWeaponParts();

	//for WeaponStatusWidget
	void GetTotalAmmo();
	
	int32 GetCurrentAmmoInMag();

	void ChangeSafetyLever();
	void ChangeSafetyLeverForAI(EWeaponFiringMode FiringMode);
	
	/* Attack */
	void StartFire();
	void StopFire();


	void Reload();
	void ReloadEnd();

	virtual void Old_BulletOut() PURE_VIRTUAL(AWeapon::BulletOut);
	virtual void New_BulletOut() PURE_VIRTUAL(AWeapon::New_BulletOut);
	virtual void AIBulletOut() PURE_VIRTUAL(AWeapon::AIBulletOut);

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




	void WeaponFX();


	virtual void Remove() override;
};
