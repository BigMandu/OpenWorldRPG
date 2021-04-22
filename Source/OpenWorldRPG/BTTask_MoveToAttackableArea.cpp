// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAttackableArea.h"



UBTTask_MoveToAttackableArea::UBTTask_MoveToAttackableArea()
{
	NodeName = TEXT("MoveTo Attackable Area");
}
EBTNodeResult::Type UBTTask_MoveToAttackableArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
		
	return Result;
}