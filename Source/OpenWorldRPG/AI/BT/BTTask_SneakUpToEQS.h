// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "BTTask_SneakUpToEQS.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_SneakUpToEQS : public UBTTask_RunEQSQuery
{
	GENERATED_BODY()

	UBTTask_SneakUpToEQS(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
