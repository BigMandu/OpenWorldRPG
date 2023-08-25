// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CQBMissionResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UCQBMissionResultWidget : public UUserWidget
{
	GENERATED_BODY()
	TWeakObjectPtr<class AMainController> PlayerCon;
public:
	UPROPERTY(meta=(bindwidget))
	class UButton* CloseButton;
	UPROPERTY(meta=(bindwidget))
	class UTextBlock* Time;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* PerfectDrillCnt;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* Score;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* BonusScore;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* TotalScore;

	//virtual void NativeConstruct() override;

	void AfterConstruct(AMainController* Pcon);
	void SetVariable(const float& T_ElapsedTime, const int32& T_PerfectDrillCnt, const int32& T_TotalScore);

	UFUNCTION()
	void ClosedWidget();

};
