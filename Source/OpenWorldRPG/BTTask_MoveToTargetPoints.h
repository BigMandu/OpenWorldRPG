// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToTargetPoints.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_MoveToTargetPoints : public UBTTaskNode
{
	GENERATED_BODY()
public:
	FTimerHandle WaitingTime;
	UBTTask_MoveToTargetPoints();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	
};
