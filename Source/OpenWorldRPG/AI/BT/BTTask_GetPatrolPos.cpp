// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetPatrolPos.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"

UBTTask_GetPatrolPos::UBTTask_GetPatrolPos()
{
	NodeName = TEXT("GetPatrolPos");
}

//Random Patrol�� �Ҷ� �ּ� �Ÿ� �̻��� Patrol Point�� ���´�.
EBTNodeResult::Type UBTTask_GetPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	
	if (BBComp && AIController)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetCharacter());
		if (Enemy)
		{
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(Enemy->GetWorld());
			if (NavSys)
			{
				const FVector OriginPos = BBComp->GetValueAsVector(AIController->OriginPosKey);
				FVector CurrentEnemyPos = Enemy->GetActorLocation();

				FVector NewPatrolPos;
				FNavLocation NavLocation;
				float sub = 0.f;
				int32 Count = 0;
				while ( sub <= 500.f) //������ġ�� ���α��� ������ġ�� x�̻� ���̳�������
				{
					//10�� �̻� �õ��ص� ��ã���� �����Ѵ�.
					if(Count >= 10) break;
		
					bool bNavResult = NavSys->GetRandomReachablePointInRadius(OriginPos, Enemy->RandomPatrolRadius, NavLocation);
					if (bNavResult)
					{
						NewPatrolPos = NavLocation.Location;
						sub = abs(CurrentEnemyPos.Size() - NewPatrolPos.Size());
						++Count;
					}
				}
				BBComp->SetValueAsVector(AIController->TargetLocationKey, NewPatrolPos);
				//AIController->UpdatePatrolPosKey(NewPatrolPos);

				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}