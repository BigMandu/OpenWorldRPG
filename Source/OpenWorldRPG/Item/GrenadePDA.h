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

	//Cascade Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class UParticleSystem* Ca_GrenadeEffect;

	//Niagara Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class UNiagaraSystem* Ni_GrenadeEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade | FX")
	class USoundCue* GrenadeEffectSound;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Animation")
	class UAnimMontage* ThrowingAnimMontage;

	/*UPROPERTY(EditDefaultsOnly, Category = "Grenade | CommonStat")
	TSubclassOf<class UDamageType> DmgType;*/

	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Fragmentation")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade | Smoke")
	float SmokeDuration;

};
