// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "CustomDDOperation.generated.h"

/**
 * 
 */
class UNewItemwidget;
class UNewItemObject;
class UNewInventoryGrid;

UCLASS()
class OPENWORLDRPG_API UCustomDDOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:

	UNewItemObject* ItemObj;
	UNewInventoryGrid* ParentGridWidget;
	UNewItemwidget* DragWidget;
};
