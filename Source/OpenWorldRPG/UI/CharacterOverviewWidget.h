// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverviewWidget.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class OPENWORLDRPG_API UCharacterOverviewWidget: public UUserWidget
{
	GENERATED_BODY()

	FTextFormat AcqAndReqCnt;
public:

	/* ------------------- Stats ----------------- */

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

	/* ------------------- Mission ----------------- */
	UPROPERTY(meta=(bindwidget))
	UTextBlock* MissionName;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* MissionDescription;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* MissionAquiredObjectCount;

	UPROPERTY(meta=(bindwidget))
	UTextBlock* Choice1Name;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* Choice1Description;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* Choice2Name;
	UPROPERTY(meta=(bindwidget))
	UTextBlock* Choice2Description;

	UPROPERTY()
	class AOpenWorldRPGGameModeBase* Gmode;
	


	/* FUNCTIONS */

protected:
	virtual void NativeConstruct() override;

public:

	void BindStatManager(class UStatManagementComponent* NewManageComponent);

	//void BindPlayerState(AMyPlayerState* Var_PState);
	void BindMissionFunc();


	UFUNCTION()
	void UpdateLevelAndStat();
	UFUNCTION()
	void UpdateExpPoint();

	UFUNCTION()
	void UpdateMissionProgress();
	UFUNCTION()
	void UpdateMission();

};
