// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BTTask_InteractWithObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/EnemyCharacter.h"
#include "OpenWorldRPG/Item/Interactable.h"


UBTTask_InteractWithObject::UBTTask_InteractWithObject()
{

}

EBTNodeResult::Type UBTTask_InteractWithObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* EChar = Cast<AEnemyCharacter>(AICon->GetCharacter());

	if(BBComp && EChar)
	{
		AInteractable* Inter = Cast<AInteractable>(BBComp->GetValueAsObject(AICon->ObjectKey));
		ABaseCharacter* BChar = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AICon->ObjectKey));
		if(Inter)
		{
			Inter->Interaction(EChar);

			//Interaction을 하고나서 Focus와 Perception Ignore에 추가하기 위해 함수를 호출
			AICon->LostObject(Inter);
		}
		if (BChar)
		{
			BChar->Interaction(EChar);
			AICon->LostObject(BChar);
		}
	}

	return Result;
}
