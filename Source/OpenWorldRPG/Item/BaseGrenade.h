// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "BaseGrenade.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ABaseGrenade : public AWeapon
{
	GENERATED_BODY()
private:
	FTimerHandle EffectTimerHandle;
	FTimerHandle UpdateAimTimerHandle;

	TWeakObjectPtr<class AWeapon> BeforeEquipppedWeapon = nullptr;
	TWeakObjectPtr<class UNewItemObject> GObj = nullptr;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UProjectileMovementComponent* ProjectileMovementComp;

public:
	ABaseGrenade();
	virtual void OnConstruction(const FTransform& Transform) override;
	void AttachToHand(class ABaseCharacter* Actor, class UNewItemObject* Obj);

	void ReadyToThrow();

	void ThrowGrenade(ABaseCharacter* Actor);

private:
	void EquipBeforeWeapon(ABaseCharacter* Actor);
	
	UFUNCTION()
	void BeginEffect();

	void FragmentEffect();
	void SmokeEffect();
};
