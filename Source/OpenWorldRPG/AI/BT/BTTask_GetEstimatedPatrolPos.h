// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetEstimatedPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_GetEstimatedPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	public:
		UBTTask_GetEstimatedPatrolPos();
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
