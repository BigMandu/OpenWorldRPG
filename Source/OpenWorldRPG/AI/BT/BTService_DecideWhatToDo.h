// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DecideWhatToDo.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTService_DecideWhatToDo : public UBTService
{
	GENERATED_BODY()
private:
	/* Decision Value */
	bool bNoWeapon = false;
	bool bLowHP = false;
	bool bLowAmmo = false;
	bool bNoAmmo = false;
	bool bWasEngage = false;
	bool bInEnemyFOV = false;
	//bool bOnlyDetectHearing = false;

	class UBlackboardComponent* BBComp;
	class AEnemyAIController* AICon;
	class AEnemyCharacter* AIChar;
public:
	UBTService_DecideWhatToDo();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void SetDecisionValue();//UBehaviorTreeComponent& OwnerComp);
	void GetDecisionValue();
	void DecisionBranch();//UBehaviorTreeComponent& OwnerComp);


	bool IsThisAIinTargetFOV();//UBlackboardComponent* BBComp, AEnemyAIController* OwnerAICon, AEnemyCharacter* OwnerAI);
	bool HasOtherWeapon();//AEnemyCharacter* OwnerAI);


	void AIOpenFire();
	void AIItemFarming();
};
