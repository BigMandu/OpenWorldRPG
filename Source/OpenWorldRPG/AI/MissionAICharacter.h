// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "MissionAICharacter.generated.h"

/**
 *
 */

 USTRUCT()
struct FBeforePlayedAudioInfo
{
	GENERATED_USTRUCT_BODY()
	FBeforePlayedAudioInfo()
	{
		playedMissionUniqID = TEXT("none");
		playedNumber = 0;
		MAXPlayNumber = 0;
	}
	FBeforePlayedAudioInfo(FString T_UniqID, int32 T_number, int32 T_MAXNumber)
	{
		playedMissionUniqID = T_UniqID;
		playedNumber = T_number;
		MAXPlayNumber = T_MAXNumber;
	}

	FString playedMissionUniqID;
	int32 playedNumber;
	int32 MAXPlayNumber;
 };

UCLASS()
class OPENWORLDRPG_API AMissionAICharacter : public AEnemyCharacter, public IMissionInterface
{
	GENERATED_BODY()

	/*UPROPERTY()
	FCharacterVoiceTable VoiceTableRow;*/

	FBeforePlayedAudioInfo BeforePlayedAudioInfo;


	bool bIsShowing;
	FVector ThisActorsLocation;
	class AMainController* PlayerCon;
	
public:
	AMissionAICharacter();
	UPROPERTY(EditAnywhere, Category = Mission)
	FString ThisCharacterUniqID;
	UPROPERTY(EditAnywhere, Category = Mission)
	FText Name;
	UPROPERTY(EditAnywhere, Category = Mission)
	UDataTable* CharacterVoiceTable;

	UPROPERTY(EditAnywhere, Category = Mission)
	bool bAlsoGiveMission;
	UPROPERTY(EditAnywhere, Category = Mission)
	FString GiveMissionUniqueID;

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* NotifyIcon;

	UPROPERTY(EditAnywhere)
	FVector2D WidgetSize = FVector2D(100.f);

private:
	//void SetAudioInfo();
public:

	virtual void PostInitializeComponents() override;
	virtual void Interaction(AActor* Actor) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartDialogue();

	void StartAct(UAnimMontage* act);

	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();

};
