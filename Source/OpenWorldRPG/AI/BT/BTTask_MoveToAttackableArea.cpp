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

	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	check(Enemy);

	Enemy->SetAIStatus(EAIStatus::EAS_Attack);
	return Result;
}