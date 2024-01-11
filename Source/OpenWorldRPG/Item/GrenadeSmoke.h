// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/GrenadeBase.h"
//#include "OpenWorldRPG/Item/GrenadeHandler.h"
#include "GrenadeSmoke.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AGrenadeSmoke : public AGrenadeBase//, public IGrenadeHandler
{
	GENERATED_BODY()
private:
	/*UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SmokeEffectVisibleBlockSphere;*/
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* SmokeEffectVisibleBlock;

	FName SpriteSize_MIN;
	FName SpriteSize_MAX;
	FName SpriteColorRGBfloat;
	FName SpriteSpawnRate;
	FName SpriteSphereRadius;
protected:
	/*FTimerHandle EffectTriggerTimerHandle;
	FTimerHandle EffectDurationTimerHandle;*/


private:
	void VisibleBlock();

public:
	AGrenadeSmoke();
	virtual void BeginEffect() override;
	//virtual void EndEffect() override;

	//virtual void TriggerGrenade(ABaseCharacter* Actor);
	//virtual void DetectThrow(ABaseCharacter* Actor) override;
};
