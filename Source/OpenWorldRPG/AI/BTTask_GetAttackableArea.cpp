// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetAttackableArea.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/EnemyCharacter.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/MainCharacter.h"
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
	 

	FVector NewAILocation;

	//if (abs(PlayerLo.Z - AILo.Z) >= 30.f) //�������� 30 �̻��̸� 
	//{
	//	NewAILocation = PlayerLo; //�׳� PlayerLocation���� ���ش�.
	//}
	//else //���� ���� �� �ȳ���
	{
		FVector RotationVec = (PlayerLo - AILo).GetSafeNormal(); //�÷��̾��� ��������
		float Distance = (FVector::Dist(PlayerLo, AILo) - (Enemy->Range - Enemy->Range * 0.5)); //Player�� Enemy+Range�� �Ÿ��� ���Ѵ�. -> �ʹ� Ÿ��Ʈ�ؼ� ��¦ �� ������ �����.
		FVector AIWithRange = (RotationVec * Distance) + AILo; //Range��ŭ ���ư���. ->Range��ŭ�� ������ �ȵǰ�, Player�� ��ġ��  AI�� ��ġ�� ���� Range���� ��ŭ Ŭ��~�� ���ش�.

		NewAILocation = FVector(AIWithRange.X, AIWithRange.Y, AIWithRange.Z); //Player�� ���̷� ���ش�. -> ai�� �ο� �ɷ� ���ö󰣴�. ���� �ʿ�.
	}
	
	//for debug
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackableArea : %s"), *NewAILocation.ToString());
		//UKismetSystemLibrary::DrawDebugSphere(this, AttackableLocation, 50.f, 12, FLinearColor::Blue, 3.f, 2.f);
		UKismetSystemLibrary::DrawDebugLine(this, AILo, NewAILocation, FLinearColor::Yellow, 2.f, 2.f);
		UKismetSystemLibrary::DrawDebugSphere(this, NewAILocation, 20.f, 8, FLinearColor::Yellow, 2.f, 2.f);

	}
	AICon->UpdateAttackableLocationKey(NewAILocation);
	return Result;
}