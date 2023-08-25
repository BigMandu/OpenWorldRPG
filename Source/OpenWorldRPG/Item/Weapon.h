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


//단발, 점사, 연사를 구분
UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8
{
	EWFM_Safe		UMETA(DisplayName = "Safe"),	//안전
	EWFM_SemiAuto	UMETA(DisplayName = "SemiAuto"),//단발
	EWFM_Burst		UMETA(DisplayName = "Burst"),	//점사
	EWFM_FullAuto	UMETA(DisplayName = "FullAuto"),//연발

	EWFM_MAX		UMETA(DisplayName = "DefaultMAX")
};

//Weapon의 상태
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Idle		UMETA(DisplayName = "Idle"),
	EWS_Equipping	UMETA(DisplayName = "Eqiupping"),
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

	//WeaponClipping을 위해 Hand와 Muzzle의 Relative값.
	FTransform MuzzleRelative;

	FVector WorldAimPosition;
	//FVector WeaponMuzzleLocation;

	//아래 3개의 FTransform은 PDA로 옮김.
	//FPMesh의 WeaponGrip 소켓에 붙일 Weapon의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform FPMeshAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform PrimaryWeaponAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform SubWeaponAttachTransform;



	/* Aim Initialize에서 사용 */
	FRotator StartFiringRotation;
	FRotator EndFiringRotation;

	float RecoilTime; //Curve float에서 사용.
	float Time;

	FVector PreviousSpread;
	FVector NextSpread;


public:
	AWeapon();
	
	//UEquipmentComponent* OwningEquipment;
	//AActor* OwningPlayer;

	//FOnBeginHighReady OnBeginHighReady;
	//FOnEndHighReady OnEndHighReady;


	//StepEquip때 대입한다.
	//지정한 값을 쉽게 사용하기 위해 Cast해둔다.
	UPROPERTY()
	UWeaponPDA* WeaponDataAsset;

	UPROPERTY()
	class UWeaponPartsManagerObject* WeaponPartsManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* OptionalStaticMesh;

	UPROPERTY()
	FTransform OriginalWeaponTransform;

	

	bool bLMBDown;

	bool bIsFiring;
	bool bIsAiming; //Main에서 값을 단순히 넣어주기만 한다.

	bool bIsTacticalDevEmit;

	//Clipping을 하고있을때 Aim을 막고, BulletOut의 Trace를 수정하기 위한 boolean
	bool bIsHighReady;

	float AlphaTime;
	float RecoilAlphaTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | SpawnParts")
	bool bHasSpawnParts;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | SpawnParts")
	TArray<UCustomPDA*> SpawnMuzzlePartsList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | SpawnParts")
	TArray<UCustomPDA*> SpawnScopePartsList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | SpawnParts")
	TArray<UCustomPDA*> SpawnTacticalPartsList;

	/* FPS Aim모드 일때 위치값 저장 -> PDA에 저장함.*/
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	//FTransform CharFPMeshTransform;
	//
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Transform")
	//FTransform WeapSKMeshTransform;


	//Primary, Sub을 지정한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ERifleSlot RifleAssign;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponFiringMode WeaponFiringMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponState CurrentWeaponState;


	FTimerHandle AimInitHandle;
	FTimerHandle RecoilHandle;


	/********* Ammo *********/
	//소모한 탄 개수
	int32 ConsumeAmmoCnt = 0;

	//탄창에 남은 탄약의 개수
	int32 AmmoLeftInMag = 0;

	//Inv에 남은 같은 타입 탄약의 총 개수 forWeaponStatusWidget.
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

	void AddSpawnParts(TArray<UCustomPDA*>SpawnParts);

protected:	
	void UpdateAim();
	virtual void BeginPlay() override;
	virtual bool StepEquip(AActor* Char, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX) override;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	void SetMesh() override;
	void GunAttachToMesh(AActor* Actor);
	void GunAttachToSubSocket(AActor* Actor);


	void SettingWeaponPartsManager();

	FTransform GetSightSocketTransform();
	
	UFUNCTION()
	void UpdateWeaponParts();

	//for WeaponStatusWidget
	void GetTotalAmmo();
	
	int32 GetCurrentAmmoInMag();


	void ToggleTacticalEquip();

	void ChangeSafetyLever();
	void ChangeSafetyLeverForAI(EWeaponFiringMode FiringMode);
	
	/* Attack */
	void StartFire();
	void StopFire();

	void Equipping();
	void EndEquipping();

	bool CheckCanReload();
	void Reload();
	void ReloadEnd();

	void PlayReloadSound(EPlayReloadSound SoundType);

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