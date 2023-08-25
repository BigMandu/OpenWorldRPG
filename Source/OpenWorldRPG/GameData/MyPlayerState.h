// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "MyPlayerState.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionSetting);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionProgress);

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
private:

	int32 HeadShotCount;
	int32 KillCount;

	int32 DeathCount;

	float HeadshotRate;

public:

	//FOnMissionSetting OnMissionSetting;
	//FOnMissionProgress OnMissionProgress;

	//저장 해야함
	UPROPERTY()
	struct FIntelAcquiredInfo IntelAcq;
	//저장 해야함.
	//UPROPERTY()
	//struct FInProgressMissionInfo CurrentMissionInfo;


	/* Functions */
private:
	
	//bool CheckAllIntelAcquired();	

	//void SetCurrentMissionInfo(struct FMissionDataTableRow* Rowdata);

	//bool CheckInProgressMission(AActor& MissionObject);

	//void UpdateMissionProgress();
	//void UpdateToNextMission();

public:
	AMyPlayerState(const FObjectInitializer& ObjectInitializer);


	virtual void PostInitializeComponents() override;

	void AfterPossess();

	/* =Caution!!= Overview Widget에서 처음 한번만 강제로 업데이트 하기 위해 사용됨. 그 외 사용 금지.*/
	//void ForcedMissionUpdate();

	void AddHeadShotCount(AActor* KillerPlayer);


	//void EvaluateMissionStatus(AActor* Object = nullptr);

	int32 GetHeadShotCount()
	{
		return HeadShotCount;
	}

	void AddKillCount(AActor* KillerPlayer);

	int32 GetKillCount()
	{
		return KillCount;
	}

	float GetHeadshotRate()
	{
		return HeadShotCount/KillCount;
	}

	void AddDeathCount()
	{
		++DeathCount;
	}

	int32 GetDeathCount()
	{
		return DeathCount;
	}

	void GetIntel(EIntelType GetIntelType, AActor* MissionObject);

	bool CanReadThisIntel(EIntelType ReadIntelType);

};
