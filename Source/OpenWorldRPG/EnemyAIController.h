// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	class AEnemyCharacter* Enemy;

	/*********** AI Perception ***************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UAISenseConfig_Hearing* HearingConfig;

	FTimerHandle TargetLostTimer;
	FTimerDelegate TargetLostDelegate;

	/************* Behavior Tree *************/
	UPROPERTY()
	class UBehaviorTreeComponent* BTComp;
	UPROPERTY()
	class UBlackboardComponent* BBComp;

	/**********  Blackboard Key   **********/

	const FName OriginPosKey = FName(TEXT("OriginPos"));
	const FName PatrolPosKey = FName(TEXT("PatrolPos"));
	const FName bHasTargetPointsKey = FName(TEXT("HasTargetPoints"));
	const FName TargetPointIndexKey = FName(TEXT("TargetPointIndex"));
	const FName PlayerKey = FName(TEXT("Player"));
	const FName bSeePlayerKey = FName(TEXT("SeePlayer"));
	const FName bHearPlayerKey = FName(TEXT("HearPlayer"));


protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void DetectedTarget(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void LostTarget(AActor* Target);

	virtual void OnPossess(APawn* InPawn) override;

	void UpdateTargetPointIndex(int32 index);
	void UpdatePatrolPosKey(FVector NewPatrolPos);

	
};
