// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "GrenadePDA.generated.h"

/**
 * 
 */


UCLASS()
class OPENWORLDRPG_API UGrenadePDA : public UBasePDA
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | CommonStat")
	EGrenadeType GrenadeType;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | CommonStat")
	float EffectDelayTime;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | CommonStat")
	float EffectRadius;



	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Fragmentation")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeDuration;

};
