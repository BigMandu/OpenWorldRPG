// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAttackableArea.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_MoveToAttackableArea : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UBTTask_MoveToAttackableArea();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
