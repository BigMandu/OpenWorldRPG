// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "BaseGrenade.generated.h"

/**
 * 
 */

 /* Grenade가 터진걸 AICon에 알리기 위한 Delegate*/
 DECLARE_MULTICAST_DELEGATE_OneParam(FOnGrenadeDestroy, ABaseGrenade*);

UCLASS()
class OPENWORLDRPG_API ABaseGrenade : public AWeapon
{
	GENERATED_BODY()
private:
	FTimerHandle EffectTriggerTimerHandle;
	FTimerHandle EffectDurationTimerHandle;



	//TWeakObjectPtr<class AWeapon> BeforeEquipppedWeapon = nullptr;
	//TWeakObjectPtr<class UNewItemObject> GObj = nullptr;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UProjectileMovementComponent* ProjectileMovementComp;

	//Cascade Effect
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* Ca_ParticleComp;

	//Niagara Effect
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* Ni_ParticleComp;

	class UAudioComponent* EffectAudioComp;

	class UAudioComponent* BounceAudioComp;

public:
	ABaseGrenade();

	FOnGrenadeDestroy OnGrenadeDestroy;

	UPROPERTY(BlueprintReadOnly)
	bool bReadyToThrow;

	UPROPERTY(BlueprintReadOnly)
	bool bThrow;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	void AttachToHand(class ABaseCharacter* Actor, class UNewItemObject* Obj, bool bIsNeedToDestory = true) override;


	void ReadyToThrow();
	//called when LMB up
	void ThrowGrenade(ABaseCharacter* Actor);
	void DetectThrow(ABaseCharacter* Actor);
private:
	//void EquipBeforeWeapon(ABaseCharacter* Actor);
	
	UFUNCTION()
	void BeginEffect();

	UFUNCTION()
	void EndEffect();

	void FragmentEffect(class UGrenadePDA* gPDA);
	void SmokeEffect(UGrenadePDA* gPDA);
};
