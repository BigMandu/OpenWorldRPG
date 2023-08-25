// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Equipment.h"
//#include "OpenWorldRPG/Item/Weapon.h"
#include "WeaponParts.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AWeaponParts : public AWeapon
{
	GENERATED_BODY()
private:
	UNewItemObject* OwnerWeapon_OBJ;
	TWeakObjectPtr<class AWeapon> OwnerWeapon;

	FVector EmitEndPosition;
public:
	AWeaponParts();
	bool bEmit;

	//UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* LaserEffect;
	UNiagaraComponent* LaserEndEffect;

	class USpotLightComponent* FlashLightComp;


	virtual void Tick(float DeltaTime) override;
	void SetEmitValue(bool bNeedToEmit);

	void SetOwnerWeapon(AWeapon* Weapon);
	void AdjustAttachLocation();

	void GetEmitEndLocation();
	void GetFinalEmitEndLocation();

	void SetEndPointEffectLocation(FVector Location);

	void Emit();
	//UNewItemObject* GetOwnerWeapon();

	//void SetOwnerWeapon(UNewItemObject* OwnerWeapon);

	//void RemoveOwnerWeapon(UNewItemObject* OwnerWeapon);
	
};
