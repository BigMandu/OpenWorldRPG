// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTTask_InteractWithObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
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
			//if (Inter->bIsPreOccupied == false)
			{
				//Inter->bIsPreOccupied = true;
				Inter->Interaction(EChar);

				//Interaction�� �ϰ��� Focus�� Perception Ignore�� �߰��ϱ� ���� �Լ��� ȣ��
				AICon->LostObject(Inter);
			}
		}
		if (BChar)
		{
			BChar->Interaction(EChar);
			AICon->LostObject(BChar);
		}
	}

	return Result;
}
