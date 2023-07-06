// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Rotate.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include <DrawDebugHelpers.h>
#include <NavigationSystem.h>

UBTService_Rotate::UBTService_Rotate()
{
	NodeName = TEXT("Rotate");
	Interval = 0.0001;
	RotateAlpha= 0.f;
	RotateDeltaTime = 0.f;
	TraceAlpha = 0.f;
	TraceDeltaTime = 0.f;

	//bCreateNodeInstance = true;
}


uint16 UBTService_Rotate::GetInstanceMemorySize() const
{
	return	sizeof(FBTRotateMemory);
}


void UBTService_Rotate::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
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
	FBTRotateMemory* MyMemory = reinterpret_cast< FBTRotateMemory* >( NodeMemory );
	if(MyMemory == nullptr ) return;

	if(AIChar->AIStatus != EAIStatus::EAS_Patrol )
	{
		
		return;
	}

	MyMemory->ClearDeltatime += DeltaSeconds;
	MyMemory->ClearAlpha = MyMemory->ClearDeltatime / 3.f;
	
	if ( MyMemory->ClearAlpha >= 1.f )
	{
		//debug
		//UE_LOG(LogTemp, Warning, TEXT("RotNode: Alpha : %f"), MyMemory->TraceAlpha);
		//UE_LOG(LogTemp, Warning, TEXT("RotNode: DelAlpha is Over 1.f clear Focus"));

		MyMemory->ClearDeltatime = 0.f;
		ClearFocusAndChangeRot(AICon,AIChar);
	}

	MyMemory->TraceDeltatime += DeltaSeconds;
	MyMemory->TraceAlpha = MyMemory->TraceDeltatime / 5.f;

	if( MyMemory->TraceAlpha >= 1.f )
	{
		//UE_LOG(LogTemp, Warning, TEXT("RotNode: RotAlpha is over 1.f fire LineTrace"));

		MyMemory->TraceDeltatime = 0.f;
		TraceAndSetFocus(AICon,AIChar);	
	}


	//Old Version
	/*
	if ( bRightHit == false && !GetWorld()->GetTimerManager().IsTimerActive(RightHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RightHandle);
		
		//오른쪽으로 턴
		UE_LOG(LogTemp,Warning,TEXT("RotateTickNode// need to Turn Right."));
		const FRotator SetRot = AIChar->GetActorRotation().Add(0.f,90.f,0.f);

		AICon->SetFocalPoint(RightFocusLocation);
		
		GetWorld()->GetTimerManager().SetTimer(RightHandle, [ = ] {		
			FRotator InterpRot = FMath::RInterpTo(AIChar->GetActorRotation(), SetRot, GetWorld()->GetDeltaSeconds(), 20.f);
			AIChar->SetActorRotation(InterpRot);
			IsReachedTargetRightRot = ( InterpRot.Yaw >=  SetRot.Yaw)? true : false; 
			UE_LOG(LogTemp, Warning, TEXT("RotateTickNode// TurnRight InterpRot : %s"),*InterpRot.ToString());
		},GetWorld()->GetDeltaSeconds(),true);
		
	}

	if ( bLeftHit == false && !GetWorld()->GetTimerManager().IsTimerActive(LeftHandle))
	{
		//Right로 돌아야 하는데, Right Rotation이 끝나지 않았다면 이 LeftTurn을 수행 않도록 한다.
		//if(bRightHit == false && IsReachedTargetRightRot == false ) return;

		//왼쪽으로 턴
		float InterpSpeed = 20.f;
		UE_LOG(LogTemp, Warning, TEXT("RotateTickNode// need to Turn Left."))
		GetWorld()->GetTimerManager().ClearTimer(LeftHandle);

		AICon->SetFocalPoint(LeftFocusLocation);
		//RightTurn을 하고 있었다면, 왼쪽으로 도는
		if ( GetWorld()->GetTimerManager().IsTimerActive(RightHandle) )
		{
			InterpSpeed += 20.f;
		}
		

		const FRotator SetRot = AIChar->GetActorRotation().Add(0.f, -90.f, 0.f);
		GetWorld()->GetTimerManager().SetTimer(LeftHandle, [ = ] {
			FRotator InterpRot = FMath::RInterpTo(AIChar->GetActorRotation(), SetRot, GetWorld()->GetDeltaSeconds(), InterpSpeed);
			AIChar->SetActorRotation(InterpRot);

			IsReachedTargetLeftRot = ( InterpRot.Yaw <= SetRot.Yaw ) ? true : false;
			UE_LOG(LogTemp, Warning, TEXT("RotateTickNode//TurnLeft InterpRot : %s"), *InterpRot.ToString());

		}, GetWorld()->GetDeltaSeconds(), true);
		//AIChar->SetActorRotation(SetRot);
	}
	*/


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

//Focus를 Clear해주고 AI의 방향을 진행방향으로 변경한다.
void UBTService_Rotate::ClearFocusAndChangeRot(AEnemyAIController* AICon, AEnemyCharacter* AIChar)
{
	AICon->ClearFocus(EAIFocusPriority::Default);


	UPathFollowingComponent* PathFollowComp = AICon->GetPathFollowingComponent();
	if ( PathFollowComp == nullptr ) return;
	const FNavPathSharedPtr Path = PathFollowComp->GetPath();
	if ( Path.IsValid() == false ) return;

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
		FVector StartPathPointDir = ( PathPoint[ 1 ].Location - PathPoint[ 0 ].Location ).GetSafeNormal();
		FVector TargetFocusLoc = CurrentLoc + StartPathPointDir * 1000.f;

		//경로방향으로 SetFocus를 해준뒤에 바로 ClearFocus를 해준다.
		//만약 Defalt(우선순위가 제일 높음)으로 했을경우, 이 코드에 들어올때 Enemy가 Set이 되면
		//Location에 Focus된게 Clear되지 않음.. 한단계 낮은 Gameplay로 하니 잘 풀어진다.
		AICon->SetFocalPoint(TargetFocusLoc,EAIFocusPriority::Gameplay);
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
	}
}


/**AI의 머리에서 Perception의 Sightconfig에서 지정한 시야각의 양 끝단에서 LineTrace를 쏘고
 * LineTrace에 아무것도 맞지 않았을 경우에, 해당 방향으로 회전하는 함수
 * 
 */
void UBTService_Rotate::TraceAndSetFocus(AEnemyAIController* AICon, AEnemyCharacter* AIChar)
{
	//LineTrace의 시작점을 머리에서 부터 하기위해 머리 head socket의 위치를 가져온다.
	const FVector LineTraceStartLoc = AIChar->GetMesh()->GetSocketLocation(AIChar->HeadSocketName);
	float SingleAngle = AICon->SightConfig->PeripheralVisionAngleDegrees;

	//LineTrace의 길이를 정한다.
	float AngleRecognizeDist = 200.f;

	//AI의 현재 방향에서 시야각만큼의 Rot을 증감하여 라인트레이스의 방향을 정한다.
	const FRotator RightRot = AIChar->GetActorRotation().Add(0.f, SingleAngle, 0.f);
	const FRotator LeftRot = AIChar->GetActorRotation().Add(0.f, -SingleAngle, 0.f);

	const FVector RightFocusLocation = LineTraceStartLoc + RightRot.Vector() * AngleRecognizeDist;
	const FVector LeftFocusLocation = LineTraceStartLoc + LeftRot.Vector() * AngleRecognizeDist;

	/*DrawDebugLine(GetWorld(), LineTraceStartLoc, RightFocusLocation, FColor::Magenta, false, 1.f, 0, 2.f);
	DrawDebugLine(GetWorld(), LineTraceStartLoc, LeftFocusLocation, FColor::Magenta, false, 1.f, 0, 2.f);*/

	//Hit이 됐는지 안됐는지만 알면되니 HitResult를 공동변수로 사용한다.
	FHitResult Hit;
	bool bRightHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStartLoc, RightFocusLocation, ECollisionChannel::ECC_Visibility);
	bool bLeftHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStartLoc, LeftFocusLocation, ECollisionChannel::ECC_Visibility);

	//Hitresult가 없는경우에 빈공간이며, 해당 방향으로 회전해야한다.
	//둘 다 안맞은 경우 양쪽 모두 체크해야하며 오른쪽부터 체크 하기 위해 RightHit부터 검증한다.

	//양쪽 모두 뻥 뚫려있다면
	if ( !bRightHit && !bLeftHit )
	{
		//0.f~ 2.f값 사이의 랜덤값을 불러온다.
		float Judgevalue = FMath::FRandRange(0.f, 2.3f);
		//UE_LOG(LogTemp,Warning,TEXT("RotateNode:: JudgeValue is %f"), Judgevalue);
		//1.8이하의 값이라면 왼/오 중에 택한다.
		if ( Judgevalue <= 1.8f )
		{
			if ( Judgevalue >= 0.6 )
			{
				AICon->SetFocalPoint(LeftFocusLocation, EAIFocusPriority::Default);
			}
			else
			{
				AICon->SetFocalPoint(RightFocusLocation, EAIFocusPriority::Default);
			}

		}
		//1.8 초과의 값이라면 뒤를 향하도록 한다.
		else
		{
			const FRotator Rot = AIChar->GetActorRotation().Add(0.f, 180.f, 0.f);
			const FVector RearFocusLocation = LineTraceStartLoc + Rot.Vector() * AngleRecognizeDist;
			AICon->SetFocalPoint(RearFocusLocation);
		}
	}
	//오른쪽만 뚫려있는 경우
	else if ( !bRightHit && bLeftHit )
	{
		AICon->SetFocalPoint(RightFocusLocation, EAIFocusPriority::Default);
	}
	//왼쪽만 뚫려있는 경우
	else if ( bRightHit && !bLeftHit )
	{
		AICon->SetFocalPoint(LeftFocusLocation, EAIFocusPriority::Default);
	}

}
