// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class ABaseCharacter;
class AEnemyCharacter;
class ALootBox;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class OPENWORLDRPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AEnemyCharacter* OwnerActor;

	/*********** AI Perception ***************/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	UAIPerceptionComponent* PerceptionComp;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Hearing* HearingConfig;

	FTimerHandle TargetLostTimer;
	FTimerDelegate TargetLostDelegate;



	float Alpha;
	float AttackMVDist;
	bool bUpdateEnemyLo;
	FVector EnemyAttackLo;

	/************* Behavior Tree *************/
	UPROPERTY()
	UBehaviorTreeComponent* BTComp;
	UPROPERTY()
	UBlackboardComponent* BBComp;

	/**********  Blackboard Key   **********/
	//Object
	const FName EnemyKey = FName("Enemy");
	const FName TargetKey = FName("Target");

	//FVector
	const FName OriginPosKey = FName("OriginPos");
	const FName PatrolPointIndexKey = FName("PatrolPointIndex");
	const FName TargetLocationKey = FName("TargetLocation");
	//const FName PatrolPosKey = FName("PatrolPos");
	//const FName HearLocation = FName("HearLocation");
	//const FName AttackableLocationKey = FName("AttackableLocation");
	//const FName LootingLocationKey = FName("LootingLocation");

	//Boolean
	const FName bSeeEnemyKey = FName("SeeEnemy");
	const FName bHearEnemyKey = FName("HearEnemy");
	const FName bCanAttackKey = FName("CanAttack");
	const FName bHasPatrolPointsKey = FName("HasPatrolPoints");
	const FName bOutOfAmmoKey = FName("OutOfAmmo");
	const FName bTryFindObejct = FName("TryFindObject");


	/*********************************************/



protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;


	bool CheckIsEnemy(ABaseCharacter* Target);

	UFUNCTION()
	void DetectedTarget(AActor* Target, FAIStimulus Stimulus);

	void DetectedCharacter(ABaseCharacter* Player, FAIStimulus Stimulus);
	void DetectedLootBox(ALootBox* Loot, FAIStimulus Stimulus);

	UFUNCTION()
	void LostTarget(ABaseCharacter* Target); //AActor* Target);


	void CalcAttackDist(float DeltaTime);
	void AttackMoving(const FVector Vec, FVector RightVec);

	/**********   Update BlackBoard Key func  ***********/
	//BT Task에서 사용하기 위한 함수인데 여기에서도 씀.
	void UpdateBBCompVectorKey(FName KeyName, FVector Vector);
	void UpdateBBCompBoolKey(FName KeyName, bool bBool);
	void UpdateBBCompIntegerKey(FName KeyName, int32 Numb);
	void UpdateBBCompObjectKey(FName KeyName, AActor* Actor);


	/*
	void UpdatePatrolPointIndex(int32 index);
	void UpdatePatrolPosKey(FVector NewPatrolPos);
	void UpdatePlayerKey(AActor* Actor);
	void UpdateHearLocationKey(FVector Location);
	void UpdateSeePlayerKey(bool HasSee);
	void UpdateHearPlayerKey(bool HasHear);
	void UpdateAttackableLocationKey(FVector Location);
	*/
	
	
};
