// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "GrenadeBase.generated.h"

/**
 * 
 */

 /* Grenade가 터진걸 AICon에 알리기 위한 Delegate*/
 DECLARE_MULTICAST_DELEGATE_OneParam(FOnGrenadeDestroy, AGrenadeBase*);


UCLASS()
class OPENWORLDRPG_API AGrenadeBase : public AWeapon
{
	GENERATED_BODY()

protected:

	//Cascade Effect
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* Ca_ParticleComp;
	
	//Niagara Effect
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* Ni_ParticleComp;

	class UAudioComponent* EffectAudioComp;
	UAudioComponent* BounceAudioComp;
	


	FTimerHandle EffectTriggerTimerHandle;
	FTimerHandle EffectDurationTimerHandle;
	FTimerHandle DestoryGrenadeTimerHandle;

private:
	
	//void FragmentEffect(class UGrenadePDA* gPDA);
	//void SmokeEffect(UGrenadePDA* gPDA);

protected:
	UFUNCTION()
	virtual void BeginEffect();	

public:
	AGrenadeBase();

	FOnGrenadeDestroy OnGrenadeDestroy;

	UPROPERTY(BlueprintReadOnly)
	bool bReadyToThrow;

	UPROPERTY(BlueprintReadOnly)
	bool bThrow;

	virtual void OnConstruction(const FTransform& Transform) override;

	//Play bounce sound
	UFUNCTION()
	void OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void AttachToHand(class ABaseCharacter* Actor, class UNewItemObject* Obj, bool bIsNeedToDestory = true) override;

	void ReadyToThrow();
	//called when LMB up
	void ThrowGrenade(ABaseCharacter* Actor);
	
	void DetectThrow(ABaseCharacter* Actor);

	UFUNCTION()
	virtual void EndEffect();

	UFUNCTION()
	void StartDestory();

};
