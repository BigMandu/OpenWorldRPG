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

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FPSAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TPSAnim;
};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

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

	/* Recoil */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_X;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* Recoil_Y;

	/* FWeaponStat Defaults */
	FWeaponStat()
	{
		WeaponRange = 5000.f;
		HipBulletSpread = 1.5f;
		AimBulletSpread = 1.f;

		AmmoPerMag = 30;

		bHasBurstMode = true;
		BurstRound = 3;


		//m4a1은 분당 700~950발.분당 950으로 잡고 초당 15.8발을 쏘면됨. 0.06초당 한발씩.
		FireRatePerMin = 950;

		/*FireRatePerSec이나 SecondPerBullet은
		 * FireRatePerMin이 Weapon마다 변할 수 있으니.
		 * Equip함수에서 계산하여 저장한다.
		 */

	}
};

UCLASS()
class OPENWORLDRPG_API UWeaponPDA : public UCustomPDA
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "WeaponStat")
	FWeaponStat WeaponStat;
	

	/* FPS Aim 모드일때 위치값 저장 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equip | Aim Transform")
	FTransform CharFPMeshTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equip | Aim Transform")
	FTransform WeapSKMeshTransform;


	//FPMesh의 WeaponGrip 소켓에 붙일 Weapon의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Transform")
	FTransform FPMeshAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Transform")
	FTransform PrimaryWeaponAttachTransform;

	//TPMesh에 Weapon을 Attach할 소켓의 Transform값.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Transform")
	FTransform SubWeaponAttachTransform;


	/* FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* EquippedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* FireMuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* BulletHitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> CamShake;	

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim FireAnimaton;

};
