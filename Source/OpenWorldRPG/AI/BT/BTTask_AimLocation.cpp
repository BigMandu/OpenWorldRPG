// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTTask_AimLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_AimLocation::UBTTask_AimLocation()
{
	bNotifyTick = false;
}

void UBTTask_AimLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_AimLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type ReturnResult = EBTNodeResult::Failed;

	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if (AICon && BBComp)
	{
		FVector AimLocation = BBComp->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());

		AICon->SetFocalPoint(AimLocation, EAIFocusPriority::Gameplay);
		ReturnResult = EBTNodeResult::Succeeded;
	}

	return ReturnResult;
}