// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_Cone.h"
#include "EnvQueryGenerator_FindSneakUp.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UEnvQueryGenerator_FindSneakUp : public UEnvQueryGenerator_Cone
{
	GENERATED_BODY()
	UEnvQueryGenerator_FindSneakUp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;
	
};
