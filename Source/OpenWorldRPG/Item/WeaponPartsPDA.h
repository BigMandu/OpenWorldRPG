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

	UPROPERTY(EditAnywhere, Category = "Parts")
	EWeaponPartsType WeaponPartsType;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	class UNiagaraSystem* TacticalEmitEffect;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float TacticalEmitLimitRange;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	FLinearColor LaserEmitColor;

	UPROPERTY(EditAnywhere, Category = "Parts | Tactical")
	float LaserWidth;




	/* Adjust Attach Location */
	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForM4;

	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForAK;

	UPROPERTY(EditAnywhere, Category = "Parts | AttachLocation")
	FVector AttachLocForM9;

};
