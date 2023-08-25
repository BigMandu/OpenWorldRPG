// Fill out your copyright notice in the Description page of Project Settings.


#include "CQBMissionResultWidget.h"
#include "OpenWorldRPG/MainController.h"
#include "Components/Button.h"

//void UCQBMissionResultWidget::NativeConstruct()
//{
//	
//}

void UCQBMissionResultWidget::AfterConstruct(AMainController* Pcon)
{
	PlayerCon = Pcon;
	CloseButton->OnClicked.AddDynamic(this,&UCQBMissionResultWidget::ClosedWidget);
}

void UCQBMissionResultWidget::SetVariable(const float& T_ElapsedTime, const int32& T_PerfectDrillCnt, const int32& T_TotalScore)
{
	float ElsapsedTime = FMath::TruncateToHalfIfClose(T_ElapsedTime);
	int32 BonusScoreCnt = T_PerfectDrillCnt * 5;
	int32 ScoreCnt = T_TotalScore - BonusScoreCnt;

	if (ScoreCnt + BonusScoreCnt == T_TotalScore)
	{
		UE_LOG(LogTemp,Warning,TEXT("UCQBMissionResultWidget:: Validation complete"));
	}

	Time->SetText(FText::AsNumber(ElsapsedTime));
	PerfectDrillCnt->SetText(FText::AsNumber(T_PerfectDrillCnt));
	Score->SetText(FText::AsNumber(ScoreCnt));
	BonusScore->SetText(FText::AsNumber(BonusScoreCnt));
	TotalScore->SetText(FText::AsNumber(T_TotalScore));
}

void UCQBMissionResultWidget::ClosedWidget()
{
	if (PlayerCon.IsValid())
	{
		PlayerCon->HideCQBResult();
	}
}