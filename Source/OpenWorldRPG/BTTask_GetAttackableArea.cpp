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

	FVector RotationVec = (PlayerLo - AILo).GetSafeNormal(); //플레이어의 방향으로
	float Distance = (FVector::Dist(PlayerLo, AILo) - (Enemy->Range - Enemy->Range*0.1)); //Player와 Enemy+Range의 거리를 구한다. -> 너무 타이트해서 살짝 더 나가게 해줬다.
	FVector AIWithRange = (RotationVec * Distance) + AILo; //Range만큼 나아간다. ->Range만큼만 나가면 안되고, Player의 위치와  AI의 위치의 차가 Range보다 얼만큼 클때 ㅇㅇ

	FVector NewAILocation = FVector(AIWithRange.X, AIWithRange.Y, PlayerLo.Z); //Player의 높이로 해준다. 안그럼 못올라감.
	
	
	//for debug
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackableArea : %s"), *NewAILocation.ToString());
		//UKismetSystemLibrary::DrawDebugSphere(this, AttackableLocation, 50.f, 12, FLinearColor::Blue, 3.f, 2.f);
		UKismetSystemLibrary::DrawDebugLine(this, AILo, NewAILocation, FLinearColor::Yellow, 2.f, 2.f);
	}
	AICon->UpdateAttackableLocationKey(NewAILocation);
	return Result;
}