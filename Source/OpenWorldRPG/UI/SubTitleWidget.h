// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubTitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API USubTitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(meta = (bindwidget))
	class UTextBlock* Name;
	UPROPERTY(meta = (bindwidget))
	UTextBlock* Context;


	virtual void NativeConstruct() override;

	void SetSub(FText CharacterName, FText Script, bool bBlueTeam);
	void RemoveSub();
};
