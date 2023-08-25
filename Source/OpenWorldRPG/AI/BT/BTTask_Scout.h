// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_Scout.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_Scout : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UBTTask_Scout(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
