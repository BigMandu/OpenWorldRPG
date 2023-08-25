// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/GameData/MyPlayerState.h"

AMyPlayerState::AMyPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	//AMainCharacter* TPlayer = Cast<AMainCharacter>(GetPawn()); //GetPawn<AMainCharacter>();
	//Player = TPlayer ? TPlayer : nullptr;
}

void AMyPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Mission Setting */
	/** InProgress mission unique name이 비어있다면, (first gameplay)
	*	M001N 이라는 UnqID Mission을 첫 미션으로 설정한다.
	*/
	//if (CurrentMissionInfo.InProgressMissionUnqID.IsEmpty())
	//{
	//	UWorld* World = GetWorld();
	//	if(World == nullptr) return;
	//	UE_LOG(LogTemp,Warning,TEXT("AMyPlayerState// First game play! set InprogressMission"));
	//	//mission info를 초기화 한다.
	//	if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// First game play! set InprogressMission"));
	//		FString MapNameWithPrefix = World->GetMapName();
	//		MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
	//		if (MapNameWithPrefix.Contains(TEXT("Training")))
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Set BootCamp Tutorial Mission!"));
	//			FString TableName(GInst->TutorialMissionDataTable->GetName());

	//			if (FMissionDataTableRow* Row = GInst->TutorialMissionDataTable->FindRow<FMissionDataTableRow>(FName(TEXT("BCM001")), TableName))
	//			{
	//				SetCurrentMissionInfo(Row);
	//			}
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Set Main Mission!"));
	//			FString TableName(GInst->MainMissionDataTable->GetName());

	//			if (FMissionDataTableRow* Row = GInst->MainMissionDataTable->FindRow<FMissionDataTableRow>(FName(TEXT("M001N")), TableName))
	//			{
	//				SetCurrentMissionInfo(Row);
	//			}
	//		}		
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Already has mission"));
	//}

}

void AMyPlayerState::AfterPossess()
{
	if (GetPawn())
	{
		//AMainCharacter* TPlayer = Cast<AMainCharacter>(GetPawn()); //GetPawn<AMainCharacter>();
		//Player = TPlayer ? TPlayer : nullptr;
	}
}

/*********************************************************************/
/*                               Mission                             */
/*********************************************************************/

/** DataTable을 loop하면서 Prerequisite (선행 미션)이 있고,
*	그 선행 미션의 Unique id가 지금 진행중이던 Mission의 UnqId면
*	loop돌던 Mission의 Data로 mission을 교체한다.	
*/
//void AMyPlayerState::UpdateToNextMission()
//{
//	UWorld* World = GetWorld();
//	if(World == nullptr) return;
//	if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
//	{
//		FString MapNameWithPrefix = World->GetMapName();
//		MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
//		if (MapNameWithPrefix.Contains(TEXT("Training")))
//		{
//			for (auto looprow : GInst->TutorialMissionDataTable->GetRowMap())
//			{
//				FMissionDataTableRow* Row = (FMissionDataTableRow*)looprow.Value;
//				if (Row->bHasPrerequisiteMission && Row->PrerequisiteUniqueID == CurrentMissionInfo.InProgressMissionUnqID)
//				{
//					SetCurrentMissionInfo(Row);
//				}
//			}
//		}
//		else
//		{
//			for (auto loopRow : GInst->MainMissionDataTable->GetRowMap())
//			{
//				FMissionDataTableRow* Row = (FMissionDataTableRow*)loopRow.Value;
//
//				if (Row->bHasPrerequisiteMission && Row->PrerequisiteUniqueID == CurrentMissionInfo.InProgressMissionUnqID)
//				{
//					SetCurrentMissionInfo(Row);
//				}
//			}
//		}
//
//		
//	}
//}


/** Mission Data를 CurrentMissionInfo struct에 담는다.
*/
//void AMyPlayerState::SetCurrentMissionInfo(struct FMissionDataTableRow* Rowdata)
//{
//	CurrentMissionInfo.InProgressMissionUnqID = Rowdata->MissionUniqueID;
//	CurrentMissionInfo.RequireObjectCount = Rowdata->RequiredObjectCount;
//	CurrentMissionInfo.AcquiredObjectCount = 0;
//
//	if (Rowdata->MissionObjective)
//	{
//		CurrentMissionInfo.AcquireObjectType = Rowdata->MissionObjective->GetDefaultObject<AActor>();//Cast<AActor>(Rowdata->MissionObjective->GetClass());//Cast<AActor>(Rowdata->MissionObjective.Get());
//	}
//
//	CurrentMissionInfo.MissionName = Rowdata->MissionName;
//	CurrentMissionInfo.MissionDescription = Rowdata->MissionDescription;
//	CurrentMissionInfo.MissionActionType = Rowdata->MissionActionType;
//	CurrentMissionInfo.Choice1Name = Rowdata->Choice1Name;
//	CurrentMissionInfo.Choice1Desc = Rowdata->Choice1Description;
//	CurrentMissionInfo.Choice2Name = Rowdata->Choice2Name;
//	CurrentMissionInfo.Choice2Desc = Rowdata->Choice2Description;
//
//	//UCharacterOverviewWidget에 bind됨.
//	//OnMissionSetting.Broadcast();
//}

/** 강제로 broadcast를 하기 위한 func.
*/
//void AMyPlayerState::ForcedMissionUpdate()
//{
//	//UCharacterOverviewWidget에 bind됨.
//	//OnMissionSetting.Broadcast();
//}


void AMyPlayerState::GetIntel(EIntelType GetIntelType, AActor* MissionObject)
{
	bool bChangeValue = false;
	switch (GetIntelType)
	{
		case EIntelType::EIT_Intel1:
			if (!IntelAcq.AcqIntel1)
			{
				IntelAcq.AcqIntel1 = true;
				bChangeValue = true;
			}
		break;
		case EIntelType::EIT_Intel2:
			if (!IntelAcq.AcqIntel2)
			{
				IntelAcq.AcqIntel2 = true;
				bChangeValue = true;
			}			
		break;
		case EIntelType::EIT_Intel3:
			if (!IntelAcq.AcqIntel3)
			{
				IntelAcq.AcqIntel3 = true;
				bChangeValue = true;
			}
		break;
		case EIntelType::EIT_Intel4:
			if (!IntelAcq.AcqIntel4)
			{
				IntelAcq.AcqIntel4 = true;
				bChangeValue = true;
			}
		break;
		case EIntelType::EIT_Intel5:
			if (!IntelAcq.AcqIntel5)
			{
				IntelAcq.AcqIntel5 = true;
				bChangeValue = true;
			}
		break;
	}

	if (bChangeValue)
	{
		if(AOpenWorldRPGGameModeBase* Gmode = Cast< AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			Gmode->EvaluateMissionStatus(MissionObject);
		}
		//EvaluateMissionStatus(MissionObject);
	}
}

//void AMyPlayerState::EvaluateMissionStatus(AActor* Object)
//{
//	if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Collect || CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Kill)
//	{
//		if (Object)
//		{
//			if (CheckInProgressMission(*Object))
//			{
//				UpdateMissionProgress();
//			}
//			
//		}
//	}
//	else if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Reach)
//	{
//		UpdateMissionProgress();
//	}
//}

/** 지금 수집한 Object가   현재 진행중인 미션의 Object의 Child class인 경우
*	true를 return한다.
*/
//bool AMyPlayerState::CheckInProgressMission(AActor& MissionObject)
//{
//	if (MissionObject.GetClass()->IsChildOf(CurrentMissionInfo.AcquireObjectType->StaticClass()))
//	{
//		return true;
//	}
//	return false;
//}


/** InProgress mission의 object acquired count를 증가시키고,
*	Mission의 require obj count를 만족한다면 UpdateToNextMission function을 호출한다.
*/
//void AMyPlayerState::UpdateMissionProgress()
//{
//	bool bCanUpdateMission = false;
//	if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Collect || CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Kill)
//	{
//		CurrentMissionInfo.AcquiredObjectCount = FMath::Clamp(CurrentMissionInfo.AcquiredObjectCount += 1, 0, CurrentMissionInfo.RequireObjectCount);
//
//		//OnMissionProgress.Broadcast();
//
//		if (CurrentMissionInfo.AcquiredObjectCount >= CurrentMissionInfo.RequireObjectCount)
//		{
//
//			bCanUpdateMission = true;
//			
//		}
//	}
//	else if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Reach)
//	{
//		bCanUpdateMission = true;
//	}
//
//	if (bCanUpdateMission)
//	{
//		/* Mission Success,
//		*  Sound And notifyUI
//		*/
//		UpdateToNextMission();
//	}
//}
//


bool AMyPlayerState::CanReadThisIntel(EIntelType ReadIntelType)
{
	switch ( ReadIntelType )
	{
		case EIntelType::EIT_Intel1:
			return IntelAcq.AcqIntel1;
			//return bIsAcquiredBook1;
		break;
		case EIntelType::EIT_Intel2:
			return IntelAcq.AcqIntel2;
			//return bIsAcquiredBook2;
			break;
		case EIntelType::EIT_Intel3:
			return IntelAcq.AcqIntel3;
			//return bIsAcquiredBook3;
			break;
		case EIntelType::EIT_Intel4:
			return IntelAcq.AcqIntel4;
			//return bIsAcquiredBook4;
			break;
		case EIntelType::EIT_Intel5:
			return IntelAcq.AcqIntel5;
			//return bIsAcquiredBook5;
			break;
	}
	return false;
}
//
//bool AMyPlayerState::CheckAllIntelAcquired()
//{
//	//if (bIsAcquiredBook1 && bIsAcquiredBook2 && bIsAcquiredBook3 && bIsAcquiredBook4 && bIsAcquiredBook5)
//	if(IntelAcq.AcqIntel1 && IntelAcq.AcqIntel2 && IntelAcq.AcqIntel3 && IntelAcq.AcqIntel4 && IntelAcq.AcqIntel5)
//	{
//		return true;
//	}
//	return false;
//}






/*********************************************************************/
/*                                Score                              */
/*********************************************************************/

void AMyPlayerState::AddHeadShotCount(AActor* KillerPlayer)
{
	if(KillerPlayer)
	{
		++HeadShotCount;
	
		AMainCharacter* Player = Cast<AMainCharacter>(KillerPlayer); //GetPawn<AMainCharacter>();	

		if (Player)
		{
			if (Player->StatManagementComponent)
			{
				Player->StatManagementComponent->AddExpPoint(100);
			}
		}
	}
}

void AMyPlayerState::AddKillCount(AActor* KillerPlayer)
{
	if (KillerPlayer)
	{
		++KillCount;
		AMainCharacter* Player = Cast<AMainCharacter>(KillerPlayer); //GetPawn<AMainCharacter>();
		if (Player)
		{
			if (Player->StatManagementComponent)
			{
				Player->StatManagementComponent->AddExpPoint(50);
			}
		}
	}
}