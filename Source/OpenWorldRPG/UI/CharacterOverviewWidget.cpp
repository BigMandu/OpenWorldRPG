// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CharacterOverviewWidget.h"
#include "OpenWorldRPG/GameData/MyPlayerState.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UCharacterOverviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		BindMissionFunc();
	}
	
}

void UCharacterOverviewWidget::BindMissionFunc()
{
	if (Gmode)
	{
	 //PlayerState에 multicast delegate를 놓고,
	 //여기서 bind를 하고.
	 //playerstate에서 quest item을 습득하거나,
	 //quest가 완료되어 다음 quest가 진행될때 broadcast를 하자.

		Gmode->OnMissionSetting.AddDynamic(this, &UCharacterOverviewWidget::UpdateMission);
		Gmode->OnMissionProgress.AddDynamic(this, &UCharacterOverviewWidget::UpdateMissionProgress);
		Gmode->ForcedMissionUpdate();
	}
}

//bind at MainCharacter class.
void UCharacterOverviewWidget::BindStatManager(UStatManagementComponent* NewManageComponent)
{
	if ( NewManageComponent )
	{
		StatManger = NewManageComponent;

		StatManger->OnLevelUp.AddDynamic(this, &UCharacterOverviewWidget::UpdateLevelAndStat);
		StatManger->OnExpPointChange.AddDynamic(this, &UCharacterOverviewWidget::UpdateExpPoint);
		
		UpdateLevelAndStat();
		UpdateExpPoint();		
	}
}

void UCharacterOverviewWidget::UpdateLevelAndStat()
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

void UCharacterOverviewWidget::UpdateExpPoint()
{
	if ( StatManger.IsValid() )
	{
		const FCharacterCurrentStat& CharStat = StatManger->CurrentStat;
		const FCurrentMAXStats* const CStat = StatManger->CurrentMAXStats;

		const float CurExp = CharStat.ExperiencePoint;
		const float ReqExp = CStat->NextExp;

		CurrentExpNum->SetText(FText::AsNumber(CurExp));
		RemainingExpNum->SetText(FText::AsNumber(ReqExp - CurExp));

		ExpProgressBar->SetPercent(CurExp/ReqExp);
	}
}

void UCharacterOverviewWidget::UpdateMissionProgress()
{
	if (Gmode)
	{
		int32 CurrentAcqCnt = Gmode->CurrentMissionInfo.AcquiredObjectCount;
		int32 RequireCnt = Gmode->CurrentMissionInfo.RequireObjectCount;

		//FText FmtTXT = FText::Format(TEXT("[{0}/{1}]"), FText::AsNumber(CurrentAcqCnt), FText::AsNumber(RequireCnt));
		FText FmtTXT = FText::Format(FText::FromString(TEXT("[{0}/{1}]")), FText::AsNumber(CurrentAcqCnt), FText::AsNumber(RequireCnt));

		//LOCTEXT
		//AcqAndReqCnt = FTextFormat(TEXT("{0}/{0}"), Args);
		MissionAquiredObjectCount->SetText(FmtTXT);
	}
}

void UCharacterOverviewWidget::UpdateMission()
{
	if (Gmode)
	{
		MissionName->SetText(Gmode->CurrentMissionInfo.MissionName);
		MissionDescription->SetText(Gmode->CurrentMissionInfo.MissionDescription);

		Choice1Name->SetText(Gmode->CurrentMissionInfo.Choice1Name);
		Choice1Description->SetText(Gmode->CurrentMissionInfo.Choice1Desc);
		Choice2Name->SetText(Gmode->CurrentMissionInfo.Choice2Name);
		Choice2Description->SetText(Gmode->CurrentMissionInfo.Choice2Desc);
		UpdateMissionProgress();
	}
}
