// Fill out your copyright notice in the Description page of Project Settings.




#include "EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"
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
#include "EnvironmentQuery/EnvQueryTypes.h"

#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "../MainController.h"
#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/Item/GrenadePDA.h"
#include "DrawDebugHelpers.h"



AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true;

	/**************** CrowdFollowing Comp 관련 ****************/
	if (UCrowdFollowingComponent* CrowdFollowComp = Cast<UCrowdFollowingComponent>(GetComponentByClass(UCrowdFollowingComponent::StaticClass())))
	{
		//CrowdFollowComp->SuspendCrowdSteering(true);
		

	/*	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
		if (CrowdManager)
		{
			CrowdManager->RegisterAgent(this);

			CrowdManager->DrawDebugCollisionSegments(this);
			CrowdManager->DrawDebugCorners(this);
			CrowdManager->DrawDebugNeighbors(this);
			CrowdManager->DrawDebugPath(this);
			
		}*/
	}


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

		if(BBComp && OwnerActor->BTAsset)
		{
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
}

/* Fragmentation Type의 수류탄이 던져진 상태면
* BBComp의 DetectGrenade를 true로 변경한다.
*/
bool AEnemyAIController::IsGrenade(AActor* DetectActor)
{
	if ( ABaseGrenade* Grenade = Cast<ABaseGrenade>(DetectActor) )
	{
		if ( UGrenadePDA* GPDA = Cast< UGrenadePDA>(Grenade->ItemSetting.DataAsset) )
		{
			if ( GPDA->GrenadeType == EGrenadeType::EGT_Fragment && Grenade->bThrow )
			{
				return true;
			}
		}		
	}
	return false;
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

/*
 * Interactable class의 Interaction함수에서 각각의 Object마다 호출.
 * ex) Equipment의 Equip함수에서 호출
*/

//True - Interact가능, false - 불가능
bool AEnemyAIController::CanInteraction(AActor* Object)
{
	bool bCanInteract = true;

	//AI가 장착한 장비거나, 다른 캐릭터가 장착중인 장비는 Interaction이 불가능하다.
	//여기서 추가해야할것 -> 다른 AI가 이미 먼저 식별했을 경우의 조건도 추가해야함.
	//Equip에 boolean 변수를 하나 추가해서. OtherCharacter의 occupy선점 여부를 확인 하는거 ㄹ추가하면 될듯?
	
	//UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::CanInteraction / PreOccupied is false, Check Equip, Owning"));
	//AI가 해당 Equip을 장착하지 않았거나, OwningPlayer가 없다면
	if ( AEquipment* Equip = Cast<AEquipment>(Object))
	{
		//UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::CanInteraction / Can Interaction"));
		if ( Equip->OwningPlayer != nullptr )
		{
			bCanInteract = false;
		}
			
	}
	else if ( ABaseCharacter* BChar = Cast<ABaseCharacter>(Object) )
	{
		if ( BChar->CharacterStatus != ECharacterStatus::EPS_Dead )
		{
			bCanInteract = false;
		}
	}	

	return bCanInteract;
}


void AEnemyAIController::DetectedTarget(AActor* Target, FAIStimulus Stimulus)
{
	if (Target)
	{
		ABaseCharacter* Char = Cast<ABaseCharacter>(Target);
		
		/*중요한 순서대로 배치하자.
		* 1순위 던져진 수류탄
		* 2순위 Target(Enemy)
		* 3순위 Object(장비,무기,Lootbox,,etc)
		*/

		if( IsGrenade(Target))
		{
			DetectedGrenade(Target, Stimulus);
		}
		//Character를 감지했을 경우 Enemy인지 확인한다.
		else if (Char && CheckIsEnemy(Char) && Char->CharacterStatus != ECharacterStatus::EPS_Dead )
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
void AEnemyAIController::DetectedGrenade(AActor* DetectTarget, FAIStimulus Stimulus)
{
	if ( ABaseGrenade* Grenade = Cast<ABaseGrenade>(DetectTarget) )
	{
		//Grenade List에 없다면 추가 단계를 진행한다.
		if (DetectedGrenadeList.Num() == 0 || DetectedGrenadeList.Find(Grenade) == INDEX_NONE )
		{
			UE_LOG(LogTemp, Warning, TEXT("AICon::DetectGrenade // Detect Grenade!!!! "));

			UpdateBBCompBoolKey(bDetectGrenade, true);
			DetectedGrenadeList.Add(Grenade);
			//Grenade가 터졌을때 BB key를 update해주기 위해
			//bind 해준다. (BaseGrenade::EndEffect에서 broadcast 됨.)
			Grenade->OnGrenadeDestroy.AddUFunction(this, FName("GrenadeEffectIsEnd"));

			//위치를 갱신한다.
			CalcGrenadeLocation();
		}
		//이미 detect한걸 재식별 했을 경우 위치만 갱신한다.
		else if ( DetectedGrenadeList.Find(Grenade) != INDEX_NONE)
		{
			CalcGrenadeLocation();
		}
	}
}

void AEnemyAIController::GrenadeEffectIsEnd(ABaseGrenade* ExplodedGrenade)
{
	
	if ( DetectedGrenadeList.Find(ExplodedGrenade) != INDEX_NONE )
	{
		UE_LOG(LogTemp, Warning, TEXT("AICon::GrenadeEffectIsEnd// grenade  was exploded , remove at list"));
		DetectedGrenadeList.Remove(ExplodedGrenade);
		
	}

	//DetectedList가 비어있다면 인식되었던 Grenade가 없으므로
	//detect bool key를 false한다.
	if ( DetectedGrenadeList.Num() <= 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("AICon::GrenadeEffectIsEnd// There is no more Detect grenade"));
		UpdateBBCompBoolKey(bDetectGrenade, false);

		//식별된 수류탄의 위치값도 0.f로 초기화 한다.
		UpdateBBCompVectorKey(DetectGrenadeLocationKey, FVector(0.f));
	}

}


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


			//Car의 Controller를 가져와야함.
			
			
			if ( AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController()))
			{
				if ( PlayerCon->bIsinCar )
				{
					return;
				}
				PlayerCon->AddToListTargetingThisActor(this);
			}

			UpdateBBCompBoolKey(bSeeEnemyKey, true);
			UpdateBBCompBoolKey(bHearEnemyKey, false);
			UpdateBBCompObjectKey(EnemyKey, Player);

			if ( Player->CharacterStatus == ECharacterStatus::EPS_Dead )
			{
				UpdateBBCompObjectKey(EnemyKey, nullptr);
			}
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

		ClearFocus(EAIFocusPriority::Default);
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
	//GetPerceptionComponent()->ForgetActor(Target); //test
	GetPerceptionComponent()->ForgetAll();

	//BBComp의 블랙보드 키 값들을 초기화 한다.
	UpdateBBCompObjectKey(EnemyKey, nullptr);
	UpdateBBCompVectorKey(TargetLocationKey, FVector::ZeroVector);

	UpdateBBCompBoolKey(bWasEngageKey, false);
	UpdateBBCompBoolKey(bCanAttackKey, false);
	UpdateBBCompBoolKey(bMovingBehindTargetKey, false);

	if ( AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController()) )
	{
		PlayerCon->RemoveAtListTargetingThisActor(this);
	}
	OwnerActor->SetAimMode(EAimMode::EAM_NotAim);
}

void AEnemyAIController::LostObject(AActor* InteractActor)
{
	UpdateBBCompObjectKey(ObjectKey, nullptr);

	//Perception에 Sight된 Log를 삭제하고
	//GetPerceptionComponent()->ForgetActor(InteractActor);
	GetPerceptionComponent()->ForgetAll();

	//Perception IgnoreActor에 추가한다.
	//OwnerActor->PerceptionIgnoreActor.Add(InteractActor);
}

/********************************************************************/

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


/* 위치값을 저장하는 이유는
* EQS를 돌리기 위함이다.
*/
void AEnemyAIController::CalcGrenadeLocation()
{
	if(!BBComp ) return;
	if( DetectedGrenadeList.Num() <= 0 ) return;
	
	const FVector AILoc = OwnerActor->GetActorLocation();
	//const FVector SavedGrenadeLoc = BBComp->GetValueAsVector(DetectGrenadeLocationKey);
	ABaseGrenade* SavedGrenade = Cast<ABaseGrenade>(BBComp->GetValueAsObject(DetectedGrenadeKey));
	FVector SavedGrenadeLoc = FVector(0.f);

	ABaseGrenade* ShortDistGrenade = nullptr;

	//이미 Detect된 Grenade가 있었다면 비교를 위해 저장해둔다.
	if(SavedGrenade )
	{
		SavedGrenadeLoc = SavedGrenade->GetActorLocation();
		ShortDistGrenade = SavedGrenade;
	}
	//저장된 Grenade가 없다면
	else
	{
		ShortDistGrenade = DetectedGrenadeList[ 0 ].Get();
		SavedGrenadeLoc = ShortDistGrenade->GetActorLocation();
		
	}

	//저장된 GrenadeLocation과 AI와 거리를 측정한 값을 저장해둔다.
	const float SavedDist = FVector::Dist(SavedGrenadeLoc, AILoc);
	

	//기저장된 Dist를 ShortestDist에 대입해 이 값을 첫 시작점으로 잡는다.
	float ShortestDist = SavedDist;

	for ( auto Grenade : DetectedGrenadeList )
	{
		if ( Grenade.IsValid() )
		{
			//식별된GL에 있는 Grenade의 위치와 AI와 거리를 측정한다.
			const FVector DetectGrenadeLoc = Grenade.Get()->GetActorLocation();
			const float NewDist = FVector::Dist(DetectGrenadeLoc, AILoc);

			//ShortestDist보다 짧은 Dist면 저장하면서 loop를 돈다.
			if ( NewDist < ShortestDist )
			{
				ShortestDist = NewDist;
				ShortDistGrenade = Grenade.Get();
			}
		}
	
	}

	DrawDebugSphere(GetWorld(), ShortDistGrenade->GetActorLocation(), 10.f,8,FColor::Green,false, 5.f);
	//loop가 끝나면 ShortDistGrenade의 위치값을 BBComp에 넘겨준다.
	//UpdateBBCompVectorKey(DetectGrenadeLocationKey, ShortDistGrenade->GetActorLocation());
	UpdateBBCompObjectKey(DetectedGrenadeKey, ShortDistGrenade);
	
	
	
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

/*SetFocus || SetFocalpoint를 이용해  AI가 Targeting을 할때 부드러운 회전을 하기 위함이다.*/
void AEnemyAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	//bUpdatePawn은 false로 넘겨 부모 클래스의 아래 if(bUpdatePawn)부분을 실행하지 않도록 한다.
	Super::UpdateControlRotation(DeltaTime,false);

	if ( bUpdatePawn )
	{
		APawn* const MyPawn = GetPawn();
		if(MyPawn == nullptr ) return;
		const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

		//AI Controller의 회전은 이미 Focus Location으로 향해 있기 때문에 목적 Rot은 ControlRotation으로 한다.
		//ControlRotation의 값은 부모 클래스의 이 함수에서 이미 진행을 한 상태임.
		const FRotator InterpRot = FMath::RInterpTo(MyPawn->GetActorRotation(),GetControlRotation(),DeltaTime,30.f);

		if ( CurrentPawnRotation.Equals(InterpRot, 1e-3f) == false )
		{
			MyPawn->FaceRotation(InterpRot, DeltaTime);
		}
	}
}

//AAIController에 있는 함수를 override함.
//Super::를 호출하지 않는다.
//Focus를 Actor에 줄경우, 해당 Actor의 Head에 focus를 하기 위함.
FVector AEnemyAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	FVector FocusLocation;
	if ( const ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor) )
	{
		FocusLocation = BChar->GetMesh()->GetSocketLocation(OwnerActor->HeadSocketName);
	}
	else
	{
		FocusLocation = Actor->GetActorLocation();
	}

	return Actor != nullptr ? FocusLocation : FAISystem::InvalidLocation;
}

void AEnemyAIController::HandleEQSResult(TSharedPtr<FEnvQueryResult> result)
{
	//추후에 진행.
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

