// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Weapon_Instant.generated.h"

/**
 * 
 */
USTRUCT()
struct FBulletData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	int32 Damage;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	float HipBulletSpread;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	float AimBulletSpread;

	/* FBulletData Defaults */
	FBulletData()
	{
		Damage = 10;
		HipBulletSpread = 300.f;
		AimBulletSpread = 50.f;
	}
};

UCLASS()
class OPENWORLDRPG_API AWeapon_Instant : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeapon_Instant();
	
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	FBulletData BulletStat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* BulletHitEffect;
	
	//1~3¹ß µ¿¾È ÅºÂøÁöÁ¡À» ¿Ã¸°µÚ, ÀÌÈÄ ÅºÂøÁ¡À» À§ÇÔ.
	float LastZpos;


	FVector PreviousSpread;
	FVector NextSpread;

	virtual void BulletOut() override;
	FVector BulletSpread(FVector Vec);

	void CheckHit(FHitResult& Hit);

	void CalcRecoilNApply(FVector *PreSpread, FVector *NexSpread);
	float PitchRecoilValue(float Zvalue);
	float YawRecoilValue(FVector Vec);
};
