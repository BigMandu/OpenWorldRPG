// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UDeathWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (bindwidget))
	class UButton* ContinueBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* MainmenuBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* ToDesktopBtn;

};
