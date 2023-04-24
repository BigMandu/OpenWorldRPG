// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryGenerator_FindSneakUp.h"

UEnvQueryGenerator_FindSneakUp::UEnvQueryGenerator_FindSneakUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AlignedPointsDistance.DefaultValue = 100.f;
	ConeDegrees.DefaultValue = 90.f;
	AngleStep.DefaultValue = 10.f;
	Range.DefaultValue = 1000.f;
	bIncludeContextLocation = false;
}

void UEnvQueryGenerator_FindSneakUp::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	Super::GenerateItems(QueryInstance);
}
