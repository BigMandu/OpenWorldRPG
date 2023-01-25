// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTTask_GetChaseLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
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
		FVector CurrentTargetLocation = Cast<AActor>(BBComp->GetValueAsObject(AICon->EnemyKey))->GetActorLocation();

		FVector LastSeenLocation =BBComp->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		FRotator LastSeenRotation = BBComp->GetValue<UBlackboardKeyType_Rotator>(LastRotatorKey.GetSelectedKeyID());
		FVector RotationVec = LastSeenRotation.Vector();

		FVector FinalSearchLocation = FVector(0.f);

		//������ Target�� ���̿� ����Target�� ���̰� �ٸ���� ���� ��ġ�� ������ �ĺ���ġ�� �����ϰ� ������.
		if (CurrentTargetLocation.Z != LastSeenLocation.Z || LastSeenLocation.Z != OwnerComp.GetOwner()->GetActorLocation().Z)
		{
			FinalSearchLocation = LastSeenLocation;
		}
		//���̰� ������� == Target�� �ڳʳ�, ��ֹ� ���̷� �� ���� �����.
		//���� Ÿ���� ������ �ĺ� ��ġ��, Target�� ���� ��ġ�� �Ÿ����� ���ؼ� 
		//�� �Ÿ����� �ּ� : �Ÿ���/3,  �ִ� : �Ÿ���  ������ ������ ���� ����
		// ���� OwnerAI�� ��ġ�� Target�� ������ �ĺ� ȸ���� * �Ÿ� �������� ���� ���� ���� ��ġ�� ���Ѵ�.
		else
		{
			//Target�� ���� ��ġ�� ������ �ĺ� ��ġ�� �Ÿ��� ���Ѵ�.
			FVector LocationDist = FVector(FVector::Dist(LastSeenLocation, CurrentTargetLocation));

			//������ ���� �Ÿ����� �������� 3���� ������(�ּ�) �� ������(�ִ�)������ ���� �Ÿ� ���� ���Ѵ�.
			float RandDist = FMath::RandRange(LocationDist.Size() / 3, LocationDist.Size());

			//Target�� ������ �ĺ� ȸ������ ���� ���� �Ÿ�float���� ����  �߰������� ��ŭ �� ������ �����ϴ� Vector���� ���Ѵ�.
			FVector PreCalc = FVector(RotationVec.X * RandDist, RotationVec.Y * RandDist, RotationVec.Z);


			//���� �� OwnerAI�� ��ġ���� ������ ���� ����+�Ÿ����͸� ���� ���� ���� ��ġ�� �����Ѵ�.
			FinalSearchLocation = OwnerComp.GetOwner()->GetActorLocation() + PreCalc;
		}

		//���� ���� SearchLocation�� BBKey�� Set�Ѵ�.
		BBComp->SetValueAsVector(AICon->EstimatedLocationOfTargetKey,FinalSearchLocation);
		ReturnResult = EBTNodeResult::Succeeded;
	}

	


	return ReturnResult;
}