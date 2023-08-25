// Copyright Epic Games, Inc. All Rights Reserved.


#include "OpenWorldRPGGameModeBase.h"

#include "Components/VerticalBox.h"
//#include "Kismet/GameplayStatics.h"

#include "OpenWorldRPG/GameData/MyGameInstance.h"
#include "OpenWorldRPG/Save/SaveWorld.h"
#include "OpenWorldRPG/UI/MainMenuWidget.h"

#include "OpenWorldRPG/SpawnVolume.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "OpenWorldRPG/Mission/MissionVolume.h"
#include "OpenWorldRPG/Mission/MissionWeapon_Instant.h"

//Blueprint'/Game/Blueprints/DamageType/DmgType_Falling.DmgType_Falling'
AOpenWorldRPGGameModeBase::AOpenWorldRPGGameModeBase() : Super()
{
	FString DmgtypePath = TEXT("/Game/Blueprints/DamageType/DmgType_Falling");
	ConstructorHelpers::FClassFinder<UMyDamageType>FallingDmgTypeOBJ(*DmgtypePath);

	if (FallingDmgTypeOBJ.Succeeded())
	{
		FallingDmgType = FallingDmgTypeOBJ.Class;
	}
}

void AOpenWorldRPGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	checkf(NewPlayer, TEXT("Player is nullptr"));
	Super::PostLogin(NewPlayer);

	/* Mission Setting */
	/** InProgress mission unique name이 비어있다면, (first gameplay)
	*	M001N 이라는 UnqID Mission을 첫 미션으로 설정한다.
	*/
	if (CurrentMissionInfo.InProgressMissionUnqID.IsEmpty())
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase// First game play! set InprogressMission"));
		//mission info를 초기화 한다.
		if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
		{
			UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// First game play! set InprogressMission"));
			FString MapNameWithPrefix = World->GetMapName();
			MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
			if (MapNameWithPrefix.Contains(TEXT("Training")))
			{
				UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Set BootCamp Tutorial Mission!"));
				FString TableName(GInst->TutorialMissionDataTable->GetName());

				if (FMissionDataTableRow* Row = GInst->TutorialMissionDataTable->FindRow<FMissionDataTableRow>(FName(TEXT("BCM001")), TableName))
				{
					SetCurrentMissionInfo(Row);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Set Main Mission!"));
				FString TableName(GInst->MainMissionDataTable->GetName());

				if (FMissionDataTableRow* Row = GInst->MainMissionDataTable->FindRow<FMissionDataTableRow>(FName(TEXT("M001N")), TableName))
				{
					SetCurrentMissionInfo(Row);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMyPlayerState// Already has mission"));
	}
	
}

void AOpenWorldRPGGameModeBase::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogTemp,Warning,TEXT("AOpenWorldRPGGameModeBase::StartPlay"));

	
	UMyGameInstance* Inst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (Inst)
	{
		//Inst->Hide Loading Scren()
		Inst->StopLoadingScreen();		
	}
	PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());

	if(PlayerCon)
	{
		FInputModeGameOnly Inputmode;
		PlayerCon->SetInputMode(Inputmode);
		PlayerCon->SetShowMouseCursor(false);
		/*GetWorld()->GetFirstPlayerController()->SetInputMode(Inputmode);
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);*/
		/*if (PCon->MainHud)
		{
			Mainhud = Mainhud;
		}*/
	}
	
}

void AOpenWorldRPGGameModeBase::ShowSystemNotiMessage(ESystemNotificationMessageType MsgType)
{
	if (PlayerCon->MainHud)
	{
		PlayerCon->MainHud->SetNotificationMessage(MsgType);
	}
}

void AOpenWorldRPGGameModeBase::ShowGameProgressNotiMessage(EGameProgressNotificationMessageType MsgType)
{
	if (PlayerCon->MainHud)
	{
		PlayerCon->MainHud->SetNotificationMessage(MsgType);
	}
}

void AOpenWorldRPGGameModeBase::SaveWorldStatus()
{
	FString SlotName = TEXT("SaveWorld");
	int32 Index = 0;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false)
	{
		SaveGame_World = Cast<USaveWorld>(UGameplayStatics::CreateSaveGameObject(USaveWorld::StaticClass()));
	}
	else
	{
		SaveGame_World = Cast<USaveWorld>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));
	}


	if (SaveGame_World == nullptr) return;

	//Save시에 그냥 통으로 날리고 새로 저장하도록 한다.
	SaveGame_World->SavedSpawnVolStatusMap.Empty();


	//World에 배치된 Spawn Vol을 전부 구한다.
	TArray<AActor*> WorldVolumeList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), WorldVolumeList);	

	/*Spawnvolume을 순회하면서
	* spawnVolume의 DestroyCount와 Spawnvol자체를 Map에 저장한다
	* */
	for (auto GetActor : WorldVolumeList)
	{
		if (GetActor == nullptr) continue;
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(GetActor);
		if (SpawnVolume)
		{
			UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::SaveWorldStatus// %s status saved "), *SpawnVolume->GetFName().ToString());
			//SaveGame_World->SavedSpawnVolume.Add(SpawnVolume);
			SaveGame_World->SavedSpawnVolStatusMap.Add(SpawnVolume, SpawnVolume->DestoryCount);
		}
	}

	UGameplayStatics::SaveGameToSlot(SaveGame_World, SlotName, Index);
}

void AOpenWorldRPGGameModeBase::LoadWorldStatus()
{
	FString SlotName = TEXT("SaveWorld");
	int32 Index = 0;

	if ( UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::SaveWorldStatus // Load Fail!"));
	}

	USaveWorld* LoadGame_World = Cast<USaveWorld>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));

	if (LoadGame_World == nullptr) return;

	TArray<AActor*> WorldVolumeList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), WorldVolumeList);

	/* save와 비슷하다.
	* 우선 world에 배치된 spawnvol을 순회하면서
	* loadGame에 있는 Map에서 spawnvol을 비교하며 일치하면 destroycnt를 그대로 대입시킨다.
	*/
	for (auto GetActor : WorldVolumeList)
	{
		if (!GetActor) continue;
		ASpawnVolume* SpawnVol = Cast<ASpawnVolume>(GetActor);
		if (SpawnVol)
		{
			int32* DestoryCnt = LoadGame_World->SavedSpawnVolStatusMap.Find(SpawnVol);
			SpawnVol->DestoryCount = *DestoryCnt;
			UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::LoadWorldStatus //VolName : %s , DestCnt : %d"), *SpawnVol->GetFName().ToString(), *DestoryCnt);
		}
	}

}




float AOpenWorldRPGGameModeBase::ModifyApplyDamage(float BeforeDamage, struct FDamageEvent const& DamageEvent, class AActor* DamagedVictim, class AController* EventInstigator, AActor* DamageCauser)
{
	FVector ImpulseDir;
	FHitResult Hit;
	AActor* HitInstigator = DamageCauser ? DamageCauser : nullptr;
	DamageEvent.GetBestHitInfo(DamagedVictim, HitInstigator, Hit, ImpulseDir);

	
	float BonusDamageFactor = 1.f;

	FString HitBoneName = Hit.BoneName.ToString();
	UE_LOG(LogTemp, Warning, TEXT("BChar::HitBone: %s"), *HitBoneName);


	if (HitBoneName.Contains("Head", ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HeadShot!"));
		BonusDamageFactor = 1.55f;
	}
	else if (HitBoneName.Contains("Neck", ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::Neck Shot!"));
		BonusDamageFactor = 1.3f;
	}
	else if (HitBoneName.Contains("Spine_03", ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::Chest Shot!"));
		BonusDamageFactor = 1.3f;
	}


	//Damage formula
	float Armor = 30.f; 
	//DamagedVictim->Armor <<- 나중에 이걸로 바꿔야함.
	float CalculateDamage = BeforeDamage * BonusDamageFactor * ( 1 - Armor / 100 );

	return CalculateDamage;
}


void AOpenWorldRPGGameModeBase::ProcessKillEvent(AController* Killer, AController* KilledChar, struct FDamageEvent const& DamageEvent)
{
	AMyPlayerState* KillerState = Killer ? Killer->GetPlayerState<AMyPlayerState>() : nullptr;
	AMyPlayerState* VictimState = KilledChar ? KilledChar->GetPlayerState<AMyPlayerState>() : nullptr;

	if (KillerState && Killer != KilledChar)
	{
		FVector ImpulseDir;
		FHitResult Hit;
		AActor* HitInstigator = Killer->GetCharacter() ? Killer->GetCharacter() : nullptr;
		DamageEvent.GetBestHitInfo(KilledChar, HitInstigator, Hit, ImpulseDir);

		if (Hit.IsValidBlockingHit())
		{
			FString HitBoneName = Hit.BoneName.ToString();
			if ( HitBoneName.Contains("Head", ESearchCase::IgnoreCase, ESearchDir::FromStart) )
			{
				KillerState->AddHeadShotCount(Killer->GetCharacter());
			}

			KillerState->AddKillCount(Killer->GetCharacter());

		}

		//KillerState->KillScore
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::you Kill Enemy!"));
	}

	if (VictimState)
	{
		UMyDamageType* DmgType = Cast<UMyDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DmgType)
		{
			VictimState->AddDeathCount();

			if (APlayerController* PCon = Cast<APlayerController>(KilledChar))
			{
				UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::You Died"));
				if (PCon->bForceFeedbackEnabled)
				{
					FForceFeedbackParameters FFBParams;
					FFBParams.bLooping = false;
					FFBParams.Tag = "Death";
					FFBParams.bPlayWhilePaused = true;
					//PCon->PlayDynamicForceFeedback()
					PCon->ClientPlayForceFeedback(DmgType->KilledForceFB, FFBParams);
				}
			}
		}
		//VictimState->death
	}
}


void AOpenWorldRPGGameModeBase::ProcessHitEvent(AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent)
{
	//Player가 맞았을 경우
	if (AMainController* pcon = Cast<AMainController>(DamageReceiver))
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HitEvent, I'm Hit!!"));
		/*여기서 Playercon의 함수 호출하자.
		* Show Hit indicator 함수?를 만들어서 호출하면 될듯.
		*/
		if(!PlayerCon->MainHud) return;
		PlayerCon->MainHud->NotifyHitbySomething(DamageGiver, DamageReceiver, DamageEvent);
	}

	//Player가 enemy를 때려서 맞췄을 경우
	if (AMainController* pcon = Cast<AMainController>(DamageGiver))
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HitEvent, you Hit someone!!"));
		/*맞췄다는 표시를 Hud에 띄우자.
		*/
		if (!PlayerCon->MainHud) return;
		PlayerCon->MainHud->NotifyHitSomething();
	}

}











///////////////////////////////////////////////////////////////////////
/*                               Mission                             */
///////////////////////////////////////////////////////////////////////


/** DataTable을 loop하면서 Prerequisite (선행 미션)이 있고,
*	그 선행 미션의 Unique id가 지금 진행중이던 Mission의 UnqId면
*	loop돌던 Mission의 Data로 mission을 교체한다.
*/
void AOpenWorldRPGGameModeBase::UpdateToNextMission()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	bool bUpdateComplete = false;

	if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
	{
		bool bNextMissionUpdateFlag = false;
		FString MapNameWithPrefix = World->GetMapName();
		MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
		if (MapNameWithPrefix.Contains(TEXT("Training")))
		{
			for (auto looprow : GInst->TutorialMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)looprow.Value;

				//만약 진행중이던 미션이 AI와 Interact하는 미션이었다면
				// 미션 UniqID와 비교후, 동일하다면 다음미션을 update한다.
				if (bIsAIInteractMission)
				{
					//flag가 true일 경우 해당 미션으로 update하고 AIInteracMission flag를 초기화한다.
					if (bNextMissionUpdateFlag)
					{
						bIsAIInteractMission = false;
						SetCurrentMissionInfo(Row);
						bUpdateComplete = true;
						break;
					}
					//미션 ID가 동일하다면 flag를 true로 변경한다.
					else if (Row->MissionUniqueID.Equals(CurrentMissionInfo.InProgressMissionUnqID))
					{
						bNextMissionUpdateFlag = true;
					}
				}
				else if (Row->bHasPrerequisiteMission && Row->PrerequisiteUniqueID == CurrentMissionInfo.InProgressMissionUnqID)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
			}
		}
		else
		{
			for (auto loopRow : GInst->MainMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)loopRow.Value;

				if (bIsAIInteractMission)
				{
					//flag가 true일 경우 해당 미션으로 update하고 AIInteracMission flag를 초기화한다.
					if (bNextMissionUpdateFlag)
					{
						bIsAIInteractMission = false;
						SetCurrentMissionInfo(Row);
						bUpdateComplete = true;
						break;
					}
					//미션 ID가 동일하다면 flag를 true로 변경한다.
					else if (Row->MissionUniqueID.Equals(CurrentMissionInfo.InProgressMissionUnqID))
					{
						bNextMissionUpdateFlag = true;
					}
				}
				else if (Row->bHasPrerequisiteMission && Row->PrerequisiteUniqueID == CurrentMissionInfo.InProgressMissionUnqID)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
			}
		}
	}

	if (bUpdateComplete)
	{
		MissionUpdateNotify();
	}

	return;
}

void AOpenWorldRPGGameModeBase::UpdateMissionToThis(FString UpdateMissionUniqID)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	bool bUpdateComplete = false;

	if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
	{
		FString MapNameWithPrefix = World->GetMapName();
		MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
		if (MapNameWithPrefix.Contains(TEXT("Training")))
		{
			for (auto looprow : GInst->TutorialMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)looprow.Value;

				if (Row->MissionUniqueID == UpdateMissionUniqID)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
			}
		}
		else
		{
			for (auto loopRow : GInst->MainMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)loopRow.Value;

				if (Row->MissionUniqueID == UpdateMissionUniqID)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
			}
		}
	}

	if (bUpdateComplete)
	{
		MissionUpdateNotify();
	}
	return;
}

void AOpenWorldRPGGameModeBase::UpdateToNextMissionByID(FString CurrentMissionUniqID)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	bool bUpdateComplete = false;

	if (UMyGameInstance* GInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
	{
		bool bNextMissionUpdateFlag = false;
		FString MapNameWithPrefix = World->GetMapName();
		MapNameWithPrefix.RemoveFromStart(World->StreamingLevelsPrefix);
		if (MapNameWithPrefix.Contains(TEXT("Training")))
		{
			for (auto looprow : GInst->TutorialMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)looprow.Value;

				if (bNextMissionUpdateFlag)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
				//미션 ID가 동일하다면 flag를 true로 변경한다.
				else if (Row->MissionUniqueID.Equals(CurrentMissionUniqID))
				{
					bNextMissionUpdateFlag = true;
				}
				
			}
		}
		else
		{
			for (auto loopRow : GInst->MainMissionDataTable->GetRowMap())
			{
				FMissionDataTableRow* Row = (FMissionDataTableRow*)loopRow.Value;
								
				if (bNextMissionUpdateFlag)
				{
					SetCurrentMissionInfo(Row);
					bUpdateComplete = true;
					break;
				}
				//미션 ID가 동일하다면 flag를 true로 변경한다.
				else if (Row->MissionUniqueID.Equals(CurrentMissionUniqID))
				{
					bNextMissionUpdateFlag = true;
				}
				
			}
		}
	}

	if (bUpdateComplete)
	{
		MissionUpdateNotify();
	}
	return;
}


/** Mission Data를 CurrentMissionInfo struct에 담는다.
*/
void AOpenWorldRPGGameModeBase::SetCurrentMissionInfo(struct FMissionDataTableRow* Rowdata)
{
	
	
	if (Rowdata->MissionObjective)
	{
		CurrentMissionInfo.RequireObjectType = Rowdata->MissionObjective;
		//CurrentMissionInfo.RequireObjectType = Rowdata->MissionObjective->GetDefaultObject<AActor>();//Cast<AActor>(Rowdata->MissionObjective->GetClass());//Cast<AActor>(Rowdata->MissionObjective.Get());
	}
	if (Rowdata->Choice1Objective)
	{
		CurrentMissionInfo.Choice1RequireObjectType = Rowdata->Choice1Objective;
		//CurrentMissionInfo.Choice1RequireObjectType = Rowdata->Choice1Objective->GetDefaultObject<AActor>();
	}
	if (Rowdata->Choice2Objective)
	{
		CurrentMissionInfo.Choice2RequireObjectType = Rowdata->Choice2Objective;
		//CurrentMissionInfo.Choice2RequireObjectType = Rowdata->Choice2Objective->GetDefaultObject<AActor>();
	}
	

	//만약 해당 mission의 object를 표시할 수 있다면, widget을 표시한다.
	if (Rowdata->bCanShowHint)
	{
		ShowMissionObjectNotifyWidget(*Rowdata);
	}

	CurrentMissionInfo.InProgressMissionUnqID = Rowdata->MissionUniqueID;
	CurrentMissionInfo.MissionType = Rowdata->Missiontype;
	CurrentMissionInfo.MissionActionType = Rowdata->MissionActionType;
	CurrentMissionInfo.MissionAIUnqID = Rowdata->CharacterUniqueID;

	CurrentMissionInfo.RequireObjectCount = Rowdata->RequiredObjectCount;
	CurrentMissionInfo.AcquiredObjectCount = 0;
	CurrentMissionInfo.MissionName = Rowdata->MissionName;
	CurrentMissionInfo.MissionDescription = Rowdata->MissionDescription;
	
	CurrentMissionInfo.Choice1Name = Rowdata->Choice1Name;
	CurrentMissionInfo.Choice1Desc = Rowdata->Choice1Description;
	CurrentMissionInfo.Choice1MissionUniqID = Rowdata->Choice1MissionUniqID;
	CurrentMissionInfo.Choice1AIUnqID = Rowdata->Character1UniqueID;

	CurrentMissionInfo.Choice2Name = Rowdata->Choice2Name;
	CurrentMissionInfo.Choice2Desc = Rowdata->Choice2Description;
	CurrentMissionInfo.Choice2MissionUniqID = Rowdata->Choice2MissionUniqID;
	CurrentMissionInfo.Choice2AIUnqID = Rowdata->Character2UniqueID;

	//UCharacterOverviewWidget에 bind됨.
	OnMissionSetting.Broadcast();
}

FInProgressMissionInfo AOpenWorldRPGGameModeBase::GetCurrentMission()
{
	return CurrentMissionInfo;
}

/** 강제로 broadcast를 하기 위한 func.
*/
void AOpenWorldRPGGameModeBase::ForcedMissionUpdate()
{
	//UCharacterOverviewWidget에 bind됨.
	OnMissionSetting.Broadcast();
}

/** 
 * Mission Object를 Interact하면 호출하는 함수.
 */
bool AOpenWorldRPGGameModeBase::EvaluateMissionStatus(AActor* Object)
{
	if(Object == nullptr) return false;
	IMissionInterface* MInter = Cast<IMissionInterface>(Object);
	if(MInter == nullptr) return false;

	bool bReturn = false;

	//Normal mission
	if (CurrentMissionInfo.MissionType == EMissionType::EMT_Normal)
	{
		if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Collect || CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Kill)
		{
			if (CheckInProgressMission(Object))
			{
				UpdateMissionProgress();
				return true; //bReturn = true;
			}
		}
		else
		{
			if (CheckValidated(*MInter, CurrentMissionInfo.MissionAIUnqID, CurrentMissionInfo.InProgressMissionUnqID))
			{
				UpdateToNextMission();
				return true; //bReturn = true;
			}
		}
	}
	//Choice mission
	else
	{
		/**
		 * Choice mission인 경우, 검증 단계를 통과한다면
		 * choice mission만을 위한 bool 변수를 true로 toggle한 뒤
		 * 해당 MissionUniqueID로 미션을 Update한다.
		 */
		bool bChoice1 = false;
		bool bCanUpdateSpecificMission = false;

		if (CheckValidated(*MInter, CurrentMissionInfo.Choice1AIUnqID, CurrentMissionInfo.Choice1MissionUniqID))
		{
			bCanUpdateSpecificMission = true;
			bChoice1 = true;
		}
		if (!bChoice1 && CheckValidated(*MInter, CurrentMissionInfo.Choice2AIUnqID, CurrentMissionInfo.Choice2MissionUniqID))
		{
			bCanUpdateSpecificMission = true;
		}


		if (bCanUpdateSpecificMission)
		{
			/* if CurrentMission Type is Choice mission,
			* delete Hint widget .
			*/
			HideMissionObjectNotifyWidget();

			//만약 choice 미션에 ai였다면 아래 bool변수가 toogle됐을것임.\
					//초기화 해줘야한다. 어차피 choice missio에서는 UPdateMission을 사용하지 않기 때문임.
			if (bIsAIInteractMission)
			{
				if (bChoice1)
				{
					UpdateMissionToThis(CurrentMissionInfo.Choice1MissionUniqID);
					//UpdateToNextMission();
					return true;
				}
				else
				{
					UpdateMissionToThis(CurrentMissionInfo.Choice2MissionUniqID);
					//UpdateToNextMission();
					return true;
				}

			}
			else
			{
				if (bChoice1)
				{
					UpdateToNextMissionByID(CurrentMissionInfo.Choice1MissionUniqID);
					return true;
				}
				else
				{
					UpdateToNextMissionByID(CurrentMissionInfo.Choice2MissionUniqID);
					return true;
				}

			}
		}
	}


	/*if (bReturn)
	{
		UpdateMissionProgress();
	}*/

	return bReturn;
}


/** 코드 간소화.
 * Object의 Character Uniq ID부터 확인한다.
 * 
 */
bool AOpenWorldRPGGameModeBase::CheckValidated(IMissionInterface& MInter, FString AIUnqID, FString MissionUnqID)
{
	//Character Uniq ID가 비어있지 않다면, AI Character Interaction Mission임.
	//Mission Data table이 요구하는 CharUniqID면 true리턴하고 마친다.
	if (MInter.CharacterUniqueID_Interface.IsEmpty() == false)
	{
		if (MInter.CharacterUniqueID_Interface.Equals(AIUnqID))
		{
			/**
			 * for update mission.
			 * 만약, AI와 Interact 하는 mission이었다면
			 * 바로 다음 미션으로 update하기 위한 flag.
			 */
			bIsAIInteractMission = true;
			return true;
		}
	}
	else
	{
		if (MInter.MissionUniqueID_Interface.Equals(MissionUnqID))
		{
			return true;
		}
	}
	return false;
}

/** 지금 수집한 Object가   현재 진행중인 미션의 Object의 Child class인 경우
*	true를 return하기 위해서
*	BP class로 Cast를 한뒤, SuperClass(Source class)를 얻어와 비교를 하도록 한다.
*/
bool AOpenWorldRPGGameModeBase::CheckInProgressMission(AActor* MissionObject)
{
	//Mission에 관련된 class는 IMissionInterface의 클래스로 derived하기 때문에
	// 모든 Missionclass가 childof로 되는거 같다... 때문에 기존 코드를 수정함.
	//if (MissionObject.GetClass()->IsChildOf(CurrentMissionInfo.AcquireObjectType->StaticClass()))
	//UClass* ObjClass = MissionObject->GetClass();

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(MissionObject->GetClass());
	if (BPClass)
	{
		UClass* ParentClass = BPClass->GetSuperClass();
		
		UClass* MissionObjClass = CurrentMissionInfo.RequireObjectType->GetDefaultObject<AActor>()->GetClass(); //->GetClass();
		if (ParentClass == MissionObjClass)
		{
			if (IMissionInterface* MissionI = Cast<IMissionInterface>(MissionObject))
			{
				if (MissionI->MissionUniqueID_Interface.Equals(CurrentMissionInfo.InProgressMissionUnqID))
				{
					return true;
				}
			}
		}

	}

	return false;
}


/** InProgress mission의 object acquired count를 증가시키고,
*	Mission의 require obj count를 만족한다면 UpdateToNextMission function을 호출한다.
*/
void AOpenWorldRPGGameModeBase::UpdateMissionProgress()
{
	bool bCanUpdateMission = false;
	if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Collect || CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Kill)
	{
		CurrentMissionInfo.AcquiredObjectCount = FMath::Clamp(CurrentMissionInfo.AcquiredObjectCount += 1, 0, CurrentMissionInfo.RequireObjectCount);

		OnMissionProgress.Broadcast();

		if (CurrentMissionInfo.AcquiredObjectCount >= CurrentMissionInfo.RequireObjectCount)
		{

			bCanUpdateMission = true;

		}
	}
	else// if (CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Reach || CurrentMissionInfo.MissionActionType == EMissionActionType::EMAT_Interact)
	{
		bCanUpdateMission = true;
	}

	if (bCanUpdateMission)
	{
		UpdateToNextMission();
	}
}

void AOpenWorldRPGGameModeBase::ShowMissionObjectNotifyWidget(FMissionDataTableRow& rowData)
{
	UWorld* World = GetWorld() ? GetWorld() : nullptr;
	if(!World) return;

	if (rowData.Missiontype == EMissionType::EMT_Normal)
	{
		UCustomSystemLibrary::ToggleDisplayHintsWidget(World, true, rowData.MissionObjective, rowData.MissionUniqueID, rowData.CharacterUniqueID, rowData.MissionActionType);
	}
	//choice mission이면 choice1과 choice2의 object에 hint를 표시한다.
	else
	{
		UCustomSystemLibrary::ToggleDisplayHintsWidget(World, true, rowData.Choice1Objective, rowData.Choice1MissionUniqID, rowData.Character1UniqueID);
		UCustomSystemLibrary::ToggleDisplayHintsWidget(World, true, rowData.Choice2Objective, rowData.Choice2MissionUniqID, rowData.Character2UniqueID);
	}

	// Old version.
	/*TArray<AActor*> MissionObjects;
	
	if(rowData.Missiontype == EMissionType::EMT_Normal)
	{
		switch (rowData.MissionActionType)
		{
			case EMissionActionType::EMAT_Reach:
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMissionVolume::StaticClass(), MissionObjects);
				break;
			case EMissionActionType::EMAT_Kill:
			case EMissionActionType::EMAT_Interact:
			case EMissionActionType::EMAT_Collect:
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), rowData.MissionObjective, MissionObjects);
				break;
		}
	}


	if (MissionObjects.Num() > 0)
	{
		for (auto Obj : MissionObjects)
		{
			if (IMissionInterface* MissionI = Cast<IMissionInterface>(Obj))
			{
				if (MissionI->MissionUniqueID_Interface.Equals(rowData.MissionUniqueID))
				{
					MissionI->ShowNotifyIcon();
				}
			}
		}
	}*/
}

void AOpenWorldRPGGameModeBase::HideMissionObjectNotifyWidget()
{
	UWorld* World = GetWorld() ? GetWorld() : nullptr;
	if (!World) return;

	UCustomSystemLibrary::ToggleDisplayHintsWidget(World, false, CurrentMissionInfo.Choice1RequireObjectType, CurrentMissionInfo.Choice1MissionUniqID, CurrentMissionInfo.Choice1AIUnqID);
	UCustomSystemLibrary::ToggleDisplayHintsWidget(World, false, CurrentMissionInfo.Choice2RequireObjectType, CurrentMissionInfo.Choice2MissionUniqID, CurrentMissionInfo.Choice2AIUnqID);
}


void AOpenWorldRPGGameModeBase::MissionUpdateNotify()
{
	/* Mission Success,
	*  Sound And notifyUI
	*/

	UE_LOG(LogTemp,Warning,TEXT("AOpenWorldRPGGameModeBase::MissionUpdateNotify // Complete Mission! and update mission! "));

}



///////////////////////////////////////////////////////////////////////////////
/*                             CQB 전용 MIssion                              */
///////////////////////////////////////////////////////////////////////////////


/** 전용 Mission Door를 Interaction하면 아래 함수를 호출하자.
 * 
 */
void AOpenWorldRPGGameModeBase::StartCQBMission()
{
	GetWorldTimerManager().ClearTimer(CQBMissionTimer);
	bStartedCQBMission = true;
	UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::StartCQBMission"));
	GetWorldTimerManager().SetTimer(CQBMissionTimer, [=]{
		CQBTimer += GetWorld()->GetDeltaSeconds();
	}, GetWorld()->GetDeltaSeconds(),true);
}

/** 전용 Mission Target에 TakeDmg를 주고, TripleTab함수가 완료되면 아래 함수를 호출하자.
 * 
 */
void AOpenWorldRPGGameModeBase::OnlyCQBMission(int32 PerfectCnt, int32 TotalScore)
{
	CQBMission_PerfectCnt += PerfectCnt;
	CQBMission_TotalScore += TotalScore;
}

/** 전용 mission volume에 닿으면 아래 함수를 호출하자.
 * 
 */
void AOpenWorldRPGGameModeBase::EndCQBMission()
{
	UWorld* world = GetWorld() ? GetWorld() : nullptr;
	if(!world) return;

	if (PlayerCon && bStartedCQBMission)
	{
		bStartedCQBMission = false;
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::ENDCQBMission"));
		PlayerCon->ShowCQBResult(CQBTimer, CQBMission_PerfectCnt, CQBMission_TotalScore);
	}
	GetWorldTimerManager().ClearTimer(CQBMissionTimer);
	CQBTimer = 0;
	CQBMission_PerfectCnt = 0;
	CQBMission_TotalScore = 0;
	

	TArray<AActor*> Actorarr;
	UGameplayStatics::GetAllActorsOfClass(world, ATargetBindVolume::StaticClass(), Actorarr);
	InitCQBMissionAsset(true, Actorarr);
	Actorarr.Empty();
	UGameplayStatics::GetAllActorsOfClass(world, ADoor::StaticClass(), Actorarr);
	InitCQBMissionAsset(false, Actorarr);
}


void AOpenWorldRPGGameModeBase::InitCQBMissionAsset(bool bIsTargetBindVolume, TArray<AActor*>& OutActors)
{
	if(OutActors.Num() < 1) return;

	for (auto Obj : OutActors)
	{
		if (bIsTargetBindVolume)
		{
			if (ATargetBindVolume* TBV = Cast<ATargetBindVolume>(Obj))
			{
				TBV->bIsAlreadyOccurredOverlap = false;
			}
		}
		else
		{
			if (ADoor* MD = Cast<ADoor>(Obj))
			{
				if (MD->DoorType == EDoorType::EDT_LockedDoor)
				{
					MD->LockTheDoor();
				}
			}
		}
	}

}