// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IntelListItemObject.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UIntelListItemObject : public UObject
{
	GENERATED_BODY()
public:
	//external variable
	FText IntelName;

	//internal variable
	EIntelType IntelType;

};
