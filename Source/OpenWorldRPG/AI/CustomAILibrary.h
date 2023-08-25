// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomAILibrary.generated.h"

/**
 * 
 */

 class AEnemyCharacter;

UCLASS()
class OPENWORLDRPG_API UCustomAILibrary : public UObject
{
	GENERATED_BODY()


public:
	static bool CheckIsInside(const AEnemyCharacter& AIChar);

};
