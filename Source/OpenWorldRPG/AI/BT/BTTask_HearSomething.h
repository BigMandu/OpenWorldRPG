// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HearSomething.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_HearSomething : public UBTTaskNode
{
	GENERATED_BODY()
private:
	class UBlackboardComponent* BBComp;
	class AEnemyAIController* AICon;
	class AEnemyCharacter* AIChar;

protected:
	UBTTask_HearSomething();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

private:
	void CheckWeapon();

	bool CornerRecogTraceIsHit(const FVector& StartLocation, const FVector& EndLocation, const UWorld& world);
	bool CheckHaveToAimAndWait();
	void GetWhereToAim();
	
};
