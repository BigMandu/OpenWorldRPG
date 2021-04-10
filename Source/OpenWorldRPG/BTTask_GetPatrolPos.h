// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_GetPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GetPatrolPos();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
