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

		const FVector InterestOriginPos = BBComp->GetValueAsVector(	AIController->EstimatedLocationOfTargetKey);
		FNavLocation PatrolPos;
		float sub = 0.f;
		int32 loopcnt = 0;
		while ( sub <= 120.f )
		{
			if(loopcnt >= 10 ) break;
			if ( NavSys->GetRandomReachablePointInRadius(InterestOriginPos, 200.f, PatrolPos) )
			{
				FVector FinalPatrolPos = PatrolPos.Location;
				BBComp->SetValueAsVector(AIController->TargetLocationKey, FinalPatrolPos);

				sub = abs(InterestOriginPos.Size() - FinalPatrolPos.Size());
				
			}
		}

		if ( loopcnt >= 10 || sub >= 80.f )
		{
			Result = EBTNodeResult::Succeeded;
		}

	}
	return Result;
}
