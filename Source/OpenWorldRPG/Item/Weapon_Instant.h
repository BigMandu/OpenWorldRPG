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

	/* FBulletData Defaults */
	FBulletData()
	{
		Damage = 10;
	}
};

UCLASS(Abstract)
class OPENWORLDRPG_API AWeapon_Instant : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeapon_Instant(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	FBulletData BulletStat;

	virtual void BulletOut() override;
	FHitResult BulletTrace(FVector& StartTrace, FVector& EndTrace);

	void CheckHit(FHitResult& Hit);
};
