﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetAttackableArea.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Math/Vector.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "EnvironmentQuery/EnvQuery.h"
//#include "EnvironmentQuery/EnvQueryManager.h"

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

	ABaseCharacter* Char = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	check(Char);

	/*QueryReq = FEnvQueryRequest(Query, AICon);
	QueryReq.Execute(EEnvQueryRunMode::SingleResult, this, &UBTTask_GetAttackableArea::QueryFinished);*/

	BBComp->ClearValue(AICon->TargetLocationKey);

	FVector PlayerLo = Char->GetActorLocation();
	FVector AILo = Enemy->GetActorLocation();
	 

	FVector NewAILocation;

	//if (abs(PlayerLo.Z - AILo.Z) >= 30.f) //높이차가 30 이상이면 
	//{
	//	NewAILocation = PlayerLo; //그냥 PlayerLocation으로 해준다.
	//}
	//else //높이 차가 얼마 안나면
	{
		FVector RotationVec = (PlayerLo - AILo).GetSafeNormal(); //플레이어의 방향으로
		float Distance = (FVector::Dist(PlayerLo, AILo) - (Enemy->Range - Enemy->Range * 0.5)); //Player와 Enemy+Range의 거리를 구한다. -> 너무 타이트해서 살짝 더 나가게 해줬다.
		FVector AIWithRange = (RotationVec * Distance) + AILo; //Range만큼 나아간다. ->Range만큼만 나가면 안되고, Player의 위치와  AI의 위치의 차가 Range보다 얼만큼 클때~로 해준다.

		NewAILocation = FVector(AIWithRange.X, AIWithRange.Y, AIWithRange.Z); //Player의 높이로 해준다. -> ai가 턱에 걸려 못올라간다. 수정 필요.
	}
	
	//for debug
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackableArea : %s"), *NewAILocation.ToString());
		//UKismetSystemLibrary::DrawDebugSphere(this, AttackableLocation, 50.f, 12, FLinearColor::Blue, 3.f, 2.f);
		UKismetSystemLibrary::DrawDebugLine(this, AILo, NewAILocation, FLinearColor::Yellow, 2.f, 2.f);
		UKismetSystemLibrary::DrawDebugSphere(this, NewAILocation, 20.f, 8, FLinearColor::Yellow, 2.f, 2.f);

	}

	AICon->UpdateBBCompVectorKey(AICon->TargetLocationKey, NewAILocation);
	//AICon->UpdateAttackableLocationKey(NewAILocation);
	return Result;
}

//void UBTTask_GetAttackableArea::QueryFinished(TSharedPtr<FEnvQueryRequest>* Result)
//{
//	
//}


