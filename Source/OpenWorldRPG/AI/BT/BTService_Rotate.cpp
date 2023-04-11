// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Rotate.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include <DrawDebugHelpers.h>

UBTService_Rotate::UBTService_Rotate()
{
	NodeName = TEXT("Rotate");
	Interval = 0.0001;

	//Distance = 100.f;
	Alpha = 0.f;
}

void UBTService_Rotate::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	check(BBComp);
	check(AICon);
	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());
	check(AIChar);


	const FVector LineTraceStartLoc = AIChar->GetMesh()->GetSocketLocation(AIChar->HeadSocketName);
	float SingleAngle = AICon->SightConfig->PeripheralVisionAngleDegrees;
	float AngleRecognizeDist = 200.f;

	const FRotator RightRot = AIChar->GetActorRotation().Add(0.f, SingleAngle, 0.f);
	const FRotator LeftRot = AIChar->GetActorRotation().Add(0.f,-SingleAngle,0.f);

	//Hit이 됐는지 안됐는지만 알면되니 HitResult를 공동변수로 사용한다.
	FHitResult Hit;

	DrawDebugLine(GetWorld(), LineTraceStartLoc, LineTraceStartLoc + RightRot.Vector() * AngleRecognizeDist,FColor::Magenta,false,1.f,0,2.f);
	DrawDebugLine(GetWorld(), LineTraceStartLoc, LineTraceStartLoc + LeftRot.Vector() * AngleRecognizeDist,FColor::Magenta,false,1.f,0,2.f);
	bool bRightHit = GetWorld()->LineTraceSingleByChannel(Hit,LineTraceStartLoc,LineTraceStartLoc+RightRot.Vector()*AngleRecognizeDist,ECollisionChannel::ECC_WorldStatic);
	bool bLeftHit = GetWorld()->LineTraceSingleByChannel(Hit,LineTraceStartLoc,LineTraceStartLoc+ LeftRot.Vector()*AngleRecognizeDist,ECollisionChannel::ECC_WorldStatic);

	//Hitresult가 없는경우에 빈공간이며, 해당 방향으로 회전해야한다.
	//둘 다 안맞은 경우 양쪽 모두 체크해야하며 오른쪽부터 체크 하기 위해 RightHit부터 검증한다.
	
	if ( bRightHit == false )
	{
		//오른쪽으로 턴
		const FRotator SetRot = AIChar->GetActorRotation().Add(0.f,90.f,0.f);
		AIChar->SetActorRotation(SetRot);
	}

	if ( bLeftHit == false )
	{
		//왼쪽으로 턴
		const FRotator SetRot = AIChar->GetActorRotation().Add(0.f, -90.f, 0.f);
		AIChar->SetActorRotation(SetRot);
	}



	/*코너 탐지후 회전 하는 코드*/
	/*
	UPathFollowingComponent* PathFollowComp = AICon->GetPathFollowingComponent();
	if ( PathFollowComp == nullptr ) return;
	const FNavPathSharedPtr Path = PathFollowComp->GetPath();
	if(Path.IsValid() == false ) return ;
	
	//PathPoint를 구해온다.
	TArray<FNavPathPoint> PathPoint = Path->GetPathPoints();

	//현재 AI의 방향을 구한다.
	FVector CurrentLoc = AIChar->GetActorLocation();

	//구한 PathPoint의 개수를 얻어온다, (경로 탐색이 끝난 경우 최대 2개이고, 이 경우는 무조건 경로탐색이 끝난 뒤다.)
	//- 참고 : 경로탐색중엔 코너부분의 PathPoint는 3이상이 나올 수 있다.
	//경로탐색이 끝난 PathPoint에는 시작점/끝점 두개만 존재한다.
	int32 PathPointNum = PathPoint.Num();
	if ( PathPointNum >= 2 )
	{
		//시작지점에서 종료방향을 구한다.
		FVector StartPathPointDir = (PathPoint[1].Location - PathPoint[0].Location).GetSafeNormal();
		//종료지점에서 시작지점의 방향을 구한다.
		FVector EndPathPointDir = (PathPoint[PathPointNum-1].Location - PathPoint[PathPointNum-2].Location).GetSafeNormal();

		if ( FVector::DotProduct(CurrentDirection, StartPathPointDir) < 0.5f && FVector::DotProduct(CurrentDirection, EndPathPointDir) < 0.5f )
		{
			AICon->GetCharacter()->AddControllerYawInput(90.0f);
		}

	}
*/


	/*PathFollowComp->GetNextPathIndex();

	const FNavPathPoint& NextPathPt = Path->GetPathPoints()[ PathFollowComp->GetNextPathIndex()];
	if ( NextPathPt.HasNodeRef() )
	{

	}*/
	

}
