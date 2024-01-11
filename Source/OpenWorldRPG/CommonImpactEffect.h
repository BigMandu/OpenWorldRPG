// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenWorldRPG/GameData/CommonImpactEffectTable.h"
#include "CommonImpactEffect.generated.h"

UCLASS()
class OPENWORLDRPG_API ACommonImpactEffect : public AActor
{
	GENERATED_BODY()

	/*UPROPERTY()
	struct FCommonImpactEffectTable EffectTable;*/

	FHitResult Hit;

	class UNiagaraSystem* SetEffect;
	class USoundCue* SetSoundCue;

	FTransform ActorTF;

	UNiagaraSystem* GetImpactFX(struct FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType);
	USoundCue* GetImpactSound(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType);

	

protected:
	virtual void PostInitializeComponents() override;

public:	
	// Sets default values for this actor's properties
	ACommonImpactEffect();

	USceneComponent* DefaultRootComponent;

	void SetImpactEffect(FCommonImpactEffectTable EffectDT, FHitResult HitInfo, FVector HitForce, FTransform CIETF);

	//void PlayImpactEffect();

};
