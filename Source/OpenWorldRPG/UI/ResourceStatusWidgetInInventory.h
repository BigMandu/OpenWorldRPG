// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceStatusWidgetInInventory.generated.h"

/**
 * 
 */
class UProgressBar;
class UTextBlock;

UCLASS()
class OPENWORLDRPG_API UResourceStatusWidgetInInventory : public UUserWidget
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<UStatManagementComponent> StatManager;

	UPROPERTY(meta = (bindwidget))
	UProgressBar* HPProgress;
	UPROPERTY(meta = (bindwidget))
	UProgressBar* StamProgress;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* HPCurrent;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* HPMax;
	
	UPROPERTY(meta = (bindwidget))
	UTextBlock* StamCurrent;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* StamMax;

protected:
	virtual void NativeConstruct() override;

public:
	void BindStatManager(TWeakObjectPtr<UStatManagementComponent> StatWizardManager);

	UFUNCTION()
	void UpdateHPRatio();
	UFUNCTION()
	void UpdateStamRatio();

	UFUNCTION()
	void UpdateCurrentHPNum();
	UFUNCTION()
	void UpdateCurrentStamNum();
	UFUNCTION()
	void UpdateMaxHPNum();
	UFUNCTION()
	void UpdateMaxStamNum();

};
