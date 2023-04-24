// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SneakUpToEQS.h"
#include <OpenWorldRPG/AI/EnemyAIController.h>
#include <OpenWorldRPG/AI/EnemyCharacter.h>

UBTTask_SneakUpToEQS::UBTTask_SneakUpToEQS(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "SneakUpToEQS";
}

EBTNodeResult::Type UBTTask_SneakUpToEQS::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);
	EBTNodeResult::Type ReturnResult = EBTNodeResult::Failed;
	AEnemyAIController* AIcon = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if(!AIcon ) return ReturnResult;

	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AIcon->GetPawn());
	if (!AIChar) return ReturnResult;

	AIChar->SetAimMode(EAimMode::EAM_Aim);


	ReturnResult = EBTNodeResult::InProgress;
	return ReturnResult;
}
