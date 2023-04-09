// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTTask_GetChaseLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include <DrawDebugHelpers.h>
#include <OpenWorldRPG/BaseCharacter.h>
#include <OpenWorldRPG/AI/EnemyAIController.h>
//#include "OpenWorldRPG/AI/EnemyAIController.h"

UBTTask_GetChaseLocation::UBTTask_GetChaseLocation()
{
	bNotifyTick = false;

}


void UBTTask_GetChaseLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
		LastRotatorKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_GetChaseLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type ReturnResult = EBTNodeResult::Failed;

	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	
	if(AICon && BBComp)
	{
		/*Character의 Canbeseenfrom은 Head Socket을 가지고 인식하기 때문에
		* 여기서 Target값과 아래 인식한 Location의 Zvalue가 서로 달라 예외처리로 빠져 문제가 발생하여
		* CurrentTargetLoc값을 인식값과 같도록 HeadSocket의 위치로 함.
		*/
		/*FVector CurrentTargetLocation = Cast<AActor>(BBComp->GetValueAsObject(AICon->EnemyKey))->GetActorLocation();*/

		ABaseCharacter* Target = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
		if(Target == nullptr ) return EBTNodeResult::Failed;
		FVector CurrentTargetLocation = Target->GetMesh()->GetSocketLocation(Target->HeadSocketName);

		

		FVector LastSeenLocation =BBComp->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		FRotator LastSeenRotation = BBComp->GetValue<UBlackboardKeyType_Rotator>(LastRotatorKey.GetSelectedKeyID());
		FVector RotationVec = LastSeenRotation.Vector();

		FVector FinalSearchLocation = FVector(0.f);


		//마지막 Target의 높이와 현재Target의 높이가 다른경우 수색 위치를 마지막 식별위치로 대입하고 끝낸다.
		if (!FMath::IsNearlyEqual(CurrentTargetLocation.Z, LastSeenLocation.Z, 1.f) || !FMath::IsNearlyEqual(OwnerComp.GetOwner()->GetActorLocation().Z, LastSeenLocation.Z, 1.f) )
		{
			UE_LOG(LogTemp,Warning,TEXT("UBTTask_GetChaseLocation::ExecuteTask // Last Seen Location's Z value is different. Set FinalSerachLocation To Last Seen Location"));
			UE_LOG(LogTemp, Warning, TEXT("CurrentTargetZ : %f, LastSeenZ : %f, ThisAI Z : %f ",CurrentTargetLocation.Z,LastSeenLocation.Z, OwnerComp.GetOwner()->GetActorLocation().Z));
			FinalSearchLocation = LastSeenLocation;
		}
		//높이가 같은경우 == Target이 코너나, 장애물 사이로 들어가 숨은 경우임.
		//따라서 타겟의 마지막 식별 위치와, Target의 현재 위치의 거리값을 구해서 
		//그 거리값을 최소 : 거리값/3,  최대 : 거리값  사이의 랜덤한 값을 구해
		// 현재 OwnerAI의 위치에 Target의 마지막 식별 회전값 * 거리 랜덤값을 더해 최종 수색 위치를 구한다.
		else
		{
			//Target의 현재 위치와 마지막 식별 위치의 거리를 구한다.
			FVector LocationDist = FVector(FVector::Dist(LastSeenLocation, CurrentTargetLocation));

			//위에서 구한 거리에서 무작위값 3으로 나눈값(최소) 과 원래값(최대)사이의 랜덤 거리 값을 구한다.
			float RandDist = FMath::RandRange(LocationDist.Size() / 3, LocationDist.Size());

			//Target의 마지막 식별 회전값에 위의 랜덤 거리float값을 곱해  추가적으로 얼만큼 더 나갈지 결정하는 Vector값을 구한다.
			FVector PreCalc = FVector(RotationVec.X * RandDist, RotationVec.Y * RandDist, RotationVec.Z);


			//현재 이 OwnerAI의 위치값에 위에서 구한 방향+거리벡터를 더해 최종 수색 위치를 결정한다.
			FinalSearchLocation = OwnerComp.GetOwner()->GetActorLocation() + PreCalc;
		}

		DrawDebugSphere(GetWorld(), LastSeenLocation, 8.f, 6, FColor::Purple, false, 10.f, 0, 3.f);
		DrawDebugSphere(GetWorld(), CurrentTargetLocation, 8.f, 6, FColor::Green, false, 10.f, 0, 3.f);
		DrawDebugSphere(GetWorld(), FinalSearchLocation, 8.f, 6, FColor::Red, false, 10.f, 0, 3.f);

		//구한 최종 SearchLocation을 BBKey에 Set한다.
		BBComp->SetValueAsVector(AICon->EstimatedLocationOfTargetKey,FinalSearchLocation);
		ReturnResult = EBTNodeResult::Succeeded;
	}

	


	return ReturnResult;
}