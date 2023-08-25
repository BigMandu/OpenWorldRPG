// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntelPageOtherWidget.generated.h"

/**
 * 
 */
 class UImage;
 class UTextBlock;

UCLASS()
class OPENWORLDRPG_API UIntelPageOtherWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	/*UPROPERTY(meta = (bindwidget))
	UImage* BackgroundPaper;*/

	UPROPERTY(meta = (bindwidget))
	UTextBlock* ContentText;


	UPROPERTY(meta = (bindwidget))
	UTextBlock* Classification_TOP;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* Classification_BOT;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* Additional_TOP;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* Additional_BOT;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* PageNumber;

	void AdjustWidgetLocation();

	FVector2D GetUWidgetPosition(UWidget* WidgetElement);
	void SetUWidgetPosition(UWidget* WidgetElement, FVector2D Position);
};
