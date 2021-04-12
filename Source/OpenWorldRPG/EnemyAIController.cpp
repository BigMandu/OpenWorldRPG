// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "MainCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	/**************  AI Perception관련  ***************/
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
	//HearingConfig->DetectionByAffiliation.bDetectNeutrals = true; //기본세팅이 Friend로 되어있어서 주석처리 했다.
	//HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	/************** Behavoir ***********/
	BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

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

void AEnemyAIController::OnPossess(APawn* InPawn) //AIController가 해당Enemy를 제어할 수 있도록함.
{
	Super::OnPossess(InPawn);

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);

	if (Enemy)
	{
		const FVector OriginPosition = Enemy->GetActorLocation();
		
		BBComp->InitializeBlackboard(*(Enemy->BTAsset->BlackboardAsset)); //Blackboard초기화
		BBComp->SetValueAsVector(OriginPosKey, OriginPosition); //Spawn위치 저장.
		BBComp->SetValueAsBool(bHasTargetPointsKey, Enemy->bHasPatrolPoints); //Targetpoint 보유 여부를 넘겨줌.
		BBComp->SetValueAsInt(TargetPointIndexKey, 0); //Targetpoint index를 0으로 세팅한다.
		
		BTComp->StartTree(*(Enemy->BTAsset)); //마지막에 StartTree
	}	
}

void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	if (Target)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(Target);
		if (Player)
		{
			FVector DetectedLocation = Stimulus.StimulusLocation;

			FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing인지
			FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight인지 따로 처리를 하기 위해  ID를 가져온다.

			if (Stimulus.WasSuccessfullySensed())
			{
				if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
				{
					GetWorldTimerManager().ClearTimer(TargetLostTimer);
				}
				
				if (Stimulus.Type == SightSenseID) //Sight를 감지했을때
				{
					
					UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
				}
				
				if (Stimulus.Type == HearingSenseID) //Hearing을 감지했을때
				{
					UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
				}

				UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString())
			}
			else //감지를 못했을때
			{
				TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
				GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.

				UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString())
			}
		}
	}
}

void AEnemyAIController::LostTarget(AActor* Target)
{
	if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
	{
		GetWorldTimerManager().ClearTimer(TargetLostTimer);
	}
	UE_LOG(LogTemp, Warning, TEXT("AI : Target Lost!, Lost Target : %s"), *Target->GetFName().ToString());
}


/******************  Black board Key Update Function***********************/
void AEnemyAIController::UpdateTargetPointIndex(int32 index)
{
	BBComp->SetValueAsInt(TargetPointIndexKey, index);
}

void AEnemyAIController::UpdatePatrolPosKey(FVector NewPatrolPos)
{
	BBComp->SetValueAsVector(PatrolPosKey, NewPatrolPos);
}