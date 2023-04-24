// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_TargetEnemy.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UEnvQueryContext_TargetEnemy : public UEnvQueryContext
{
	GENERATED_BODY()
	virtual void ProvideContext(struct FEnvQueryInstance& QueryInstance, struct FEnvQueryContextData& ContextData) const override;
};
