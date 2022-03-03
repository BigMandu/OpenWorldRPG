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
class UTextBlock;

UCLASS()
class OPENWORLDRPG_API UNewItemObject : public UObject
{
	GENERATED_BODY()

public:
	UNewItemObject();
	AItem* item;
	bool bCanRotated;
	bool bRotated;

	FIntPoint itemsize;

	FText itemName;
	FText itemDesc;

	UMaterialInterface* icon;
	UMaterialInterface* iconRotated;


	int32 TopLeftIndex;

	UFUNCTION(BlueprintCallable)
	FIntPoint GetItemSize();

	UClass* GetItemClass();
	void ItemRotate();

	UMaterialInterface* GetItemIcon();
};
