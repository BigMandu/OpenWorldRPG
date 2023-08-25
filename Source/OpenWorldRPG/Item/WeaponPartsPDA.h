// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "WeaponPartsPDA.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UWeaponPartsPDA : public UCustomPDA
{
	GENERATED_BODY()
public:

#if WITH_EDITOR
	virtual bool CanEditChange( const FProperty* InProperty ) const override;
#endif

	UPROPERTY(EditAnywhere, Category = "Parts")
	EWeaponPartsType WeaponPartsType;


	//Must set if this is tactical parts
	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	ETacticalPartsType TacticalPartsType;

	//Must set if this is tactical laser parts
	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")//, meta = (EditCondition = "TacticalParts"))
	class UNiagaraSystem* TacticalEmitEffect;
	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	UNiagaraSystem* TacticalImpactEmitEffect;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float TacticalEmitLimitRange;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	FLinearColor LaserEmitColor;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float LaserWidth;


	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float Flashlight_InnerConeRadius;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float Flashlight_OuterConeRadius;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float Flashlight_Distance;
	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float Flashlight_Brightness;


	/* Adjust Attach Location */
	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForM4;

	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForAK;

	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForM9;

};
