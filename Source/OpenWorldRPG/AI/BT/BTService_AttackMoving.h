// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AttackMoving.generated.h"

/**
 * 
 */

/*
	이 서비스는 사용 하지 않는 서비스다.
*/
UCLASS()
class OPENWORLDRPG_API UBTService_AttackMoving : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_AttackMoving(); 

	float Distance;
	float Alpha;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
