// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UNewItemObject;

UCLASS()
class OPENWORLDRPG_API UTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UTextBlock* ItemName;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UTextBlock* ItemDesc;


	void SetData(UNewItemObject* Obj, float tilesize);
};
