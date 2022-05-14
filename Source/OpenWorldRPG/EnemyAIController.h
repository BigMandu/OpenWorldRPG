// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class AMainCharacter;
class AEnemyCharacter;
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

	AEnemyCharacter* Enemy;

	/*********** AI Perception ***************/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	UAIPerceptionComponent* PerceptionComp;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Hearing* HearingConfig;

	FTimerHandle TargetLostTimer;
	FTimerDelegate TargetLostDelegate;

	/************* Behavior Tree *************/
	UPROPERTY()
	UBehaviorTreeComponent* BTComp;
	UPROPERTY()
	UBlackboardComponent* BBComp;

	/**********  Blackboard Key   **********/

	const FName OriginPosKey = FName(TEXT("OriginPos"));
	const FName PatrolPosKey = FName(TEXT("PatrolPos"));
	const FName bHasPatrolPointsKey = FName(TEXT("HasPatrolPoints"));
	const FName PatrolPointIndexKey = FName(TEXT("PatrolPointIndex"));
	const FName PlayerKey = FName(TEXT("Player"));
	const FName HearLocation = FName(TEXT("HearLocation"));
	const FName bSeePlayerKey = FName(TEXT("SeePlayer"));
	const FName bHearPlayerKey = FName(TEXT("HearPlayer"));
	const FName AttackableLocationKey = FName(TEXT("AttackableLocation"));
	const FName CanAttackKey = FName(TEXT("CanAttack"));

	/*********************************************/

	float Alpha;
	float AttackMVDist;
	bool bUpdateEnemyLo;
	FVector EnemyAttackLo;


protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void DetectedTarget(AActor* Target, FAIStimulus Stimulus);

	void DetectedPlayer(AMainCharacter* Player, FAIStimulus Stimulus);


	UFUNCTION()
	void LostTarget(AMainCharacter* Target); //AActor* Target);

	virtual void OnPossess(APawn* InPawn) override;

	void UpdatePatrolPointIndex(int32 index);
	void UpdatePatrolPosKey(FVector NewPatrolPos);
	void UpdatePlayerKey(AActor* Actor);
	void UpdateHearLocationKey(FVector Location);
	void UpdateSeePlayerKey(bool HasSee);
	void UpdateHearPlayerKey(bool HasHear);
	void UpdateAttackableLocationKey(FVector Location);

	void AttackMoving(const FVector Vec, FVector RightVec);
	
};
