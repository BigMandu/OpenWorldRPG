// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SneakupOnTarget.h"
#include "BTTask_SneakupOnTarget.h"
#include <OpenWorldRPG/AI/EnemyAIController.h>
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include <OpenWorldRPG/MainCharacter.h>
#include "EnvironmentQuery/EnvQueryManager.h"

EBTNodeResult::Type UBTTask_SneakupOnTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type ReturnValue = EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(BBComp);
	check(AICon);

	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());
	AMainCharacter* Player = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	check(AIChar);
	check(Player);

	FVector AICurrentLocation = AIChar->GetActorLocation();
	FVector TargetCurrentLocation = Player->GetActorLocation();

	FVector AIToTargetRotVec = (TargetCurrentLocation-AICurrentLocation).GetSafeNormal();

	FindSneakUpLocation(AICon, AICurrentLocation,AIToTargetRotVec);

	 return ReturnValue;
}

FVector UBTTask_SneakupOnTarget::FindSneakUpLocation(AEnemyAIController* AICon, const FVector& AILocation, const FVector& AIToTargetUnitVec)
{
	 FEnvQueryRequest EnvQryReq = FEnvQueryRequest(AICon->SneakUpOnTargetEQS, AICon->GetCharacter());
	 EnvQryReq.Execute(EEnvQueryRunMode::SingleResult, AICon, &AEnemyAIController::HandleEQSResult);
	 //EnvQryReq.Execute(EEnvQueryRunMode::SingleResult, this);
	 
	 return FVector(0.f);
}