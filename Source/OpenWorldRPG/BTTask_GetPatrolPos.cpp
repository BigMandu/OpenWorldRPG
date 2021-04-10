// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetPatrolPos.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
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
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		
		if (NavSys && Enemy)
		{
			const FVector OriginPos = BBComp->GetValueAsVector(AIController->OriginPosKey);
			FVector NewPatrolPos;
			FVector CurrentEnemyPos;
			NewPatrolPos = NavSys->GetRandomReachablePointInRadius(GetWorld(), OriginPos, 500.f); // Radius�� ������ ���� 500.f�� �����. ���� Enemy�� ���� �߰� ����.
			CurrentEnemyPos = Enemy->GetActorLocation();


			while (abs(CurrentEnemyPos.Size() - NewPatrolPos.Size()) >= 150.f) //������ġ�� ���α��� ������ġ�� 150�̻� ���̳���
			{
				BBComp->SetValueAsVector(AIController->PatrolPosKey, NewPatrolPos);

				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}