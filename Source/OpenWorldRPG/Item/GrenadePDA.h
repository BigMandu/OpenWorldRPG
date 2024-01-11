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

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | CommonStat")
	TSubclassOf<class UDamageType> DmgType;

	//Cascade Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class UParticleSystem* Ca_GrenadeEffect;

	//Niagara Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class UNiagaraSystem* Ni_GrenadeEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class USoundCue* GrenadeEffectSound;

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	USoundCue* GrenadeBounceSound;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Animation")
	class UAnimMontage* ThrowingAnimMontage;

	

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Fragmentation")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeSpriteSizeMin;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeSpriteSizeMax;

	/** Value and Result 
	 * 1,0,0 : Red
	 * 0,1,0 : Green
	 * 0,0,1 : Blue
	 *
	 * 0,1,1 : bluegreen
	 * 1,1,0 : green but seems like toxin
	 * 1,0,1 : Purple
	 * 1,1,1 : White
	 *
	 * 2,1,1 : white but add some red
	 * 1,2,1 : white but add some green
	 * 1,1,2 : white but add soem blue
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	FVector SmokeColorRGBFloat;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeSpawnRate;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeRadius;
};
