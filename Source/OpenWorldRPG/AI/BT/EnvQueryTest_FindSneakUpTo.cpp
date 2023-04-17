// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_FindSneakUpTo.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include <OpenWorldRPG/AI/EnemyAIController.h>
#include "behaviortree/BlackboardComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "AI/Navigation/NavigationTypes.h"
#include <NavigationSystem.h>


UEnvQueryTest_FindSneakUpTo::UEnvQueryTest_FindSneakUpTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::High;

	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);

	Context = UEnvQueryContext_Querier::StaticClass();
	
}

void UEnvQueryTest_FindSneakUpTo::RunTest(FEnvQueryInstance& QueryInstance) const
{
	AActor* Owner = Cast<AActor>(QueryInstance.Owner.Get());
	if(!Owner ) return;
	AEnemyAIController* AICon = Cast<AEnemyAIController>(Owner->GetInstigatorController());
	AActor* Target = nullptr;
	if ( UObject* Obj = AICon->BBComp->GetValueAsObject(AICon->EnemyKey) )
	{
		Target = Cast<AActor>(Obj);
	}
	

	//UObject* Owner = QueryInstance.Owner.Get();
	//AActor* Target = Cast<AActor>(QueryInstance.GetItemAsActor(0));

	if(!AICon || !Target) return;


	FVector TargetLocation = Target->GetActorLocation();
	FVector ContextLocation = Owner->GetActorLocation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, ContextLocation);
	float AngleToTarget = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, Target->GetActorRotation()).Yaw;

	//임의로 80을 사용함.
	bool bIsWithinFieldOfView = FMath::Abs(AngleToTarget) <= 80.f / 2.0f;

	if ( bIsWithinFieldOfView )
	{
		QueryInstance.ItemType = UEnvQueryItemType_VectorBase::StaticClass(); // EEnvQueryItemType::Location;
		FVector IdealLocation = FindIdealSneakUpLocation(Owner, Target);

		for ( FEnvQueryInstance::ItemIterator iter(this, QueryInstance); iter; ++iter )
		{
			iter.SetScore(EEnvTestPurpose::Score,EEnvTestFilterType::Maximum,1.f,true);
		}
		/*QueryInstance.SetItemLocation(0, IdealLocation);
		QueryInstance.SetItemScore(0, 1.0f);*/
	}
	
}


FVector UEnvQueryTest_FindSneakUpTo::FindIdealSneakUpLocation(AActor* ContextObject, AActor* TargetActor) const
{
	// TODO: Implement your logic for finding the ideal sneak-up location here
	// You may want to use the AI's navigation system or some other method to find a suitable location
	// For example, you could use UNavigationSystemV1::GetRandomPointInNavigableRadius() to get a random point near the target
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(ContextObject->GetWorld());// FNavigationSystem::GetCurrent<UNavigationSystemV1>();
	if ( NavSys )
	{
		float SearchRadius = 1000.0f;
		FVector SearchOrigin = TargetActor->GetActorLocation();
		FNavLocation RandLoc;

		if ( NavSys->GetRandomPointInNavigableRadius(SearchOrigin,SearchRadius,RandLoc))
		{
			return RandLoc;
		}
	}

	// Failed to find a suitable location - return the target's location as a fallback
	return TargetActor->GetActorLocation();
}