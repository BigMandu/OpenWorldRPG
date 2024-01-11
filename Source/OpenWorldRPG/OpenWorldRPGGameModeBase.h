// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "OpenWorldRPGGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionSetting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartDeathEvent, AController*, TempController);

/**
 * 
 */
UCLASS(Blueprintable)
class OPENWORLDRPG_API AOpenWorldRPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	bool bIsAIInteractMission;

	//혹시나, mission을 거치지 않고 cqb door를 뿌셨을때를 가정하여 ..
	bool bStartedCQBMission;
	
public:
	

	UPROPERTY()
	class USaveWorld* SaveGame_World;

	//Falling Dmg type
	UPROPERTY()
	TSubclassOf<class UMyDamageType> FallingDmgType;

	/*UPROPERTY()
	class UMainHud* Mainhud;*/
	TSubclassOf<class UNotifyMessageWidget> W_NotifyMessage;

	/*UPROPERTY()
	class AMainController* PlayerCon;*/
	

	/* Mission */
	UPROPERTY()
	FOnMissionSetting OnMissionSetting;
	UPROPERTY()
	FOnMissionProgress OnMissionProgress;

	UPROPERTY()
	FOnStartDeathEvent OnStartDeathEvent;

	// need to save
	UPROPERTY()
	struct FInProgressMissionInfo CurrentMissionInfo;

	/* For CQB at Bootcamp */
	float CQBTimer;
	FTimerHandle CQBMissionTimer;
	int32 CQBMission_PerfectCnt;
	int32 CQBMission_TotalScore;

private:
	/* Mission */
	void SetCurrentMissionInfo(struct FMissionDataTableRow* Rowdata);
	bool CheckInProgressMission(AActor* MissionObject);

	void UpdateMissionProgress();
	void UpdateToNextMission();
	void UpdateToNextMissionByID(FString CurrentMissionUniqID);

	void ShowMissionObjectNotifyWidget(FMissionDataTableRow& rowData);
	void HideMissionObjectNotifyWidget();

	bool CheckValidated(class IMissionInterface& MInter, FString AIUnqID, FString MissionUnqID);


	/* Mission이 완료 및 update되면 notification widget이나 sound로 알려주기 위한 함수 */
	void MissionUpdateNotify();

	//CQBMission전용 함수..  함수 재활용을 위한 bool.
	void InitCQBMissionAsset(bool bIsTargetBindVolume, TArray<AActor*>& OutActors);

	void BindDeathEvent();


public:
	AOpenWorldRPGGameModeBase();
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	void RespawnPlayer(AController* TempController);


	/** Tries to spawn the player's pawn at a StartPlayerActor */
	virtual void RestartPlayer(AController* NewPlayer) override;
	/** Tries to spawn the player's pawn at a specific location */
	virtual void RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;


	/* called from MainCharacter, MainController class. */
	void ShowSystemNotiMessage(ESystemNotificationMessageType MsgType);

	void ShowGameProgressNotiMessage(EGameProgressNotificationMessageType MsgType);


	void SaveWorldStatus(class ANiceCar* CarRef);
	void LoadWorldStatus();



	/* Damage fomula and kill event */
	float ModifyApplyDamage(float BeforeDamage, struct FDamageEvent const& DamageEvent, class AActor* DamagedVictim, class AController* EventInstigator, AActor* DamageCauser);

	void ProcessKillEvent(AController* Killer, AController* KilledChar, struct FDamageEvent const& DamageEvent);
	void ProcessHitEvent(AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent);



	/* Mission */
	bool EvaluateMissionStatus(AActor* Object = nullptr);

	void UpdateMissionToThis(FString UpdateMissionUniqID);

	/* =Caution!!= Overview Widget에서 처음 한번만 강제로 업데이트 하기 위해 사용됨. 그 외 사용 금지.*/
	void ForcedMissionUpdate();

	/**
	 * 보통 mission이 update되면 SetCurrentMission 함수 내부에서 Broadcast로 자동 업데이트 되지만
	 * Current mission을 얻어와야할때가 있다.
	 */
	FInProgressMissionInfo GetCurrentMission();



	void StartCQBMission();
	void OnlyCQBMission(int32 PerfectCnt, int32 TotalScore);
	void EndCQBMission();

	

};
