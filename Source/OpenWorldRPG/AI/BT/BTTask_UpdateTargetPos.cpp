// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UpdateTargetPos.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"

UBTTask_UpdateTargetPos::UBTTask_UpdateTargetPos()
{
	NodeName = (TEXT("UpdatePatrolPos"));
}

//������ Point�� ������  Enemy�� ����� Patrol Point�� ��ġ�� ���´�.
EBTNodeResult::Type UBTTask_UpdateTargetPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AIController);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetCharacter());
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	check(Enemy);
	check(BBComp);

	int32 TPindex = BBComp->GetValueAsInt(AIController->PatrolPointIndexKey); //TargetPointIndex�� �����´�. (�⺻���� 0)
	int32 TPNum = Enemy->TargetPoints.Num(); //Targetpoint Array�� ������ �����´�.

	ATargetPoint* TP = Enemy->TargetPoints[TPindex]; //index��°�� TargetPoint�� �����´�.
	FVector NewLocation = TP->GetActorLocation();

	AIController->UpdateBBCompVectorKey(AIController->TargetLocationKey, NewLocation);
	//AIController->UpdatePatrolPosKey(NewLocation); //������TP�� Location�� PatrolPos�� ������Ʈ �Ѵ�.


	//������Ʈ �� ��, TPindex�� +1 �����ش�.
	if (TPindex + 1 >= TPNum)
	{
		AIController->UpdateBBCompIntegerKey(AIController->PatrolPointIndexKey, 0);
		//AIController->UpdatePatrolPointIndex(0);
	}
	else
	{
		AIController->UpdateBBCompIntegerKey(AIController->PatrolPointIndexKey, TPindex+1);
		//AIController->UpdatePatrolPointIndex(TPindex+1);
	}
	
	Result = EBTNodeResult::Succeeded;
	return Result;
}