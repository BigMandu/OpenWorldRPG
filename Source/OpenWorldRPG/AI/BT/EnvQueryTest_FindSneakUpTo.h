// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_FindSneakUpTo.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UEnvQueryTest_FindSneakUpTo : public UEnvQueryTest
{
	GENERATED_BODY()

		UEnvQueryTest_FindSneakUpTo(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category=Trace)
	TSubclassOf<UEnvQueryContext> Context;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	FVector FindIdealSneakUpLocation(AActor* ContextObject, AActor* TargetActor) const;
};
