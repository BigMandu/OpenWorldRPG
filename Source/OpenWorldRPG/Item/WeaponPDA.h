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
	UAnimMontage* FPSAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Fire Animation")
	UAnimMontage* TPSAnim;

	UPROPERTY(EditDefaultsOnly, Category = "ADS Animation")
	UAnimMontage* FPS_ADS_Anim;

};

USTRUCT()
struct FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 AmmoPerMag;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	EAmmoType AmmoType;

	//Weapon�� Damage
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float Damage;

	//Weapon�� ��Ÿ�
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	float WeaponRange;

	/* ź�� ������� ����.*/
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
		
		//m4a1�� �д� 700~950��.�д� 950���� ��� �ʴ� 15.8���� ����. 0.06�ʴ� �ѹ߾�.
		FireRatePerMin = 950;

		/*FireRatePerSec�̳� SecondPerBullet��
		 * FireRatePerMin�� Weapon���� ���� �� ������.
		 * Equip�Լ����� ����Ͽ� �����Ѵ�.
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


	/* FPS Aim ����϶� ��ġ�� ���� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim Transform")
	FTransform CharFPMeshTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim Transform")
	FTransform WeapSKMeshTransform;


	//FPMesh�� WeaponGrip ���Ͽ� ���� Weapon�� Transform��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform FPMeshAttachTransform;

	//TPMesh�� Weapon�� Attach�� ������ Transform��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Transform")
	FTransform PrimaryWeaponAttachTransform;

	//TPMesh�� Weapon�� Attach�� ������ Transform��.
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
	class USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | FX")
	USoundCue* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | FX")
	class UParticleSystem* FireMuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | FX")
	UParticleSystem* BulletHitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | FX")
	TSubclassOf<UCameraShakeBase> CamShake;	

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animation")
	FWeaponAnim Animaton;

};
