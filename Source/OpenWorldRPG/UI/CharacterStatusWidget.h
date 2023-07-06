// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class OPENWORLDRPG_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(bindwidget))
	UTextBlock* LevelText;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* LevelNum;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* ExpText;
	UPROPERTY(meta=(bindwidget))
	class UProgressBar* ExpProgressBar;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CurrentExpText;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* RemainingExpText;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CurrentExpNum;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* RemainingExpNum;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* CurrentStatText;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NextLevelStatText;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* CStamina_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CStamDrain_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CAimInitRate_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CJumpHeight_text;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* CStamina_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CStamDrain_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CAimInitRate_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* CJumpHeight_num;


	UPROPERTY(meta=(bindwidget))
	UTextBlock* NStamina_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NStamDrain_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NAimInitRate_text;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NJumpHeight_text;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* NStamina_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NStamDrain_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NAimInitRate_num;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* NJumpHeight_num;

	UPROPERTY()
	TWeakObjectPtr<class UStatManagementComponent> StatManger;
	/* FUNCTIONS */

	void BindStatManager(class UStatManagementComponent* NewManageComponent);

	UFUNCTION()
	void UpdateLevelAndStat();
	UFUNCTION()
	void UpdateExpPoint();

};
