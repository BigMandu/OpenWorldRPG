// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomInventoryLibrary.generated.h"

/**
 * 
 */
class UNewItemObject;

UCLASS()
class OPENWORLDRPG_API UCustomInventoryLibrary : public UObject
{
	GENERATED_BODY()

public:

	static void BackToItem(UNewItemObject* ItemObj);
};
