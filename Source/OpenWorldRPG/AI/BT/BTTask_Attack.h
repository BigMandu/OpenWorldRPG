// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
private:
	class UBlackboardComponent* BBComp;
	class AEnemyAIController* AICon;
	class AEnemyCharacter* AIChar;

protected:
	UBTTask_Attack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

private:
	
	void CheckWeapon();
	void FiringWeapon();

};
