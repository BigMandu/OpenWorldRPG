// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "EnemyCharacter.h"
#include "MainCharacter.h"
#include "TimerManager.h"
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
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	

	HearingConfig->bUseLoSHearing = true;
	HearingConfig->HearingRange = 1000.f;
	HearingConfig->LoSHearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	
	
	GetPerceptionComponent()->SetDominantSense(SightConfig->Implementation); //우선순위를 Sight로 설정.

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
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

	if (BBComp->GetValueAsBool(bSeeEnemyKey) == true)
	{
		//Attack Distance가 된다면
		//CanAttack boolean을 세팅해준다, Range가 x이하면 좌우로 무빙한다.
		CalcAttackDist(DeltaTime);
	}
	else
	{
		SetFocus(nullptr); //식별하지 못했으면 Focus초기화.
	}
}


//AIController가 해당Enemy를 제어할 수 있도록함.
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerActor = Cast<AEnemyCharacter>(InPawn);
	check(OwnerActor);

	if (OwnerActor)
	{
		const FVector OriginPosition = OwnerActor->GetActorLocation();
		BBComp->InitializeBlackboard(*(OwnerActor->BTAsset->BlackboardAsset)); //Blackboard초기화

		//여긴 귀찮아서 걍 이걸로함
		BBComp->SetValueAsVector(OriginPosKey, OriginPosition); //Spawn위치 저장.
		BBComp->SetValueAsBool(bHasPatrolPointsKey, OwnerActor->bHasPatrolPoints); //Targetpoint 보유 여부를 넘겨줌.
		BBComp->SetValueAsInt(PatrolPointIndexKey, 0); //Targetpoint index를 0으로 세팅한다.
		
		BTComp->StartTree(*(OwnerActor->BTAsset)); //마지막에 StartTree

		SightConfig->SetMaxAge(OwnerActor->SightMaxAge);
		HearingConfig->SetMaxAge(OwnerActor->HearingMaxAge);
	}	
}


//BaseCharacter에 설정한 TeamType을 비교해서 적을 판별한다.
bool AEnemyAIController::CheckIsEnemy(ABaseCharacter* Target)
{
	bool bEnemy = false;
	if(Target && OwnerActor)
	{
		if(OwnerActor->GetTeamType() != Target->GetTeamType())
		{
			bEnemy = true;
		}
	}
	return bEnemy;
}


void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	if (Target)
	{
		ABaseCharacter* Char = Cast<ABaseCharacter>(Target);
		ALootBox* LootBox = Cast<ALootBox>(Target);
		if (Char && CheckIsEnemy(Char))
		{
			DetectedCharacter(Char, Stimulus);
			UE_LOG(LogTemp, Warning, TEXT("AI Found Player!"));
		}

		if(LootBox)
		{
			DetectedLootBox(LootBox, Stimulus);
			UE_LOG(LogTemp, Warning, TEXT("AI Found LootBox !"));
		}
		
	}
}


void AEnemyAIController::LostTarget(ABaseCharacter* Target) //AActor* Target)
{
	if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
	{
		GetWorldTimerManager().ClearTimer(TargetLostTimer);
	}
	if (BBComp->GetValueAsBool(bHearEnemyKey))
	{
		UpdateBBCompBoolKey(bHearEnemyKey, false);
		//UpdateHearPlayerKey(false);
	}

	//디버깅용
	/*if (Enemy->bHearPlayer)
	{
		Enemy->bHearPlayer = false;
	}*/

	UpdateBBCompObjectKey(EnemyKey, nullptr);
	UpdateBBCompVectorKey(TargetLocationKey, FVector::ZeroVector);

	//UpdatePlayerKey(nullptr);
	//UpdateHearLocationKey(FVector::ZeroVector);
	//PerceptionComp->ForgetActor(Target);
	UE_LOG(LogTemp, Warning, TEXT("AI : Target Lost!, Lost Target : %s"), *Target->GetFName().ToString());
}




/**************************************************/
/***********    Detected Type         ************/
/**************************************************/
//Sight와 Hearing을 구분한다.
void AEnemyAIController::DetectedCharacter(ABaseCharacter* Player, FAIStimulus Stimulus)
{

	FVector DetectedLocation = Stimulus.StimulusLocation;

	FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing인지
	FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight인지 따로 처리를 하기 위해  ID를 가져온다.

	const FActorPerceptionInfo* info = GetPerceptionComponent()->GetActorInfo(*Player);
	check(info);


	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //타이머 초기화
		{
			GetWorldTimerManager().ClearTimer(TargetLostTimer);
		}

		//DominantSense는 GetLastStimuliLocation함수에만 쓰이는것 같다. 
		//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());
		

		//Sight를 감지 했을때
		if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight를 감지했을때
		{
			//Sight로 적이라는걸 식별했을때만 실행한다.
			
			OwnerActor->GetCharacterMovement()->MaxWalkSpeed = OwnerActor->MaxWalkSpeed;
			//Enemy->bSeePlayer = true; //디버깅
			//Enemy->bHearPlayer = false; //디버깅

			UpdateBBCompBoolKey(bSeeEnemyKey, true);
			UpdateBBCompBoolKey(bHearEnemyKey, false);
			UpdateBBCompObjectKey(EnemyKey, Player);
			//UpdateSeePlayerKey(true);
			//UpdateHearPlayerKey(false);
			//UpdatePlayerKey(Player); //Player를 Setting해준다.
			UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
			UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		}
		//hearing만 감지 했을때
		else if (Stimulus.Type == HearingSenseID && info->IsSenseActive(SightSenseID) == false)// && Stimulus.Strength >= 1.f) //Hearing을 감지했을때
		{
			//Hearing은 팀 구분없이 식별가능하게 한다.

			OwnerActor->GetCharacterMovement()->MaxWalkSpeed = OwnerActor->MinWalkSpeed;
			//Enemy->bHearPlayer = true; //디버깅
			//Enemy->bSeePlayer = false;
			UpdateBBCompBoolKey(bHearEnemyKey, true);
			UpdateBBCompVectorKey(TargetLocationKey, Stimulus.StimulusLocation);

			UpdateBBCompBoolKey(bSeeEnemyKey, false);
			
			//UpdateSeePlayerKey(false);
			///UpdateHearPlayerKey(true);
			//UpdateHearLocationKey(Stimulus.StimulusLocation);

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
		//Enemy->bSeePlayer = false; //디버깅
		UpdateBBCompBoolKey(bSeeEnemyKey, false);
		//UpdateSeePlayerKey(false);
		//Enemy->bHearPlayer = false; //디버깅

		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.

		UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString());
	
	}
}

void AEnemyAIController::DetectedLootBox(ALootBox* Loot, FAIStimulus Stimulus)
{
	if(Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("AI : Detected LootBox, "));

		UpdateBBCompVectorKey(TargetLocationKey, Stimulus.StimulusLocation);
		//UpdateBBCompVectorKey(FName("TestVector"), Stimulus.StimulusLocation);
		UpdateBBCompObjectKey(TargetKey, Loot);
		
	}
}


/******************  ***********************/

void AEnemyAIController::CalcAttackDist(float DeltaTime)
{
	AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(EnemyKey));
	if (Main)
	{
		//Player를 식별하면 계속해서 Player의 방향으로 회전한다.
		FVector PlayerLo = Main->GetActorLocation();
		FVector EnemyLo = OwnerActor->GetActorLocation();

		FVector RotationVec = (PlayerLo - EnemyLo).GetSafeNormal();
		FRotator NRot = FRotator(0.f, RotationVec.Rotation().Yaw, 0.f);
		OwnerActor->SetActorRotation(NRot); //회전과
		SetFocus(Main); //Focus를 Player를 향하게 한다.
		//SetFocalPoint(PlayerLo); // SetFocalPoint에서 SetFocus로 변경

		float Distance = FVector::Dist(EnemyLo, PlayerLo);
		if (Distance <= (OwnerActor->Range + OwnerActor->Range * 0.1)) //main과 Enemy의 거리가 enemy range내에 있으면 공격 가능.
		{
			UpdateBBCompBoolKey(bCanAttackKey, true);

			//Range가 400이하 -> 나중엔 무기 계열(샷건이나 smg계열만)에 따라 적용 여부 판단.
			if (OwnerActor->Range <= 400.f) //좌우로 움직이는 코드 -> 확률 적용하기.
			{
				FVector EnemyRightLo = OwnerActor->GetActorRightVector();
				if (bUpdateEnemyLo == false) //위치를 한번만 업데이트 하기 위함.
				{
					bUpdateEnemyLo = true;
					EnemyAttackLo = OwnerActor->GetActorLocation(); //이 어택 위치를 AttackMoving()에 넘겨준다.
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
			UpdateBBCompBoolKey(bCanAttackKey, false);
			bUpdateEnemyLo = false;
		}
	}
}

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



/**************************************************************************/
/******************  Black board Key Update Function***********************/
/**************************************************************************/

void AEnemyAIController::UpdateBBCompVectorKey(FName KeyName, FVector Vector)
{
	BBComp->SetValueAsVector(KeyName, Vector);
}
void AEnemyAIController::UpdateBBCompBoolKey(FName KeyName, bool bBool)
{
	BBComp->SetValueAsBool(KeyName, bBool);
}
void AEnemyAIController::UpdateBBCompIntegerKey(FName KeyName, int32 Numb)
{
	BBComp->SetValueAsInt(KeyName, Numb);
}
void AEnemyAIController::UpdateBBCompObjectKey(FName KeyName, AActor* Actor)
{
	BBComp->SetValueAsObject(KeyName, Actor);
}


/*
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

*/
