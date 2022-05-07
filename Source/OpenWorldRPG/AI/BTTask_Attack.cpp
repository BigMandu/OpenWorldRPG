// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	UE_LOG(LogTemp, Warning, TEXT("Attack!"));

	return Result;
}