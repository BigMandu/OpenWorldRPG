// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BTTask_MoveToTarget.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/EnemyCharacter.h"

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
	NodeName = TEXT("MoveToTarget");

	//완전 겹치지 않아도 Interact가능하도록 Acceptable Radius를 Editor에서 설정한다.
	//AcceptableRadius = 120.f;
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Return = Super::ExecuteTask(OwnerComp, NodeMemory);

	

	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	check(Enemy);
	
	Enemy->SetAIStatus(EAIStatus::EAS_Normal);

	return Return;
}
