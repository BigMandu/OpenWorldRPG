// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/EnemyCharacter.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	check(Enemy);

	//Patrol��尡 ������϶��� �ִ�ӵ��� �ȴ� �ӵ��� �����Ѵ�.
	Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MinWalkSpeed;
	AcceptableRadius = Enemy->PatrolAcceptableRadius;
	return Result;
}