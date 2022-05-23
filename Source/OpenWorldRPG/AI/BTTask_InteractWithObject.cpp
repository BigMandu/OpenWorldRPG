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
		if(Inter)
		{
			Inter->Interaction(EChar);

			/*switch(Inter->InteractType)
			{
			case EInteractType::EIT_Equipment:
				
				break;
			case EInteractType::EIT_Item:
				break;
			case EInteractType::EIT_LootBox:
				break;

			}*/
		}
	}

	return Result;
}
