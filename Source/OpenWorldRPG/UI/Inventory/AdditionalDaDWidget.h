// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "AdditionalDaDWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UAdditionalDaDWidget : public UDragDropOperation
{
	GENERATED_BODY()
public:

	UUserWidget* AdditionalWidget;
	FVector2D WidgetDragLocation;

};
