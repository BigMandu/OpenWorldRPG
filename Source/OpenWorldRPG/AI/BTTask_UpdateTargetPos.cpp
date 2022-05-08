// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UpdateTargetPos.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/EnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_UpdateTargetPos::UBTTask_UpdateTargetPos()
{
	NodeName = (TEXT("UpdateTargetPos"));
}

EBTNodeResult::Type UBTTask_UpdateTargetPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AIController);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetCharacter());
	check(Enemy);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	check(BBComp);

	int32 TPindex = BBComp->GetValueAsInt(AIController->TargetPointIndexKey); //TargetPointIndex를 가져온다. (기본세팅 0)
	int32 TPNum = Enemy->TargetPoints.Num(); //Targetpoint Array의 개수를 가져온다.

	ATargetPoint* TP = Enemy->TargetPoints[TPindex]; //index번째의 TargetPoint를 가져온다.
	FVector NewLocation = TP->GetActorLocation();
	AIController->UpdatePatrolPosKey(NewLocation); //가져온TP의 Location을 PatrolPos에 업데이트 한다.


	//업데이트 한 뒤, TPindex를 +1 시켜준다.
	if (TPindex + 1 >= TPNum)
	{
		AIController->UpdateTargetPointIndex(0);
	}
	else
	{
		AIController->UpdateTargetPointIndex(TPindex+1);
	}
	
	Result = EBTNodeResult::Succeeded;
	return Result;
}