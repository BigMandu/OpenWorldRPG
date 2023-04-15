// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SneakupOnTarget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_SneakupOnTarget : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FVector FindSneakUpLocation(AEnemyAIController* AICon, const FVector& AILocation, const FVector& AIToTargetUnitVec);
};
