// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTTask_HearSomething.h"
#include "OpenWorldRPG/AI/CustomAILibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"


UBTTask_HearSomething::UBTTask_HearSomething()
{
	NodeName = TEXT("Hear Something");
}

EBTNodeResult::Type UBTTask_HearSomething::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result  = EBTNodeResult::Succeeded;

	BBComp = OwnerComp.GetBlackboardComponent();
	AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());
	AIChar->SetAimMode(EAimMode::EAM_Aim);

	if(!AIChar || !AICon || !BBComp) return EBTNodeResult::Failed;

	GetWhereToAim();

	return Result;
}

void UBTTask_HearSomething::GetWhereToAim()
{
	FVector AimLocation = FVector::ZeroVector;

	if (CheckHaveToAimAndWait())
	{
		const FVector AILocation = AIChar->GetActorLocation();
		const FVector HearLocation = BBComp->GetValueAsVector(AICon->TargetLocationKey);
		/*UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(AIChar->GetWorld());
		if(NavSys == nullptr) return;*/
		UWorld* World = AIChar->GetWorld();

		//시작지점 -> 종료지점에서 linetrace를 쏴서 맞으면(직선임), 코너가 아니므로 리턴한다.
		/*bool bIsVisible = CornerRecogTraceIsHit(AILocation, HearLocation, *World);
		if(bIsVisible == false) return;*/

		UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, AILocation, HearLocation, AIChar);
		//UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(GetWorld(),AIChar->GetActorLocation(), HearLocation, AIChar);
		//NavSys->ProjectPointToNavigation()

		const TArray<FVector>& PathPointArr = Path->PathPoints;

		
		//LineTrace 방법
		for (FVector Point : PathPointArr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UBTTask_HearSomething::GetWhereToAim// DrawDebug!"));
			DrawDebugSphere(GetWorld(), Point, 5.f, 8, FColor::Green, false, 5.f, 0, 3.f);
			
			if (CornerRecogTraceIsHit(AILocation, Point, *World) == false)
			{
				AimLocation = Point;
			}
		}

		BBComp->SetValueAsVector(AICon->AIMLocationKey, AimLocation);

		//Angle 계산 방법 (모든 상황에 적용할 수 없음..)
		/*
		for (int32 idx = 1; idx < PathPointArr.Num() -1; ++idx)
		{
			FVector PrevDir = (PathPointArr[idx] - PathPointArr[idx-1]).GetSafeNormal2D();
			FVector NextDir = (PathPointArr[idx + 1] - PathPointArr[idx]).GetSafeNormal2D();

					
			//idx를 기준으로 이전, 이후의 두 점사이의 distance를 리턴한다.
			float Distance = FVector::Distance(PathPointArr[idx - 1], PathPointArr[idx +1]);

			//Dotproduct한 결과를 Acos함수의 파라미터로 넘겨서, 0~pi의 범위에 라디안값을 리턴한다.
			float AngleTH = FMath::RadiansToDegrees(UKismetMathLibrary::Acos(FVector::DotProduct(PrevDir, NextDir)));


			//// AngleThreshold만 사용한다면, dtNavMeshQuery::findPath 함수의 결과값인 PathPoint가
			//	직각 코너에서도 여러개의 Point가 찍히기 때문에 (즉 ,각도가 60도 하나, 30도 하나 이런식으로 직각 point가 찍히기 때문에)
			//	두 점사이의 Distance도 추가로 구해서
			//	Distance가 짧으면서, 각도의 비율을 구한다.
			//	이 값은 일단 임의로 0.1이라고 했으며 적당한 비율을 찾으면 된다.
			

			float Curv = AngleTH/Distance;
			UE_LOG(LogTemp, Warning, TEXT("UBTTask_HearSomething::GetWhereToAim// Angle : %f"), AngleTH);
			UE_LOG(LogTemp, Warning, TEXT("UBTTask_HearSomething::GetWhereToAim// Dist : %f"), Distance);
			UE_LOG(LogTemp, Warning, TEXT("UBTTask_HearSomething::GetWhereToAim// Vale Curve : %f"), Curv);

			//Curv 후보
			//0.085f
			
			if (Curv >= 0.05f )
			{
				//마지막 AimLocation이 담긴곳이 최종 Aim타겟 로케이션임.
				if (CornerRecogTraceIsHit(AILocation, PathPointArr[ idx ], *World) == false)
				{
					AimLocation = PathPointArr[idx];
				}
				UE_LOG(LogTemp, Warning, TEXT("UBTTask_HearSomething::GetWhereToAim// Red is Corner!"));
				DrawDebugSphere(GetWorld(), PathPointArr[idx], 5.f, 8, FColor::Red, false, 5.f, 0, 3.f);
			}			
		}
		*/
		DrawDebugSphere(GetWorld(), AimLocation, 5.f, 8, FColor::Magenta, false, 5.f, 0, 3.f);

	}
}

//Outside인지, Inside인지 판별후 Inside(건물내부)라면 Aim And Wait을 하도록 true를 리턴한다.
bool UBTTask_HearSomething::CheckHaveToAimAndWait()
{
	bool bReturnbool = false;
	//여기서 AILibrary::CheckIsInside함수 호출하기.
	//여기서 BBComp의 SetValue
	bReturnbool = UCustomAILibrary::CheckIsInside(*AIChar);
	BBComp->SetValueAsBool(AICon->bIsInsideKey, bReturnbool);

	return bReturnbool;
}

//장애물이 있다면 true를 리턴한다.
bool UBTTask_HearSomething::CornerRecogTraceIsHit(const FVector& StartLocation, const FVector& EndLocation, const UWorld& world)
{
	bool bReturnboolean = false;
	/* Ignore Actor */
	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(AIChar);

	AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(AICon->EnemyKey));
	if (Target)
	{
		IgnoreActor.Add(Target);
	}

	/* line trace*/
	FHitResult Hit;
	FCollisionQueryParams Params = FCollisionQueryParams(FName("CornerRecognizeTrace"), false, AIChar);
	Params.AddIgnoredActors(IgnoreActor);

	world.LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, Params);

	if (Hit.IsValidBlockingHit())
	{
		bReturnboolean = true;
	}

	return bReturnboolean;
}


void UBTTask_HearSomething::CheckWeapon()
{
	AIChar->EquippedWeapon->ChangeSafetyLeverForAI(EWeaponFiringMode::EWFM_FullAuto);
	AIChar->EquippedWeapon->bLMBDown = false;

	if (AIChar->EquippedWeapon->AmmoLeftInMag <= 0)
	{
		AIChar->EquippedWeapon->Reload();
	}

}