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

	/**************  AI Perception����  ***************/
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
	
	
	GetPerceptionComponent()->SetDominantSense(SightConfig->Implementation); //�켱������ Sight�� ����.

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
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

	if (BBComp->GetValueAsBool(bSeeEnemyKey) == true)
	{
		//Attack Distance�� �ȴٸ�
		//CanAttack boolean�� �������ش�, Range�� x���ϸ� �¿�� �����Ѵ�.
		CalcAttackDist(DeltaTime);
	}
	else
	{
		SetFocus(nullptr); //�ĺ����� �������� Focus�ʱ�ȭ.
	}
}


//AIController�� �ش�Enemy�� ������ �� �ֵ�����.
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerActor = Cast<AEnemyCharacter>(InPawn);
	check(OwnerActor);

	if (OwnerActor)
	{
		const FVector OriginPosition = OwnerActor->GetActorLocation();
		BBComp->InitializeBlackboard(*(OwnerActor->BTAsset->BlackboardAsset)); //Blackboard�ʱ�ȭ

		//���� �����Ƽ� �� �̰ɷ���
		BBComp->SetValueAsVector(OriginPosKey, OriginPosition); //Spawn��ġ ����.
		BBComp->SetValueAsBool(bHasPatrolPointsKey, OwnerActor->bHasPatrolPoints); //Targetpoint ���� ���θ� �Ѱ���.
		BBComp->SetValueAsInt(PatrolPointIndexKey, 0); //Targetpoint index�� 0���� �����Ѵ�.
		
		BTComp->StartTree(*(OwnerActor->BTAsset)); //�������� StartTree

		SightConfig->SetMaxAge(OwnerActor->SightMaxAge);
		HearingConfig->SetMaxAge(OwnerActor->HearingMaxAge);
	}	
}


//BaseCharacter�� ������ TeamType�� ���ؼ� ���� �Ǻ��Ѵ�.
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
	if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //Ÿ�̸� �ʱ�ȭ
	{
		GetWorldTimerManager().ClearTimer(TargetLostTimer);
	}
	if (BBComp->GetValueAsBool(bHearEnemyKey))
	{
		UpdateBBCompBoolKey(bHearEnemyKey, false);
		//UpdateHearPlayerKey(false);
	}

	//������
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
//Sight�� Hearing�� �����Ѵ�.
void AEnemyAIController::DetectedCharacter(ABaseCharacter* Player, FAIStimulus Stimulus)
{

	FVector DetectedLocation = Stimulus.StimulusLocation;

	FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing����
	FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight���� ���� ó���� �ϱ� ����  ID�� �����´�.

	const FActorPerceptionInfo* info = GetPerceptionComponent()->GetActorInfo(*Player);
	check(info);


	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetWorldTimerManager().IsTimerActive(TargetLostTimer)) //Ÿ�̸� �ʱ�ȭ
		{
			GetWorldTimerManager().ClearTimer(TargetLostTimer);
		}

		//DominantSense�� GetLastStimuliLocation�Լ����� ���̴°� ����. 
		//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());
		

		//Sight�� ���� ������
		if (Stimulus.Type == SightSenseID)// && Stimulus.Strength >= 1.f) //Sight�� ����������
		{
			//Sight�� ���̶�°� �ĺ��������� �����Ѵ�.
			
			OwnerActor->GetCharacterMovement()->MaxWalkSpeed = OwnerActor->MaxWalkSpeed;
			//Enemy->bSeePlayer = true; //�����
			//Enemy->bHearPlayer = false; //�����

			UpdateBBCompBoolKey(bSeeEnemyKey, true);
			UpdateBBCompBoolKey(bHearEnemyKey, false);
			UpdateBBCompObjectKey(EnemyKey, Player);
			//UpdateSeePlayerKey(true);
			//UpdateHearPlayerKey(false);
			//UpdatePlayerKey(Player); //Player�� Setting���ش�.
			UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
			UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		}
		//hearing�� ���� ������
		else if (Stimulus.Type == HearingSenseID && info->IsSenseActive(SightSenseID) == false)// && Stimulus.Strength >= 1.f) //Hearing�� ����������
		{
			//Hearing�� �� ���о��� �ĺ������ϰ� �Ѵ�.

			OwnerActor->GetCharacterMovement()->MaxWalkSpeed = OwnerActor->MinWalkSpeed;
			//Enemy->bHearPlayer = true; //�����
			//Enemy->bSeePlayer = false;
			UpdateBBCompBoolKey(bHearEnemyKey, true);
			UpdateBBCompVectorKey(TargetLocationKey, Stimulus.StimulusLocation);

			UpdateBBCompBoolKey(bSeeEnemyKey, false);
			
			//UpdateSeePlayerKey(false);
			///UpdateHearPlayerKey(true);
			//UpdateHearLocationKey(Stimulus.StimulusLocation);

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
		//Enemy->bSeePlayer = false; //�����
		UpdateBBCompBoolKey(bSeeEnemyKey, false);
		//UpdateSeePlayerKey(false);
		//Enemy->bHearPlayer = false; //�����

		TargetLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
		GetWorldTimerManager().SetTimer(TargetLostTimer, TargetLostDelegate, SightConfig->GetMaxAge(), false); //Ư���� ���Ŀ� LostTarget�Լ��� ȣ���Ѵ�.

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
		//Player�� �ĺ��ϸ� ����ؼ� Player�� �������� ȸ���Ѵ�.
		FVector PlayerLo = Main->GetActorLocation();
		FVector EnemyLo = OwnerActor->GetActorLocation();

		FVector RotationVec = (PlayerLo - EnemyLo).GetSafeNormal();
		FRotator NRot = FRotator(0.f, RotationVec.Rotation().Yaw, 0.f);
		OwnerActor->SetActorRotation(NRot); //ȸ����
		SetFocus(Main); //Focus�� Player�� ���ϰ� �Ѵ�.
		//SetFocalPoint(PlayerLo); // SetFocalPoint���� SetFocus�� ����

		float Distance = FVector::Dist(EnemyLo, PlayerLo);
		if (Distance <= (OwnerActor->Range + OwnerActor->Range * 0.1)) //main�� Enemy�� �Ÿ��� enemy range���� ������ ���� ����.
		{
			UpdateBBCompBoolKey(bCanAttackKey, true);

			//Range�� 400���� -> ���߿� ���� �迭(�����̳� smg�迭��)�� ���� ���� ���� �Ǵ�.
			if (OwnerActor->Range <= 400.f) //�¿�� �����̴� �ڵ� -> Ȯ�� �����ϱ�.
			{
				FVector EnemyRightLo = OwnerActor->GetActorRightVector();
				if (bUpdateEnemyLo == false) //��ġ�� �ѹ��� ������Ʈ �ϱ� ����.
				{
					bUpdateEnemyLo = true;
					EnemyAttackLo = OwnerActor->GetActorLocation(); //�� ���� ��ġ�� AttackMoving()�� �Ѱ��ش�.
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

	//������
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
