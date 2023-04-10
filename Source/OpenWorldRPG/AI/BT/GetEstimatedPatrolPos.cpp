// Fill out your copyright notice in the Description page of Project Settings.


#include "GetEstimatedPatrolPos.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "GetEstimatedPatrolPos.h"

UGetEstimatedPatrolPos::UGetEstimatedPatrolPos()
{
	NodeName = TEXT("GetEstimatedPatrolPos");
}

EBTNodeResult::Type UGetEstimatedPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);
	EBTNodeResult::Type Result = EBTNodeResult::Failed;


	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if ( BBComp && AIController )
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetCharacter());
		if ( Enemy ==nullptr ) return Result;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(Enemy->GetWorld());
		if(NavSys == nullptr ) return Result;

		const FVector InterestOriginPos = BBComp->GetValueAsVector(AIController->EstimatedLocationOfTargetKey);
		FNavLocation PatrolPos;
		if ( NavSys->GetRandomReachablePointInRadius(InterestOriginPos, 100.f, PatrolPos) )
		{
			BBComp->SetValueAsVector(AIController->TargetLocationKey, PatrolPos.Location);
			Result = EBTNodeResult::Succeeded;
		}


	}
	return Result;
}
