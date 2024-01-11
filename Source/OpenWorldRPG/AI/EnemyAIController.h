// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
//#include "DetourCrowdAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class ABaseCharacter;
class AEnemyCharacter;
class AInteractable;

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UNewInventoryComponent;

UCLASS()
class OPENWORLDRPG_API AEnemyAIController : public AAIController //public ADetourCrowdAIController 
{
	GENERATED_BODY()

public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AEnemyCharacter* OwnerActor;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	class UBehaviorTree* BTAsset;

	/*********** AI Perception ***************/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	UAIPerceptionComponent* PerceptionComp;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Hearing* HearingConfig;

	FTimerHandle EnemyLostTimer;
	FTimerDelegate EnemyLostDelegate;

	/*FTimerHandle ObjectLostTimer;
	FTimerDelegate ObjectLostDelegate;*/



	float Alpha;
	float AttackMVDist;
	bool bUpdateEnemyLo;
	FVector EnemyAttackLo;

	/************* Behavior Tree *************/
	UPROPERTY()
	UBehaviorTreeComponent* BTComp;
	UPROPERTY()
	UBlackboardComponent* BBComp;

	UPROPERTY(EditAnywhere,Category = AI)
	class UEnvQuery* SneakUpOnTargetEQS;


	TArray<TWeakObjectPtr<class AGrenadeBase>> DetectedGrenadeList;

	/**********  Blackboard Key   **********/
	//Object
	const FName EnemyKey = FName("Enemy");
	const FName ObjectKey = FName("Object");
	const FName DetectedGrenadeKey = FName("DetectedGrenade");

	//FVector
	const FName PatrolPointIndexKey = FName("PatrolPointIndex");
	const FName OriginPosKey = FName("OriginPos");	
	const FName TargetLocationKey = FName("TargetLocation");
	const FName LastTargetLocationKey = FName("LastTargetLocation");
	const FName LastTargetRotationKey = FName("LastTargetRotation");
	const FName EstimatedLocationOfTargetKey = FName("EstimatedLocation");
	const FName DetectGrenadeLocationKey = FName("DetectGrenadeLocation");
	const FName AIMLocationKey = FName("AIMLocation");
	//const FName PatrolPosKey = FName("PatrolPos");
	//const FName HearLocation = FName("HearLocation");
	//const FName AttackableLocationKey = FName("AttackableLocation");
	//const FName LootingLocationKey = FName("LootingLocation");

	//Boolean
	const FName bDetectGrenade = FName("DetectGrenade");
	const FName bSeeEnemyKey = FName("SeeEnemy");
	const FName bHearEnemyKey = FName("HearEnemy");
	const FName bCanAttackKey = FName("CanAttack");
	const FName bHasPatrolPointsKey = FName("HasPatrolPoints");
	const FName bMovingBehindTargetKey = FName("MovingBehindTarget");
	
	//DecideBranch boolean
	const FName bOutOfAmmoKey = FName("OutOfAmmo");
	const FName bTryFindObejctKey = FName("TryFindObject");
	const FName bNeedToCheckKey = FName("NeedToCheck");
	const FName bNoWeaponKey = FName("NoWeapon");
	const FName bLowHPKey = FName("LowHP");
	const FName bLowAmmoKey = FName("LowAmmo");
	const FName bWasEngageKey = FName("WasEngage");
	const FName bInEnemyFOVKey = FName("InEnemyFOV");
	const FName bIsInsideKey = FName("IsInside");
	


	/*********************************************/



protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;

	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;

	bool IsGrenade(AActor* DetectActor);
	bool CheckIsEnemy(ABaseCharacter* Target);	
	bool CanInteraction(AActor* Object);


	void DetectedGrenade(AActor* DetectTarget, FAIStimulus Stimulus);
	UFUNCTION()
	void GrenadeEffectIsEnd(class AGrenadeBase* ExplodedGrenade);
	UFUNCTION()
	void DetectedTarget(AActor* Target, FAIStimulus Stimulus);

	void DetectedEnemy(ABaseCharacter* Player, FAIStimulus Stimulus);
	//void DetectedObject(AInteractable* Obj, FAIStimulus Stimulus);
	void DetectedObject(AActor* Obj, FAIStimulus  Stimulus);

	UFUNCTION()
	void LostTarget(ABaseCharacter* Target); //AActor* Target);

	//UFUNCTION()
	void LostObject(AActor* InteractActor);

	

	void ItemFarming(AActor* InteractActor);
	void ItemChoice(UNewInventoryComponent* GiverInvComp);

	void CalcAttackDist(float DeltaTime);
	void CalcGrenadeLocation();

	void AttackMoving(const FVector Vec, FVector RightVec);
	

	void HandleEQSResult(TSharedPtr<struct FEnvQueryResult> result);
	
	void DecideWhatToDoAfterDetected();

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
