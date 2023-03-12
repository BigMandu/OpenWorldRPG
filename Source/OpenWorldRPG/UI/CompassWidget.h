// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	FVector NPLocation;
	FTimerHandle CompassHeldTimer;

public:
	
	UPROPERTY(meta=(BindWidget))
	class UImage* ComBackground;

	UPROPERTY(meta=(BindWidget))
	UImage* ComPointer;

	UFUNCTION()
	void PrepareCompassUpdate();

	UFUNCTION()
	void UpdateCompassPointer();
};
