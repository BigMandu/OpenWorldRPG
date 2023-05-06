// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "WeaponPDA.generated.h"

/**
 * 
 */

USTRUCT()
struct FWeaponAnim
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Fire Animation")
	class UAnimMontage* FPS_Actor_FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Animation")
	UAnimMontage* TPS_Actor_FireAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Reload Animation")
	UAnimMontage* FPS_Actor_ReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Reload Animation")
	UAnimMontage* TPS_Actor_ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Equip Animation")
	UAnimMontage* FPS_Actor_EquipAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Equip Animation")
	UAnimMontage* TPS_Actor_EquipAnim;

	
	UPROPERTY(EditDefaultsOnly, Category = "ADS Animation")
	UAnimMontage* FPS_ADS_Anim;

};

USTRUCT()
struct FWeaponSound
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	class USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	USoundCue* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	USoundCue* ClickSound;


};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 AmmoPerMag;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	EAmmoType AmmoType;

	//Weapon의 Damage
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float Damage;

	//Weapon의 사거리
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;

	/* 탄이 흩어지는 정도.*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float HipBulletSpread;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float AimBulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float FireRatePerMin;
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float FireRatePerSec;
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Stat")
	float SecondPerBullet;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Burst")
	bool bHasBurstMode;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Burst", meta = (EditCondition = "bHasBurstMode"))
	int32 BurstRound;

	/* Recoil */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Recoil")
	UCurveFloat* Recoil_X;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Recoil")
	UCurveFloat* Recoil_Y;

	/* FWeaponStat Defaults */
	FWeaponStat()
	{
		AmmoPerMag = 30;

		WeaponRange = 5000.f;
		HipBulletSpread = 1.5f;
		AimBulletSpread = 1.f;
		
		//m4a1은 분당 700~950발.분당 950으로 잡고 초당 15.8발을 쏘면됨. 0.06초당 한발씩.
		FireRatePerMin = 950;

		/*FireRatePerSec이나 SecondPerBullet은
		 * FireRatePerMin이 Weapon마다 변할 수 있으니.
		 * Equip함수에서 계산하여 저장한다.
		 */

		bHasBurstMode = false;
		BurstRound = 3;

	}
};

UCLASS()
class OPENWORLDRPG_API UWeaponPDA : public UCustomPDA
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Weapon | WeaponStat")
	FWeaponStat WeaponStat;
	
	UPROPERTY(EditAnywhere, Category = "Weapon | IMG")
	UTexture2D* WeaponVectorIMG;


	/* FPS Aim 모드일때 위치값 저장 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim Transform")
	FTransform CharFPMeshTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim Transform")
	FTransform WeapSKMeshTransform;


	//FPMesh의 WeaponGrip 소켓에 붙일 Weapon의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform FPMeshAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform PrimaryWeaponAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform SubWeaponAttachTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform PistolAttachTransform;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Capsule")
	FTransform CapsuleComponentTF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Capsule")
	float CapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Capsule")
	float CapsuleRadius;



	TWeakObjectPtr<class UNewItemObject> MuzzleParts;
	TWeakObjectPtr<UNewItemObject> TacticalParts;
	TWeakObjectPtr<UNewItemObject> ScopeParts;
	/*class UNewItemObject* MuzzleParts;
	UNewItemObject* TacticalParts;
	UNewItemObject* ScopeParts;*/


	/* FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | FX")
	class UParticleSystem* FireMuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | FX")
	UParticleSystem* BulletHitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	TSubclassOf<UCameraShakeBase> CamShake;	

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animation")
	FWeaponAnim WeaponAnimaton;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Sound")
	FWeaponSound WeaponSound;

};
