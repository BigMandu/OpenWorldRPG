// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "MissionDataTableRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMissionDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

#if WITH_EDITOR
	//virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif


	/**
	 * Mission의 고유 id 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	FString MissionUniqueID;
	
	//if this mission is a 'Choice' type, Choice1 and Choice2 must be set.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	EMissionType Missiontype;

	//if this mission has prerequisite mission have to check true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	bool bHasPrerequisiteMission;

	//if bHasPrerequisiteMission is true, have to fill this Value Prerequisite Mission's unique id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	FString PrerequisiteUniqueID;

	//Mission의 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	FText MissionName;

	//Mission의 Description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mission)
	FText MissionDescription;

	/** Reach Type인 경우엔 MissionVolume에 꼭 해당 mission unique id를 적어주자.
	 * 
	 * if MissionObjective type is MissionAICharacter,  Must set CharacterUniqueID.
	 * And if this MissionType is a Choice mission, Must set NONETYPE.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MissionAction)
	EMissionActionType MissionActionType;

	/** AICharacter의 고유 ID
	 * if MissionObjective is AICharacterType, input Character's Uniq ID not mission id.
	 */
	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MissionAction)
	FString CharacterUniqueID;

	//Mission의 Item 필요 개수 (clear하기 위한)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MissionAction)
	int32 RequiredObjectCount;

	//if this value is true, show HintWidget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MissionAction)
	bool bCanShowHint;

	//미션의 목표 액터. MissionActionType인 경우엔 꼭 지정을 해줘야한다.
	/* 특정 Actor 종류를 습득하는 경우 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MissionAction)
	TSubclassOf<AActor>MissionObjective;


	/* Choice 미션은 단순한 미션만 가능하다.
	* Reach나 Interact같은 단순 이동목적의 미션만 세팅 하도록 하자.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice1)
	FText Choice1Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice1)
	FText Choice1Description;

	/**
	 * Mission의 고유 id.
	 * 해당 미션 ID로 mission이 update된다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice1)
	FString Choice1MissionUniqID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice1)
	TSubclassOf<AActor>Choice1Objective;

	/** AICharacter의 고유 ID
	* if MissionObjective is AICharacterType, input Character's Uniq ID not mission id.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice1)
	FString Character1UniqueID;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice2)
	FText Choice2Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice2)
	FText Choice2Description;

	/**
	 * Mission의 고유 id.
	 * 해당 미션 ID로 mission이 update된다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice2)
	FString Choice2MissionUniqID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice2)
	TSubclassOf<AActor>Choice2Objective;

	/** AICharacter의 고유 ID
	* if MissionObjective is AICharacterType, input Character's Uniq ID not mission id.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Choice2)
	FString Character2UniqueID;

	
};