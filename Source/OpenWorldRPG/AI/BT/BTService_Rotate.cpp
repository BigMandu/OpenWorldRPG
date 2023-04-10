// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Rotate.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTService_Rotate.h"

UBTService_Rotate::UBTService_Rotate()
{
	NodeName = TEXT("Rotate");
	Interval = 0.0001;

	//Distance = 100.f;
	Alpha = 0.f;
}

void UBTService_Rotate::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(BBComp);
	check(AICon);
	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());

	//코너 탐지후 회전
	UPathFollowingComponent* PathFollowComp = AICon->GetPathFollowingComponent();
	if(PathFollowComp == nullptr ) return;
	TArray<FNavPathPoint> PathPoint = PathFollowComp->Getpath

}
