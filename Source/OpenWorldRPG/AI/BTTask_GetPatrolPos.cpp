// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetPatrolPos.h"
#include "OpenWorldRPG/EnemyCharacter.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetPatrolPos::UBTTask_GetPatrolPos()
{
	NodeName = TEXT("GetPatrolPos");
}

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

				while ( sub <= 500.f) //������ġ�� ���α��� ������ġ�� x�̻� ���̳�������
				{
					//NewPatrolPos = NavSys->GetRandomReachablePointInRadius(Enemy->GetWorld(), OriginPos, 1000.f); // Radius�� ������ ���� 500.f�� �����. ���� Enemy�� ���� �߰� ����.
					bool bNavResult = NavSys->GetRandomReachablePointInRadius(OriginPos, Enemy->RandomPatrolRadius, NavLocation); //���� FVector�����ϴ� �Լ��� �𸮾󿡼� �����Ѵٰ� ��.
					if (bNavResult)
					{
						NewPatrolPos = NavLocation.Location;
						sub = abs(CurrentEnemyPos.Size() - NewPatrolPos.Size());
					}
				}
				BBComp->SetValueAsVector(AIController->PatrolPosKey, NewPatrolPos);
				//AIController->UpdatePatrolPosKey(NewPatrolPos);

				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}