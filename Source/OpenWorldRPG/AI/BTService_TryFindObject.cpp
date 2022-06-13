// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BTService_TryFindObject.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/EnemyAIController.h"


UBTService_TryFindObject::UBTService_TryFindObject()
{
	NodeName = TEXT("TryFindObj");

}

void UBTService_TryFindObject::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	Time += DeltaSeconds;
	float AlphaTime = Time / FindTime;

	//UE_LOG(LogTemp, Warning, TEXT("Time : %f / AlphaTime : %f"), Time, AlphaTime);
	if(AlphaTime >= 1.0f || BBComp->GetValueAsBool(AICon->bOutOfAmmoKey))
	{
		//Time = 0.f;
		BBComp->SetValueAsBool(AICon->bTryFindObejct, true);
	}

}
