// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TryFindObject.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTService_TryFindObject : public UBTService
{
	GENERATED_BODY()
private:
	float Time = 0.f;
	float FindTime = 10.f;
public:
	UBTService_TryFindObject();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
