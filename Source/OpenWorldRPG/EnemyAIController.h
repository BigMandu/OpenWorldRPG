// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	/*********** AI Perception ฐüทร ***************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UAISenseConfig_Hearing* HearingConfig;

	FTimerHandle TargetLostTimer;
	FTimerDelegate TargetLostDelegate;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UNavigationSystemV1* NavSys;*/


protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void DetectedTarget(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void LostTarget(AActor* Target);


	
};
