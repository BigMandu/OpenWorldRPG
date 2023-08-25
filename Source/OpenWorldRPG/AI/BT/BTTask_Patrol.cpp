// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Patrol::UBTTask_Patrol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Patrol");
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if(!AICon || !Enemy || !BBComp) return EBTNodeResult::Failed;

	Enemy->SetAIStatus(EAIStatus::EAS_Patrol);

	//Patrol인데, Hearing Something이면 AimMode로 세팅해준다.
	/*if ( BBComp->GetValueAsBool(AICon->bHearEnemyKey) )
	{
		Enemy->SetAimMode(EAimMode::EAM_Aim);
	}*/
	
	//AcceptableRadius = Enemy->PatrolAcceptableRadius;
	return Result;
}