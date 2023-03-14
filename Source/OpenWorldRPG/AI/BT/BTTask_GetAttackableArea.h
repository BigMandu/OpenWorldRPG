// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
//#include "EnvironmentQuery/EnvQueryManager.h"
#include "BTTask_GetAttackableArea.generated.h"

/**
 * 
 */
class UEnvQuery;
struct FEnvQueryRequest;

UCLASS()
class OPENWORLDRPG_API UBTTask_GetAttackableArea : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GetAttackableArea();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/*UEnvQuery* Query;
	FEnvQueryRequest QueryReq;
	void QueryFinished(TSharedPtr<FEnvQueryRequest>* Result);*/
};
