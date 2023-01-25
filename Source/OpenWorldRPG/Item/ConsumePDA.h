// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "ConsumePDA.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UConsumePDA : public UBasePDA
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	bool bIsHPRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	bool bIsStaminaRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	float RecoveryTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	float RecoveryAmount;

};
