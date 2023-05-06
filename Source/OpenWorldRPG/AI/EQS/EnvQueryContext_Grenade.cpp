// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Grenade.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "behaviortree/BlackboardComponent.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"

void UEnvQueryContext_Grenade::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* Owner = Cast<AActor>(QueryInstance.Owner.Get());
	check(Owner);
	AEnemyAIController* AICon = Cast<AEnemyAIController>(Owner->GetInstigatorController());
	if ( AICon == nullptr ) return;

	if ( AActor* Target = Cast<AActor>(AICon->BBComp->GetValueAsObject(AICon->DetectedGrenadeKey)))
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
	}


	//UE_CVLOG(GET_AI_CONFIG_VAR(bAllowControllersAsEQSQuerier) == false && Cast<AController>(QueryOwner) != nullptr, QueryOwner, LogEQS, Warning, TEXT("Using Controller as query's owner is dangerous since Controller's location is usually not what you expect it to be!"));

}
