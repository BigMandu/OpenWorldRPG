// Fill out your copyright notice in the Description page of Project Settings.




#include "EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "EnemyCharacter.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"


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

	//if (BBComp->GetValueAsBool(bSeeEnemyKey) == true)
	//{
	//	//Attack Distance가 된다면
	//	//CanAttack boolean을 세팅해준다, Range가 x이하면 좌우로 무빙한다.
	//	CalcAttackDist(DeltaTime);
	//}
	//else
	//{
	//	SetFocus(nullptr); //식별하지 못했으면 Focus초기화.
	//}
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
	bool bIsEnemy = false;
	if(Target && OwnerActor)
	{
		if(OwnerActor->GetTeamType() != Target->GetTeamType())
		{
			bIsEnemy = true;
		}
	}
	return bIsEnemy;
}


void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	if (Target)
	{
		ABaseCharacter* Char = Cast<ABaseCharacter>(Target);
		//AInteractable* Object = Cast<AInteractable>(Target);

		//Character를 감지했을 경우 Enemy인지 확인한다.
		if (Char && CheckIsEnemy(Char))
		{
			DetectedEnemy(Char, Stimulus);
			//UE_LOG(LogTemp, Warning, TEXT("AI Found Player!"));
		}
		//Object를 감지했을경우는 Interact를 할수있는지 check한다.
		else if(CanInteraction(Target))//Object && CanInteraction(Target))
		{
			DetectedObject(Target, Stimulus);
			//UE_LOG(LogTemp, Warning, TEXT("AI Found Object !"));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Target name : %s"),*Target->GetFName().ToString());
	}
}




/**************************************************/
/***********    Detected Type         ************/
/**************************************************/
//Sight와 Hearing을 구분한다.
void AEnemyAIController::DetectedEnemy(ABaseCharacter* Player, FAIStimulus Stimulus)
{

	FVector DetectedLocation = Stimulus.StimulusLocation;

	FAISenseID HearingSenseID = HearingConfig->GetSenseID();  //Hearing인지
	FAISenseID SightSenseID = SightConfig->GetSenseID(); //Sight인지 따로 처리를 하기 위해  ID를 가져온다.

	const FActorPerceptionInfo* info = GetPerceptionComponent()->GetActorInfo(*Player);
	check(info);


	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetWorldTimerManager().IsTimerActive(EnemyLostTimer)) //타이머 초기화
		{
			GetWorldTimerManager().ClearTimer(EnemyLostTimer);
		}

		//DominantSense는 GetLastStimuliLocation함수에만 쓰이는것 같다. 
		//UE_LOG(LogTemp, Warning, TEXT("DominantSense is %s"), *PerceptionComponent->GetDominantSenseID().Name.ToString());
		

		//Sight를 감지 했을때 (sight로 detect 했다면, hearing은 필요없다.)
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
			//UE_LOG(LogTemp, Warning, TEXT("AI : Sight, Sense ID : %d"), SightSenseID.Index);
			//UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, SightMaxAge is : %f"), Stimulus.GetAge(), SightConfig->GetMaxAge());
		}
		//hearing만 감지 했을때
		else if (Stimulus.Type == HearingSenseID && info->IsSenseActive(SightSenseID) == false)// && Stimulus.Strength >= 1.f) //Hearing을 감지했을때
		{
			//Hearing은 팀 구분없이 식별가능하게 한다.

			OwnerActor->GetCharacterMovement()->MaxWalkSpeed = OwnerActor->MinWalkSpeed;
			//Enemy->bHearPlayer = true; //디버깅
			//Enemy->bSeePlayer = false;
			UpdateBBCompBoolKey(bHearEnemyKey, true);
			UpdateBBCompObjectKey(EnemyKey, Player);
			//UpdateBBCompBoolKey(bSeeEnemyKey, false);

			UpdateBBCompVectorKey(TargetLocationKey, Stimulus.StimulusLocation);
			
			
			
			//UpdateSeePlayerKey(false);
			///UpdateHearPlayerKey(true);
			//UpdateHearLocationKey(Stimulus.StimulusLocation);

			//청각만 들었을때 Hearing의 MaxAge이후 LostTarget을 실행한다.
			EnemyLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
			GetWorldTimerManager().SetTimer(EnemyLostTimer, EnemyLostDelegate, HearingConfig->GetMaxAge(), false);
			//UE_LOG(LogTemp, Warning, TEXT("AI : Hearing, Sense ID : %d"), HearingSenseID.Index);
			//UE_LOG(LogTemp, Warning, TEXT("Stimulus Age is : %f, HearingMaxAge is : %f"), Stimulus.GetAge(), HearingConfig->GetMaxAge());
		}
		//UE_LOG(LogTemp, Warning, TEXT("AI : Detected!! / Detect Location : %s"), *DetectedLocation.ToString());

		//Stimulus.SetExpirationAge(3.f); //1초뒤에 감각 무효화.
	}
	else //감지를 못했을때 (시야만 동작)
	{
		//감지를  실패했을때, 마지막 Target의 Vector값과 Rotation값을 저장한다.
		UpdateBBCompVectorKey(LastTargetLocationKey, DetectedLocation);
		BBComp->SetValueAsRotator(LastTargetRotationKey, Player->GetActorRotation());

		UpdateBBCompBoolKey(bSeeEnemyKey, false);
		
		EnemyLostDelegate = FTimerDelegate::CreateUObject(this, &AEnemyAIController::LostTarget, Player); // Target);
		GetWorldTimerManager().SetTimer(EnemyLostTimer, EnemyLostDelegate, SightConfig->GetMaxAge(), false); //특정초 이후에 LostTarget함수를 호출한다.

		//UE_LOG(LogTemp, Warning, TEXT("AI : Missing!! / Last Location : %s"), *DetectedLocation.ToString());
	
	}
}

void AEnemyAIController::DetectedObject(AActor* Obj, FAIStimulus  Stimulus)//AInteractable* Obj, FAIStimulus Stimulus)
{
	if(Stimulus.WasSuccessfullySensed())
	{
		//AI가 들고있는 무기의 경우 Perception 시야에 지속적으로 감지가 되어 에러가 생긴다.
		//AI가 장착한 무기를 Ignore시킨다.
		//bool bCheckAlreadyHave = OwnerActor->CheckEquippedWeapon(Obj);

		//False면 감지한 Obj가 장착한 무기가 아님.
		//if (bCheckAlreadyHave == false)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AI : Detected Object "));
			//LootBox는 StaticMesh로 지정해야함 -> NavMesh가 적용되지 않아서 Location이 아닌 Object로 이동되게 한다.
			UpdateBBCompObjectKey(ObjectKey, Obj);
		}
		//test
		//if(bCheckAlreadyHave)
		{
		//	LostObject(Obj);
		//	UE_LOG(LogTemp, Warning, TEXT("AI : It's My weapon!!"));
		}
	}
}



void AEnemyAIController::LostTarget(ABaseCharacter* Target) //AActor* Target)
{
	if (GetWorldTimerManager().IsTimerActive(EnemyLostTimer)) //타이머 초기화
	{
		GetWorldTimerManager().ClearTimer(EnemyLostTimer);
	}

	if (BBComp->GetValueAsBool(bHearEnemyKey))
	{
		UpdateBBCompBoolKey(bHearEnemyKey, false);
		UpdateBBCompBoolKey(bNeedToCheckKey, false);

	}

	//새로 추가함.
	GetPerceptionComponent()->ForgetActor(Target); //test

	UpdateBBCompObjectKey(EnemyKey, nullptr);
	UpdateBBCompVectorKey(TargetLocationKey, FVector::ZeroVector);
	UpdateBBCompBoolKey(bWasEngageKey, false);
	OwnerActor->SetAimMode(EAimMode::EAM_NotAim);
}

void AEnemyAIController::LostObject(AActor* InteractActor)
{
	UpdateBBCompObjectKey(ObjectKey, nullptr);

	//Perception에 Sight된 Log를 삭제하고
	GetPerceptionComponent()->ForgetActor(InteractActor);
	
	//Perception IgnoreActor에 추가한다.
	//OwnerActor->PerceptionIgnoreActor.Add(InteractActor);
}

/********************************************************************/

/*
 * Interactable class의 Interaction함수에서 각각의 Object마다 호출.
 * ex) Equipment의 Equip함수에서 호출
*/

//True - Interact가능, false - 불가능
bool AEnemyAIController::CanInteraction(AActor* Object)
{
	bool bCanInteract = true;

	AEquipment* Equip = Cast<AEquipment>(Object);

	//AI가 장착한 장비거나, 다른 캐릭터가 장착중인 장비는 Interaction이 불가능하다.
	//여기서 추가해야할것 -> 다른 AI가 이미 먼저 식별했을 경우의 조건도 추가해야함.
	//Equip에 boolean 변수를 하나 추가해서. OtherCharacter의 occupy선점 여부를 확인 하는거 ㄹ추가하면 될듯?

	
	//if (Equip && Equip->bIsPreOccupied == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::CanInteraction / PreOccupied is false, Check Equip, Owning"));
		//AI가 해당 Equip을 장착하지 않았거나, OwningPlayer가 없다면
		if (Equip && Equip->OwningPlayer != nullptr) //OwnerActor->CheckEquipped(Object) != true) || Equip && Equip->OwningPlayer == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::CanInteraction / Can Interaction"));
			//Interact가 가능하다
			//Equip->bIsPreOccupied = true;
			bCanInteract = false;
		}
	}

	return bCanInteract;
}

//LootBox의 OpenBox함수에서 ItemFarming함수 호출
void AEnemyAIController::ItemFarming(AActor* InteractActor)
{
	//AInteractable* Interact = Cast<AInteractable>(InteractActor);
	if(InteractActor)//if (Interact)
	{
		AContainer* Box = Cast<AContainer>(InteractActor);
		ABaseCharacter* Char = Cast<ABaseCharacter>(InteractActor);
		if (Box)
		{
			ItemChoice(Box->ContainerInventoryComp);
		}

		if (Char)
		{
			//backpack, vest, pocket, secure box 순으로 InvComp를 가져온다. 
			for (int32 iter = 0; iter < 4; ++iter)
			{
				UNewInventoryComponent* InvCmp = Char->GetAllInvComp(iter);
				if (InvCmp != nullptr)
				{
					ItemChoice(InvCmp);
				}
			}
		}

	}
}

void AEnemyAIController::ItemChoice(UNewInventoryComponent* GiverInvComp)
{
	if(GiverInvComp)
	{
		for(auto ItemObj : GiverInvComp->InventoryItems)
		{
			if (ItemObj == nullptr)
			{
				break;
			}


			//지금은 무조건 넣는데, Value값이 얼마 이상이거나, (비교는 ..좀 비용 낭비임)
			// 아니면, 자기가 갖고있는 Inventory가치를 평가해서 
			// 갖고있는 가치가 천원 이상이면 500원 미만짜리는 거들떠 보지도 않는식으로 하면 좋을지도??
			// 
			
			//Pickup에 성공하면 Item이 있던 Component에서 삭제한다.
			if (ItemObj->item->Pickup(OwnerActor))
			{
				GiverInvComp->RemoveItem(ItemObj);
			}
		}
	}
}


/******************  ***********************/


void AEnemyAIController::CalcAttackDist(float DeltaTime)
{
	ABaseCharacter* Main = Cast<ABaseCharacter>(BBComp->GetValueAsObject(EnemyKey));
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

