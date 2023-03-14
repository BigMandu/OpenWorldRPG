// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	class UTextBlock* InteractText;

	class AMainCharacter* MainChar;

	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText InterSetText();
};
