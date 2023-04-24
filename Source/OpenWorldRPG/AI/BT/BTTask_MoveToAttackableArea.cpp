// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAttackableArea.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"


UBTTask_MoveToAttackableArea::UBTTask_MoveToAttackableArea()
{
	NodeName = TEXT("MoveTo Attackable Area");
}
EBTNodeResult::Type UBTTask_MoveToAttackableArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	bUsePathfinding = true;

	
	return Result;
}

void UBTTask_MoveToAttackableArea::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp,NodeMemory,TaskResult);

	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AICon->GetPawn());
	check(AIChar);

	//도착한 뒤에 사격할 수 있도록 한다.
	AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey,true);
	AIChar->SetAIStatus(EAIStatus::EAS_Attack);
}
