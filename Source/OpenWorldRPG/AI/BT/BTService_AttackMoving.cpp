// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AttackMoving.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"

UBTService_AttackMoving::UBTService_AttackMoving()
{
	NodeName = TEXT("AttackMoving");
	Interval = 0.0001;

	Distance = 100.f;
	Alpha = 0.f;
}

void UBTService_AttackMoving::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon  = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(BBComp);
	check(AICon);
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetCharacter());
	AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	check(Enemy);
	check(Main);

	FVector EnemyLo = Enemy->GetActorLocation();
	FVector RightVec = Enemy->GetActorRightVector();
	
	Alpha += DeltaSeconds;

	if (Alpha >= 2.f)
	{
		Alpha = 0.f;
		
		Distance *= -1;
	}

	RightVec.X *= Distance;
	RightVec.Y *= Distance;

	FVector MovementVec = FVector(RightVec.X+EnemyLo.X, RightVec.Y+EnemyLo.Y, EnemyLo.Z); 
	//->좌우로 움직이는 코드는 AIController에 넣어줬다. 이 서비스는 인터벌마다 불러와서 실행되기때문에 토글형식이 안먹힘.


	//FVector PlayerLo = Main->GetActorLocation();

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

	//플레이어를 향해 회전하는 코드 -> AIController Tick에서 구현함.
	/*
	PlayerLo = Main->GetActorLocation();
	FVector EnemyLo = Enemy->GetActorLocation();

	FVector Dis = (PlayerLo - EnemyLo).GetSafeNormal();
	FRotator NRot = Dis.Rotation();

	Enemy->SetActorRotation(NRot);
	*/

	//디버깅
	{
		UE_LOG(LogTemp, Warning, TEXT("MovementVec : %s"), *MovementVec.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Delta : %f"), DeltaSeconds);
		UKismetSystemLibrary::DrawDebugLine(this, EnemyLo, MovementVec, FLinearColor::Yellow, 2.f, 2.f);
	}
}