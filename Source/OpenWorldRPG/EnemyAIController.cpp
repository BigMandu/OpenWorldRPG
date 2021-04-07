// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "MainCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	/**************  AI Perception°ü·Ã  ***************/
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing"));

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(15.f);
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	HearingConfig->bUseLoSHearing = true;
	HearingConfig->HearingRange = 1000.f;
	HearingConfig->LoSHearingRange = 1500.f;
	HearingConfig->SetMaxAge(15.f);
	//HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	//HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

}

void AEnemyAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::DetectedTarget);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	if (Target)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(Target);
		if (Player)
		{
			FVector DetectedLocation = Stimulus.StimulusLocation;

			FAISenseID HearingSenseID = HearingConfig->GetSenseID(); 
			FAISenseID SightSenseID = SightConfig->GetSenseID();
			
			/*
			FAISenseID HearingSenseID = UAISense::GetSenseID<UAISense_Hearing>(); 	
			if (PerceptionComponent->GetSenseConfig(HearingSenseID) != nullptr)
			{
				const FActorPerceptionInfo* HeardPerceptionInfo = PerceptionComponent->GetFreshestTrace(HearingSenseID);
			if (HeardPerceptionInfo != nullptr && PerceptionComponent->HasActiveStimulus(*HeardPerceptionInfo->Target, HearingSenseID))
			{
  			FVector HeardSomethingLocation = HeardPerceptionInfo->GetStimulusLocation(HearingSenseID);
  			float NoiseStrength = HeardPerceptionInfo->GetStimulusStrength(HearingSenseID);
			*/

			if (Stimulus.WasSuccessfullySensed())
			{
				if (GetWorldTimerManager().IsTimerActive(TargetLostTimer))
				{
					GetWorldTimerManager().ClearTimer(TargetLostTimer);
				}
				
				/*bIsSight = FPerceptionListener::HasSense(SightSenseID);
				bIsHearing = FPerceptionListener::HasSense(HearingSenseID);
				UE_LOG(LogTemp, Warning, TEXT("sight : %d, hearing : %d"), bIsSight, bIsHearing);*/

				if (Stimulus.Type == SightSenseID)
				{
					
					UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
				}
				else if (Stimulus.Type == HearingSenseID)
				{
					UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
				}

				UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString())
			}
			else
			{
				TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
				GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false);

				UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString())
			}
		}
	}
}

void AEnemyAIController::LostTarget(AActor* Target)
{
	if (GetWorldTimerManager().IsTimerActive(TargetLostTimer))
	{
		GetWorldTimerManager().ClearTimer(TargetLostTimer);
	}
	UE_LOG(LogTemp, Warning, TEXT("AI : Target Lost!, Lost Target : %s"), *Target->GetFName().ToString());
}

