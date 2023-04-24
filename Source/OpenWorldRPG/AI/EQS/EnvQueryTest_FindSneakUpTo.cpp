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

	BoolValue.BindData(Owner, QueryInstance.QueryID);
	bool bWantsFOV = BoolValue.GetValue();

	TArray<FVector> ContextLocations;
	if ( !QueryInstance.PrepareContext(Context, ContextLocations) )
	{
		UE_LOG(LogTemp,Warning,TEXT("UEnvQueryTest_FindSneakUpTo// Can't PrepareContext, exit func"));
		return;
	}

	for ( FEnvQueryInstance::ItemIterator iter(this, QueryInstance); iter; ++iter )
	{
		const FVector ItemLo = GetItemLocation(QueryInstance, iter.GetIndex());

		for ( int32 ContextInd = 0; ContextInd < ContextLocations.Num(); ++ContextInd )
		{
			FVector TargetLo = ContextLocations[ContextInd];
			FVector TargetFowVec = ContextLocations[ContextInd].ForwardVector;
			float TargetHalfFov = 90.f;

			//Item을 기준으로 Target(Context)의 방향을 구한다.
			FVector DirTowardThisItemVec = (TargetLo - ItemLo).GetSafeNormal();

			float DotCalc = FVector::DotProduct(TargetFowVec, DirTowardThisItemVec);
			bool bIsWithinFieldOfView = true;

			if ( DotCalc > UKismetMathLibrary::Cos(TargetHalfFov) )
			{
				//InFov가 아님.
				bIsWithinFieldOfView = false;
			}
			iter.SetScore(TestPurpose, FilterType, bIsWithinFieldOfView, bWantsFOV);
		}
		
	}	
}