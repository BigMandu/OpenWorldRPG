// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewItemwidget.generated.h"

/**
 * 
 */
class UNewItemObject;
class USizeBox;
class UBorder;
class UImage;

UCLASS()
class OPENWORLDRPG_API UNewItemwidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UNewItemObject* ItemObj;
	float Tilesize;
	FVector2D widgetsize;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BackgroundSizeBox;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BackgroundBorder;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	
	void Refresh();

	UFUNCTION(BlueprintCallable)
	FSlateBrush GetIconImage();
};
