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
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/LootBox.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true;



	/**************  AI Perception관련  ***************/
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	//PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 2000.f;

	//단안각 60도, 양안각 120도.
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	////SightConfig->SetMaxAge(15.f); //OnPossess함수에서 다시 변경함.
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	

	HearingConfig->bUseLoSHearing = true;
	HearingConfig->HearingRange = 1000.f;
	HearingConfig->LoSHearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	////HearingConfig->SetMaxAge(15.f); //OnPossess함수에서 다시 변경함.
	////HearingConfig->DetectionByAffiliation.bDetectNeutrals = true; //기본세팅이 Friend로 되어있어서 주석처리 했다.
	////HearingConfig->DetectionByAffiliation.bDetectEnemies = true;

	/************** Behavoir ***********/
	BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	/***********************/
	Alpha = 0.f;
	AttackMVDist = 100.f;
	bUpdateEnemyLo = false;
	EnemyAttackLo = FVector::ZeroVector;
}

void AEnemyAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::DetectedTarget);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//임시로 주석처리
	/*
	if (BBComp->GetValueAsBool(bSeePlayerKey) == true) //player를 식별했으면
	{
		AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(PlayerKey));
		if (Main)
		{
			//Player를 식별하면 계속해서 Player의 방향으로 회전한다.
			FVector PlayerLo = Main->GetActorLocation();
			FVector EnemyLo = Enemy->GetActorLocation();

			FVector RotationVec = (PlayerLo - EnemyLo).GetSafeNormal();
			FRotator NRot = FRotator(0.f, RotationVec.Rotation().Yaw, 0.f);
			Enemy->SetActorRotation(NRot); //회전과
			SetFocus(Main); //Focus를 Player를 향하게 한다.
			//SetFocalPoint(PlayerLo); // SetFocalPoint에서 SetFocus로 변경

			float Distance = FVector::Dist(EnemyLo, PlayerLo);
			if (Distance <= (Enemy->Range + Enemy->Range * 0.1)) //main과 Enemy의 거리가 enemy range내에 있으면 공격 가능.
			{
				BBComp->SetValueAsBool(CanAttackKey, true);

				//Range가 400이하 -> 나중엔 무기 계열(샷건이나 smg계열만)에 따라 적용 여부 판단.
				if(Enemy->Range <= 400.f) //좌우로 움직이는 코드 -> 확률 적용하기.
				{
					FVector EnemyRightLo = Enemy->GetActorRightVector();
					if (bUpdateEnemyLo == false) //위치를 한번만 업데이트 하기 위함.
					{
						bUpdateEnemyLo = true;
						EnemyAttackLo = Enemy->GetActorLocation(); //이 어택 위치를 AttackMoving()에 넘겨준다.
					}
					Alpha += DeltaTime;

					if (Alpha >= 0.3f && EnemyAttackLo != FVector::ZeroVector) //.3 초 지나면
					{
						Alpha = 0.f;

						AttackMVDist *= -1; //좌, 우로 변경하기
						AttackMoving(EnemyAttackLo, EnemyRightLo);
					}
				}
			}
			else //공격이 불가능할때
			{
				BBComp->SetValueAsBool(CanAttackKey, false);
				bUpdateEnemyLo = false;
			}
		}
	}
	else 
	{
		SetFocus(nullptr); //식별하지 못했으면 Focus초기화.
	}
	*/
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
		BBComp->SetValueAsBool(bHasPatrolPointsKey, Enemy->bHasPatrolPoints); //Targetpoint 보유 여부를 넘겨줌.
		BBComp->SetValueAsInt(PatrolPointIndexKey, 0); //Targetpoint index를 0으로 세팅한다.
		
		BTComp->StartTree(*(Enemy->BTAsset)); //마지막에 StartTree


		//Sight, Hearing의 세팅
		//SightConfig->SetMaxAge(Enemy->SightMaxAge);
		//HearingConfig->SetMaxAge(Enemy->HearingMaxAge);

		//PerceptionComp->SetDominantSense(SightConfig->Implementation); //우선순위를 Sight로 설정.
		
	}	
}


void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	

	UE_LOG(LogTemp, Warning, TEXT("AI Detected Target : %s"), *Target->GetFName().ToString());
	if (Target)
	{
		FVector DetectedLocation = Stimulus.StimulusLocation;

		//FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing인지
		//FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight인지 따로 처리를 하기 위해  ID를 가져온다.

		///*const FActorPerceptionInfo* info = PerceptionComp->GetActorInfo(*Target);

		//check(info);
		//if (Stimulus.WasSuccessfullySensed())*/
		//{
		//	if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight를 감지했을때
		//	{
		//		Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MaxWalkSpeed;
		//		Enemy->bSeePlayer = true; //디버깅
		//		Enemy->bHearPlayer = false; //디버깅
		//		/*UpdateSeePlayerKey(true);
		//		UpdateHearPlayerKey(false);
		//		UpdatePlayerKey(Player); *///Player를 Setting해준다.
		//		UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
		//		UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		//	}
		//}

		//임시로 주석처리
		
		AMainCharacter* Player = Cast<AMainCharacter>(Target);
		ALootBox* LootBox = Cast<ALootBox>(Target);
		if (Player)
		{
			//DetectedPlayer(Player, Stimulus);
			UE_LOG(LogTemp, Warning, TEXT("AI Found Player!"));
		}
		else if(LootBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI Found LootBox !"));
		}
		
	}
}

void AEnemyAIController::LostTarget(AMainCharacter* Target) //AActor* Target)
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
	//PerceptionComp->ForgetActor(Target);
	UE_LOG(LogTemp, Warning, TEXT("AI : Target Lost!, Lost Target : %s"), *Target->GetFName().ToString());
}
/**************************************************/
/***********    Detected Type         ************/
/**************************************************/

/*
void AEnemyAIController::DetectedPlayer(AMainCharacter* Player, FAIStimulus Stimulus)
{

	FVector DetectedLocation = Stimulus.StimulusLocation;

	FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing인지
	FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight인지 따로 처리를 하기 위해  ID를 가져온다.

	const FActorPerceptionInfo* info = PerceptionComp->GetActorInfo(*Player);
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
	}

	//Stimulus.WasSuccessfullySensed() ==flase와 같은 역할함.
	//if (info->HasAnyCurrentStimulus() == false)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AI ActorInfo :: has not detected"));
	//}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
		{
			GetWorldTimerManager().ClearTimer(TargetLostTimer);
		}


		//마지막이 아닌, Age가 긴 Sense를 가져온다.
		//for (int32 Sense = 0; Sense < info->LastSensedStimuli.Num(); ++Sense)
		//{
		//	float Age = info->LastSensedStimuli[Sense].GetAge();
		//	if (info->LastSensedStimuli[Sense].WasSuccessfullySensed())
		//	{
		//		FName Senseid;
		//		if (Sense == 0)
		//		{
		//			Senseid = TEXT("Sight");
		//		}
		//		else if (Sense == 1)
		//		{
		//			Senseid = TEXT("Hearing");
		//		}
		//		UE_LOG(LogTemp, Warning, TEXT("LastStimuli Sense is : %d"), Sense);
		//		UE_LOG(LogTemp, Warning, TEXT("LastStimuli Age is : %.2f"), Age);
		//	}
		//}

		//DominantSense는 GetLastStimuliLocation함수에만 쓰이는것 같다. 
		//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());


		if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight를 감지했을때
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MaxWalkSpeed;
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
			Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MinWalkSpeed;
			Enemy->bHearPlayer = true; //디버깅
			Enemy->bSeePlayer = false;
			UpdateSeePlayerKey(false);
			UpdateHearPlayerKey(true);
			UpdateHearLocationKey(Stimulus.StimulusLocation);
			//청각만 들었을때 Hearing의 MaxAge이후 LostTarget을 실행한다.
			TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
			GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, HearingConfig->GetMaxAge(), false);
			UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
			UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, HearingMaxAge is : %f"), Stimulus.GetAge(), HearingConfig->GetMaxAge());
		}
		//UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());

		//Stimulus.SetExpirationAge(3.f); //1초뒤에 감각 무효화.
	}
	else //감지를 못했을때 (시야만 동작)
	{
		//	//FActorPerceptionInfo* info = PerceptionComponent->GetActorInfo();
		//	//if (info->LastSensedStimuli[0].IsValid())
		//	//{
		//	//}
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
		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.

		UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString());

	}
}
*/

/******************  ***********************/

void AEnemyAIController::AttackMoving(const FVector LocationVec, FVector RightVec)
{
	RightVec.X *= AttackMVDist;
	RightVec.Y *= AttackMVDist;

	FVector MovementVec = FVector(RightVec.X + LocationVec.X, RightVec.Y + LocationVec.Y, LocationVec.Z);

	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(MovementVec);
	MoveTo(MoveReq);

	//디버깅용
	{
		UKismetSystemLibrary::DrawDebugLine(this, LocationVec, MovementVec, FLinearColor::Yellow, 0.6f, 2.f);
	}
}

/******************  Black board Key Update Function***********************/

void AEnemyAIController::UpdatePatrolPointIndex(int32 index)
{
	BBComp->SetValueAsInt(PatrolPointIndexKey, index);
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

