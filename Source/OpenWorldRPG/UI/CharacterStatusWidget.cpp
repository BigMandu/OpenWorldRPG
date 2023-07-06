// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CharacterStatusWidget.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "Components/ProgressBar.h"


void UCharacterStatusWidget::BindStatManager(UStatManagementComponent* NewManageComponent)
{
	if ( NewManageComponent )
	{
		StatManger = NewManageComponent;

		StatManger->OnLevelUp.AddDynamic(this, &UCharacterStatusWidget::UpdateLevelAndStat);
		StatManger->OnExpPointChange.AddDynamic(this, &UCharacterStatusWidget::UpdateExpPoint);
		
		UpdateLevelAndStat();
		UpdateExpPoint();
		
	}
}


void UCharacterStatusWidget::UpdateLevelAndStat()
{
	if ( StatManger.IsValid() )
	{
		const FCurrentMAXStats* const CStat = StatManger->CurrentMAXStats;		

		LevelNum->SetText(FText::AsNumber(StatManger->Level));

		CStamina_num->SetText(FText::AsNumber(CStat->Stamina));
		CStamDrain_num->SetText(FText::AsNumber(CStat->StaminaDrainRate));
		CAimInitRate_num->SetText(FText::AsNumber(CStat->AimInitializeRate));
		CJumpHeight_num->SetText(FText::AsNumber(CStat->JumpZ));


		if(StatManger->Level < 19 )
		{
			const FCurrentMAXStats* const NStat = StatManger->NextMAXStats;
			NStamina_num->SetText(FText::AsNumber(NStat->Stamina));
			NStamDrain_num->SetText(FText::AsNumber(NStat->StaminaDrainRate));
			NAimInitRate_num->SetText(FText::AsNumber(NStat->AimInitializeRate));
			NJumpHeight_num->SetText(FText::AsNumber(NStat->JumpZ));
		}
		else
		{
			FText emojinone = FText::FromString("-");
			NStamina_num->SetText(emojinone);
			NStamDrain_num->SetText(emojinone);
			NAimInitRate_num->SetText(emojinone);
			NJumpHeight_num->SetText(emojinone);
		}
	}
}

void UCharacterStatusWidget::UpdateExpPoint()
{
	if ( StatManger.IsValid() )
	{
		const FCharacterCurrentStat& CharStat = StatManger->CurrentStat;
		const FCurrentMAXStats* const CStat = StatManger->CurrentMAXStats;

		const float CurExp = CharStat.ExperiencePoint;
		const float ReqExp = CStat->NextExp;

		CurrentExpNum->SetText(FText::AsNumber(CurExp));
		RemainingExpNum->SetText(FText::AsNumber(ReqExp - CurExp));

		ExpProgressBar->SetPercent(CurExp/ReqExp*100.f);
	}
}