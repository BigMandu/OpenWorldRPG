// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "EnemyCharacter.h"
#include "MainCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
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
	//SightConfig->SetMaxAge(15.f); //OnPossess함수에서 다시 변경함.
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	

	HearingConfig->bUseLoSHearing = true;
	HearingConfig->HearingRange = 1000.f;
	HearingConfig->LoSHearingRange = 1500.f;
	//HearingConfig->SetMaxAge(15.f); //OnPossess함수에서 다시 변경함.
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

	if (BBComp->GetValueAsBool(bSeePlayerKey) == true) //player를 식별했으면
	{
		AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(PlayerKey));
		if (Main)
		{
			FVector PlayerLo = Main->GetActorLocation();
			FVector EnemyLo = Enemy->GetActorLocation();

			FVector Dis = (PlayerLo - EnemyLo).GetSafeNormal();
			FRotator NRot = Dis.Rotation();

			Enemy->SetActorRotation(NRot);

			float Distance = FVector::Dist(Enemy->GetActorLocation(), Main->GetActorLocation());
			if (Distance < (Enemy->Range - (Enemy->Range * 0.1))) //main과 Enemy의 거리가 enemy range내에 있으면 공격 가능.
			{
				BBComp->SetValueAsBool(CanAttackKey, true);
			}
			else
			{
				BBComp->SetValueAsBool(CanAttackKey, false);
			}
		}
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn) //AIController가 해당Enemy를 제어할 수 있도록함.
{
	Super::OnPossess(InPawn);

	Enemy = Cast<AEnemyCharacter>(InPawn);
	check(Enemy);

	if (Enemy)
	{
		const FVector OriginPosition = Enemy->GetActorLocation();
		BBComp->InitializeBlackboard(*(Enemy->BTAsset->BlackboardAsset)); //Blackboard초기화
		BBComp->SetValueAsVector(OriginPosKey, OriginPosition); //Spawn위치 저장.
		BBComp->SetValueAsBool(bHasTargetPointsKey, Enemy->bHasPatrolPoints); //Targetpoint 보유 여부를 넘겨줌.
		BBComp->SetValueAsInt(TargetPointIndexKey, 0); //Targetpoint index를 0으로 세팅한다.
		
		BTComp->StartTree(*(Enemy->BTAsset)); //마지막에 StartTree


		//Sight, Hearing의 세팅
		SightConfig->SetMaxAge(Enemy->SightMaxAge);
		HearingConfig->SetMaxAge(Enemy->HearingMaxAge);
		PerceptionComponent->SetDominantSense(SightConfig->Implementation); //우선순위를 Sight로 설정.
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

			const FActorPerceptionInfo* info = PerceptionComponent->GetActorInfo(*Player);
			check(info);

			//LastSensedStimuli를 이용한 방법.
			/*
			if (info->LastSensedStimuli[SightSenseID].WasSuccessfullySensed())
			{
				Enemy->bSeePlayer = true;
				Enemy->bHearPlayer = false;
				UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
				UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
				UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());
			}
			else if (info->LastSensedStimuli[HearingSenseID].WasSuccessfullySensed())
			{
				Enemy->bHearPlayer = true;
				Enemy->bSeePlayer = false;
				UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
				UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, HearingMaxAge is : %f"), Stimulus.GetAge(), HearingConfig->GetMaxAge());
				UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());
			}*/
			
			//Stimulus.WasSuccessfullySensed() ==flase와 같은 역할함.
			/*if (info->HasAnyCurrentStimulus() == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("AI ActorInfo :: has not detected"));
			}*/

			if (Stimulus.WasSuccessfullySensed())
			{
				if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
				{
					GetWorldTimerManager().ClearTimer(TargetLostTimer);
				}


				//마지막이 아닌, Age가 긴 Sense를 가져온다.
				/*for (int32 Sense = 0; Sense < info->LastSensedStimuli.Num(); ++Sense)
				{
					float Age = info->LastSensedStimuli[Sense].GetAge();
					if (info->LastSensedStimuli[Sense].WasSuccessfullySensed())
					{
						FName Senseid;
						if (Sense == 0)
						{
							Senseid = TEXT("Sight");
						}
						else if (Sense == 1)
						{
							Senseid = TEXT("Hearing");
						}
						UE_LOG(LogTemp, Warning, TEXT("LastStimuli Sense is : %d"), Sense);
						UE_LOG(LogTemp, Warning, TEXT("LastStimuli Age is : %.2f"), Age);
					}
				}*/

				//DominantSense는 GetLastStimuliLocation함수에만 쓰이는것 같다. 
				//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());
				

				if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight를 감지했을때
				{
					Enemy->bSeePlayer = true; //디버깅
					Enemy->bHearPlayer = false; //디버깅
					UpdateSeePlayerKey(true);
					UpdateHearPlayerKey(false);
					UpdatePlayerKey(Player); //Player를 Setting해준다.
					UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
					UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
				}
				else if (Stimulus.Type == HearingSenseID && info->IsSenseActive(SightSenseID) == false)// && Stimulus.Strength >= 1.f) //Hearing을 감지했을때
				{
					Enemy->bHearPlayer = true; //디버깅
					Enemy->bSeePlayer = false;
					UpdateSeePlayerKey(false);
					UpdateHearPlayerKey(true);
					UpdateHearLocationKey(Stimulus.StimulusLocation);
					//청각만 들었을때 Hearing의 MaxAge이후 LostTarget을 실행한다.
					TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
					GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, HearingConfig->GetMaxAge(), false);
					UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
					UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, HearingMaxAge is : %f"), Stimulus.GetAge(), HearingConfig->GetMaxAge());
				}
				//UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());

				//Stimulus.SetExpirationAge(3.f); //1초뒤에 감각 무효화.
			}
			else //감지를 못했을때 (시야만 동작)
			{
			//	/*FActorPerceptionInfo* info = PerceptionComponent->GetActorInfo();
			//	if (info->LastSensedStimuli[0].IsValid())
			//	{
			//	}*/
			// 
			//	if (Stimulus.AgeStimulus(1.f) == false)
			//	{
			//		UE_LOG(LogTemp, Warning, TEXT("Expiration Age is Passed!"));
			//	}
			//	if (Stimulus.GetAge() >= 2.f)
			//	{
			//		if (Stimulus.Type == HearingSenseID)
			//		{
			//			Stimulus.MarkExpired();
			//			Enemy->bHearPlayer = false; //디버깅
			//		}
			//		if (Stimulus.Type == SightSenseID)
			//		{
			//			Stimulus.MarkExpired();
			//			Enemy->bSeePlayer = false; //디버깅
			//		}
			//		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
			//		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.
				Enemy->bSeePlayer = false; //디버깅
				UpdateSeePlayerKey(false);
				//Enemy->bHearPlayer = false; //디버깅
				//Stimulus.MarkExpired();
				TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
				GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.

				UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString());

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
	if (BBComp->GetValueAsBool(bHearPlayerKey))
	{
		UpdateHearPlayerKey(false);
	}
	if (Enemy->bHearPlayer)
	{
		Enemy->bHearPlayer = false;
	}
	UpdatePlayerKey(nullptr);
	UpdateHearLocationKey(FVector::ZeroVector);
	PerceptionComponent->ForgetActor(Target);
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

void AEnemyAIController::UpdatePlayerKey(AActor* Actor)
{
	BBComp->SetValueAsObject(PlayerKey, Actor);
}

void AEnemyAIController::UpdateHearLocationKey(FVector Location)
{
	BBComp->SetValueAsVector(HearLocation, Location);
}

void AEnemyAIController::UpdateSeePlayerKey(bool HasSee)
{
	BBComp->SetValueAsBool(bSeePlayerKey, HasSee);
}
void AEnemyAIController::UpdateHearPlayerKey(bool HasHear)
{
	BBComp->SetValueAsBool(bHearPlayerKey, HasHear);
}

void AEnemyAIController::UpdateAttackableLocationKey(FVector Location)
{
	BBComp->SetValueAsVector(AttackableLocationKey, Location);
}