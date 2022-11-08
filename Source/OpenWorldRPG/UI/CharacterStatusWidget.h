// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

/**
 * 
 */
 class UProgressBar;
 class UStatManagementComponent;

 
UCLASS()
class OPENWORLDRPG_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	
	UStatManagementComponent* StatManger;

	UPROPERTY(meta=(bindwidget))
	UProgressBar* HPBar;
	UPROPERTY(meta = (bindwidget))
	UProgressBar* StaminaBar;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomColor")
	FLinearColor DefaultStamBarColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomColor")
	FLinearColor BelowMinimumStamBarColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomColor")
	FLinearColor ExhaustedRecoveryStamBarColor;

	void BindStatManager(UStatManagementComponent* NewManageComponent);

	UFUNCTION()
	void UpdateHPRatio();
	UFUNCTION()
	void UpdateStaminaRatio();
};
