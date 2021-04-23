// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AttackMoving.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MainCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

UBTService_AttackMoving::UBTService_AttackMoving()
{
	NodeName = TEXT("AttackMoving");
	Interval = 0.0001;
}

void UBTService_AttackMoving::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon  = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(BBComp);
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->PlayerKey));
	check(Enemy);
	check(Main);

	FVector PlayerLo = Main->GetActorLocation();

	//플레이어 주변 빙글빙글 도는 코드
	/*
	float Dist = Enemy->Range - Enemy->Range * 0.3;
	FVector Distance = FVector(Dist, 0.f, 0.f);
	FVector AngleVec = FVector(0.f, 0.f, 1.f); // z = 1.f

	Angle += DeltaSeconds * 5.f;

	if (Angle >= 360.f)
	{
		Angle = 0.f;
	}

	FVector RotateVec = Distance.RotateAngleAxis(Angle, AngleVec);
	PlayerLo.X += RotateVec.X;
	PlayerLo.Y += RotateVec.Y;

	AICon->MoveToLocation(PlayerLo);
	*/
	PlayerLo = Main->GetActorLocation();
	FVector EnemyLo = Enemy->GetActorLocation();

	FVector Dis = (PlayerLo - EnemyLo).GetSafeNormal();
	FRotator NRot = Dis.Rotation();

	Enemy->SetActorRotation(NRot);
	
	UE_LOG(LogTemp, Warning, TEXT("NewLocation : %s"), *PlayerLo.ToString());
}