// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AttackMoving.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTService_AttackMoving : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_AttackMoving();

	float Angle = 0.f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
