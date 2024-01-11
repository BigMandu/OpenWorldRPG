// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonButton.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UCommonButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Settings", meta = (bindwidget))
	class UButton* CommonBtn;
	UPROPERTY(BlueprintReadWrite, Category = "Settings", meta = (bindwidget))
	class UTextBlock* BtnText;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Value")
	FText TextContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Value")
	FSlateColor TextColor; /*FLinearColor TextColor;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Value")
	FSlateFontInfo TextInfo;

protected:
	virtual void NativePreConstruct() override;
	//virtual void NativeConstruct() override;

public:
	void SettingButton();
};
