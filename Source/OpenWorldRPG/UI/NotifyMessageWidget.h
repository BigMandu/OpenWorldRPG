// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotifyMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UNotifyMessageWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Message;

	FTimerHandle DisplayedTimer;


	/* functions */
	virtual void NativeConstruct() override;
	void SetMessageText(FText msg);

	UFUNCTION()
	void RemoveThisMessage();
};
