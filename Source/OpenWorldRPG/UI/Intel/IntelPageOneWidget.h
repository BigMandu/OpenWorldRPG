// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntelPageOneWidget.generated.h"

/**
 * 
 */

 class UImage;
 class UTextBlock;

UCLASS()
class OPENWORLDRPG_API UIntelPageOneWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	/*UPROPERTY(meta = (bindwidget))
	UImage* BackgroundPaper;*/

	UPROPERTY(meta = (bindwidget))
	class UCanvasPanel* PageCanvas;

	UPROPERTY(meta = (bindwidget))
	UImage* LogoImage;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* DateText;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* FirstTitleItem;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* SecondTitleItem;
	/*UPROPERTY(meta = (bindwidget))
	UTextBlock* ThirdTitleItem;*/
	UPROPERTY()
	UTextBlock* ThirdText;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* FourthTitleItem;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* ContentText;

	//UPROPERTY(meta = (bindwidget))
	//class UBorder* ClassificationBorder;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* Classification_TOP;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* Classification_BOT;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* Additional_TOP;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* Additional_BOT;

	//Page에 data를 집어넣는다.
	void SettingPage(const FIntelligenceDataTable& Data, const int32& pageindex);

	//Classification, Additional의 위젯 위치를 중앙으로 고정시킨다.
	void AdjustWidgetLocation();

	FVector2D GetUWidgetPosition(UWidget* WidgetElement);
	void SetUWidgetPosition(UWidget* WidgetElement, FVector2D Position);
};
