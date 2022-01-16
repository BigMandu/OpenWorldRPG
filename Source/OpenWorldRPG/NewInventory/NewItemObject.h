// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NewItemObject.generated.h"

/**
 * 
 */
class AItem;
class UTexture2D;

UCLASS()
class OPENWORLDRPG_API UNewItemObject : public UObject
{
	GENERATED_BODY()

public:
	UNewItemObject();
	AItem* item;
	bool bCanRotated;
	FIntPoint itemsize;

	UMaterialInterface* icon;
	UMaterialInterface* iconRotated;

	FIntPoint GetItemSize();
};
