// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToTargetPoints.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Engine/TargetPoint.h"

UBTTask_MoveToTargetPoints::UBTTask_MoveToTargetPoints()
{
	NodeName = (TEXT("MoveToTargetPoints"));
}

void UBTTask_MoveToTargetPoints::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	check(Enemy);

}

EBTNodeResult::Type UBTTask_MoveToTargetPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AIController);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetCharacter());
	check(Enemy);

	ATargetPoint* TP;

	bool bHas = Enemy->bHasPatrolPoints; //µð¹ö±ë¿ë
	auto iter = Enemy->TargetPoints.Num(); //µð¹ö±ë¿ë

	for (int32 i  = 0; i < iter; i++)
	{
		TP = Enemy->TargetPoints[i];
		if (TP)
		{
			Result = EBTNodeResult::InProgress;

			FVector PatrolPoint = TP->GetActorLocation();
			EPathFollowingRequestResult::Type MoveToResult;

			MoveToResult = AIController->MoveToLocation(PatrolPoint);
			if (MoveToResult == EPathFollowingRequestResult::RequestSuccessful)
			{
				UE_LOG(LogTemp, Warning, TEXT("RequestSuccessful, waiting AlreadyAtgoal"));
				while (MoveToResult == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					UE_LOG(LogTemp, Warning, TEXT("Moving"));
				}

			}

			if (MoveToResult == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				UE_LOG(LogTemp, Warning, TEXT("AlreadyAtGoal, waiting for sec"));
				GetWorld()->GetTimerManager().SetTimer(WaitingTime, 3.f, false);
			}
		}
		if (i == iter)
		{
			Result = EBTNodeResult::Succeeded;
		}
	}
	
	return Result;
}