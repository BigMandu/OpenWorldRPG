// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AimLocation.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTTask_AimLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_AimLocation();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
