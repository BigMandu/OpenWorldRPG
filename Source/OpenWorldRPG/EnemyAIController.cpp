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



	/**************  AI Perception����  ***************/
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	//PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 2000.f;

	//�ܾȰ� 60��, ��Ȱ� 120��.
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	////SightConfig->SetMaxAge(15.f); //OnPossess�Լ����� �ٽ� ������.
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	

	HearingConfig->bUseLoSHearing = true;
	HearingConfig->HearingRange = 1000.f;
	HearingConfig->LoSHearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	////HearingConfig->SetMaxAge(15.f); //OnPossess�Լ����� �ٽ� ������.
	////HearingConfig->DetectionByAffiliation.bDetectNeutrals = true; //�⺻������ Friend�� �Ǿ��־ �ּ�ó�� �ߴ�.
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

	//�ӽ÷� �ּ�ó��
	/*
	if (BBComp->GetValueAsBool(bSeePlayerKey) == true) //player�� �ĺ�������
	{
		AMainCharacter* Main = Cast<AMainCharacter>(BBComp->GetValueAsObject(PlayerKey));
		if (Main)
		{
			//Player�� �ĺ��ϸ� ����ؼ� Player�� �������� ȸ���Ѵ�.
			FVector PlayerLo = Main->GetActorLocation();
			FVector EnemyLo = Enemy->GetActorLocation();

			FVector RotationVec = (PlayerLo - EnemyLo).GetSafeNormal();
			FRotator NRot = FRotator(0.f, RotationVec.Rotation().Yaw, 0.f);
			Enemy->SetActorRotation(NRot); //ȸ����
			SetFocus(Main); //Focus�� Player�� ���ϰ� �Ѵ�.
			//SetFocalPoint(PlayerLo); // SetFocalPoint���� SetFocus�� ����

			float Distance = FVector::Dist(EnemyLo, PlayerLo);
			if (Distance <= (Enemy->Range + Enemy->Range * 0.1)) //main�� Enemy�� �Ÿ��� enemy range���� ������ ���� ����.
			{
				BBComp->SetValueAsBool(CanAttackKey, true);

				//Range�� 400���� -> ���߿� ���� �迭(�����̳� smg�迭��)�� ���� ���� ���� �Ǵ�.
				if(Enemy->Range <= 400.f) //�¿�� �����̴� �ڵ� -> Ȯ�� �����ϱ�.
				{
					FVector EnemyRightLo = Enemy->GetActorRightVector();
					if (bUpdateEnemyLo == false) //��ġ�� �ѹ��� ������Ʈ �ϱ� ����.
					{
						bUpdateEnemyLo = true;
						EnemyAttackLo = Enemy->GetActorLocation(); //�� ���� ��ġ�� AttackMoving()�� �Ѱ��ش�.
					}
					Alpha += DeltaTime;

					if (Alpha >= 0.3f && EnemyAttackLo != FVector::ZeroVector) //.3 �� ������
					{
						Alpha = 0.f;

						AttackMVDist *= -1; //��, ��� �����ϱ�
						AttackMoving(EnemyAttackLo, EnemyRightLo);
					}
				}
			}
			else //������ �Ұ����Ҷ�
			{
				BBComp->SetValueAsBool(CanAttackKey, false);
				bUpdateEnemyLo = false;
			}
		}
	}
	else 
	{
		SetFocus(nullptr); //�ĺ����� �������� Focus�ʱ�ȭ.
	}
	*/
}

void AEnemyAIController::OnPossess(APawn* InPawn) //AIController�� �ش�Enemy�� ������ �� �ֵ�����.
{
	Super::OnPossess(InPawn);

	Enemy = Cast<AEnemyCharacter>(InPawn);
	check(Enemy);

	if (Enemy)
	{
		const FVector OriginPosition = Enemy->GetActorLocation();
		BBComp->InitializeBlackboard(*(Enemy->BTAsset->BlackboardAsset)); //Blackboard�ʱ�ȭ
		BBComp->SetValueAsVector(OriginPosKey, OriginPosition); //Spawn��ġ ����.
		BBComp->SetValueAsBool(bHasPatrolPointsKey, Enemy->bHasPatrolPoints); //Targetpoint ���� ���θ� �Ѱ���.
		BBComp->SetValueAsInt(PatrolPointIndexKey, 0); //Targetpoint index�� 0���� �����Ѵ�.
		
		BTComp->StartTree(*(Enemy->BTAsset)); //�������� StartTree


		//Sight, Hearing�� ����
		//SightConfig->SetMaxAge(Enemy->SightMaxAge);
		//HearingConfig->SetMaxAge(Enemy->HearingMaxAge);

		//PerceptionComp->SetDominantSense(SightConfig->Implementation); //�켱������ Sight�� ����.
		
	}	
}


void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	

	UE_LOG(LogTemp, Warning, TEXT("AI Detected Target : %s"), *Target->GetFName().ToString());
	if (Target)
	{
		FVector DetectedLocation = Stimulus.StimulusLocation;

		//FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing����
		//FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight���� ���� ó���� �ϱ� ����  ID�� �����´�.

		///*const FActorPerceptionInfo* info = PerceptionComp->GetActorInfo(*Target);

		//check(info);
		//if (Stimulus.WasSuccessfullySensed())*/
		//{
		//	if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight�� ����������
		//	{
		//		Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MaxWalkSpeed;
		//		Enemy->bSeePlayer = true; //�����
		//		Enemy->bHearPlayer = false; //�����
		//		/*UpdateSeePlayerKey(true);
		//		UpdateHearPlayerKey(false);
		//		UpdatePlayerKey(Player); *///Player�� Setting���ش�.
		//		UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
		//		UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		//	}
		//}

		//�ӽ÷� �ּ�ó��
		
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
	if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //Ÿ�̸� �ʱ�ȭ
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

	FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing����
	FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight���� ���� ó���� �ϱ� ����  ID�� �����´�.

	const FActorPerceptionInfo* info = PerceptionComp->GetActorInfo(*Player);
	check(info);

	//LastSensedStimuli�� �̿��� ���.
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

	//Stimulus.WasSuccessfullySensed() ==flase�� ���� ������.
	//if (info->HasAnyCurrentStimulus() == false)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AI ActorInfo :: has not detected"));
	//}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //Ÿ�̸� �ʱ�ȭ
		{
			GetWorldTimerManager().ClearTimer(TargetLostTimer);
		}


		//�������� �ƴ�, Age�� �� Sense�� �����´�.
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

		//DominantSense�� GetLastStimuliLocation�Լ����� ���̴°� ����. 
		//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());


		if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight�� ����������
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MaxWalkSpeed;
			Enemy->bSeePlayer = true; //�����
			Enemy->bHearPlayer = false; //�����
			UpdateSeePlayerKey(true);
			UpdateHearPlayerKey(false);
			UpdatePlayerKey(Player); //Player�� Setting���ش�.
			UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
			UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		}
		else if (Stimulus.Type == HearingSenseID && info->IsSenseActive(SightSenseID) == false)// && Stimulus.Strength >= 1.f) //Hearing�� ����������
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->MinWalkSpeed;
			Enemy->bHearPlayer = true; //�����
			Enemy->bSeePlayer = false;
			UpdateSeePlayerKey(false);
			UpdateHearPlayerKey(true);
			UpdateHearLocationKey(Stimulus.StimulusLocation);
			//û���� ������� Hearing�� MaxAge���� LostTarget�� �����Ѵ�.
			TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
			GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, HearingConfig->GetMaxAge(), false);
			UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
			UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, HearingMaxAge is : %f"), Stimulus.GetAge(), HearingConfig->GetMaxAge());
		}
		//UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());

		//Stimulus.SetExpirationAge(3.f); //1�ʵڿ� ���� ��ȿȭ.
	}
	else //������ �������� (�þ߸� ����)
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
		//			Enemy->bHearPlayer = false; //�����
		//		}
		//		if (Stimulus.Type == SightSenseID)
		//		{
		//			Stimulus.MarkExpired();
		//			Enemy->bSeePlayer = false; //�����
		//		}
		//		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Target);
		//		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //Ư���� ���Ŀ� LostTarget�Լ��� ȣ���Ѵ�.
		Enemy->bSeePlayer = false; //�����
		UpdateSeePlayerKey(false);
		//Enemy->bHearPlayer = false; //�����
		//Stimulus.MarkExpired();
		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //Ư���� ���Ŀ� LostTarget�Լ��� ȣ���Ѵ�.

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

	//������
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

