// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetAttackableArea.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "MainCharacter.h"
#include "Math/Vector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"

UBTTask_GetAttackableArea::UBTTask_GetAttackableArea()
{
	NodeName = TEXT("GetAttackableArea");
}


EBTNodeResult::Type UBTTask_GetAttackableArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	check(BBComp);
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	check(Enemy);

	AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->PlayerKey));
	check(Main);

	BBComp->ClearValue(AICon->AttackableLocationKey);

	FVector PlayerLo = Main->GetActorLocation();
	FVector AILo = Enemy->GetActorLocation();

	FVector DistanceVec = (PlayerLo - AILo).GetSafeNormal();
	FVector AIWRange = (DistanceVec * Enemy->Range) + AILo;

	FVector NewAILo = FVector(AIWRange.X, AIWRange.Y, AILo.Z);
	
	
	//for debug
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackableArea : %s"), *NewAILo.ToString());
		//UKismetSystemLibrary::DrawDebugSphere(this, AttackableLocation, 50.f, 12, FLinearColor::Blue, 3.f, 2.f);
		UKismetSystemLibrary::DrawDebugLine(this, AILo, NewAILo, FLinearColor::Yellow, 2.f, 2.f);
	}
	AICon->UpdateAttackableLocationKey(NewAILo);
	return Result;
}