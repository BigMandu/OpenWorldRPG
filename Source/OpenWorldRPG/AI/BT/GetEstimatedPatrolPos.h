// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GetEstimatedPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UGetEstimatedPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	public:
		UGetEstimatedPatrolPos();
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
