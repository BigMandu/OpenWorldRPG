// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "ShootingTargetSMComponent.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UShootingTargetSMComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = ShootingTarget)
	int32 Score;
	UPROPERTY(EditAnywhere, Category = ShootingTarget)
	ETargetMeshType TargetMeshType;
};
