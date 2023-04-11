// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Rotate.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTService_Rotate.h"
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

	//코너 탐지후 회전
	
	UPathFollowingComponent* PathFollowComp = AICon->GetPathFollowingComponent();
	if ( PathFollowComp == nullptr ) return;
	const FNavPathSharedPtr Path = PathFollowComp->GetPath();
	if(Path.IsValid() == false ) return ;
	
	
	TArray<FNavPathPoint> PathPoint = Path->GetPathPoints();

	int32 PathPointNum = PathPoint.Num();
	if ( PathPointNum >= 3 )
	{
		for ( auto CornerPoint : PathPoint )
		{
			DrawDebugSphere(GetWorld(), CornerPoint.Location, 8.f, 6, FColor::Purple, false, 10.f, 0, 3.f);
		}
		
	}



	/*PathFollowComp->GetNextPathIndex();

	const FNavPathPoint& NextPathPt = Path->GetPathPoints()[ PathFollowComp->GetNextPathIndex()];
	if ( NextPathPt.HasNodeRef() )
	{

	}*/
	

}
