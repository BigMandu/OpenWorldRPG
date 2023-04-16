// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTService_DecideWhatToDo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISenseConfig_Sight.h"

#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"

#include "OpenWorldRPG/GameData/StatManagementComponent.h"


#define DECISION_BRANCH_DEBUG 0
#define BTSDEBUG 1

UBTService_DecideWhatToDo::UBTService_DecideWhatToDo()
{
	NodeName = TEXT("DecideWhatToDo");
}

void UBTService_DecideWhatToDo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	BBComp = OwnerComp.GetBlackboardComponent();
	AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());
	if (!BBComp || !AIChar || !AICon) return;

	SetDecisionValue();
	GetDecisionValue();
	DecisionBranch();//OwnerComp);
	
}


void UBTService_DecideWhatToDo::SetDecisionValue()//UBehaviorTreeComponent & OwnerComp)
{

	/*if (BBComp->GetValueAsBool(AICon->bHearEnemyKey))
	{
		bOnlyDetectHearing = true;
	}*/

	/*장착중인 무기가 없는 경우 */
	if (AIChar->EquippedWeapon == nullptr)
	{

		BBComp->SetValueAsBool(AICon->bNoWeaponKey,true);
		//bNoWeapon = true;
	}


	/*무기는 있으나, 탄창이 특정 갯수 이하인 경우 (1탄창 절반 이하)  또는 Ammo가 아예 없는 경우	*/
	if (bNoWeapon == false)
	{
		int32 APM = AIChar->EquippedWeapon->WeaponDataAsset->WeaponStat.AmmoPerMag;
		if (AIChar->GetTotalNumberofSameTypeAmmo() < APM / 2)
		{
			if (AIChar->GetTotalNumberofSameTypeAmmo() <= 0)
			{
				//다른 무기에도 ammo가 없으면 bNoAmmo를 세팅한다.
				if (HasOtherWeapon() == false && bNoAmmo != false)
				{
					BBComp->SetValueAsBool(AICon->bOutOfAmmoKey, true);
					//bNoAmmo = true;
				}
			}
			else
			{
				BBComp->SetValueAsBool(AICon->bLowAmmoKey, true);
				//bLowAmmo = true;
			}
		}
	}




	/* 체력이 일정 이하(10 퍼센트 이하)인 경우 */
	if (AIChar->StatManagementComponent->GetHealthRatio() <= 10.f)
	{
		BBComp->SetValueAsBool(AICon->bLowHPKey, true);
		//bLowHP = true;
	}



	/* 교전하던 중이었던 경우*/
	if (BBComp->GetValueAsBool(AICon->bCanAttackKey))
	{
		BBComp->SetValueAsBool(AICon->bWasEngageKey, true);
		//bWasEngage = true;
		
	}


	/* 이 AI가 Target의 시야 안에 있는 경우*/
	if (!BBComp->GetValueAsBool(AICon->bHearEnemyKey))
	{
		bool bInFov = IsThisAIinTargetFOV();
		BBComp->SetValueAsBool(AICon->bInEnemyFOVKey, bInFov);
		if(bInFov )
		{
			AICon->UpdateBBCompBoolKey(AICon->bMovingBehindTargetKey, true);
		}
		else
		{
			AICon->UpdateBBCompBoolKey(AICon->bMovingBehindTargetKey, false);
		}
		//bInEnemyFOV = IsThisAIinTargetFOV();//(BBComp, AICon, AIChar);
	}
}

void UBTService_DecideWhatToDo::GetDecisionValue()
{
	bNoAmmo = BBComp->GetValueAsBool(AICon->bOutOfAmmoKey);
	bNoWeapon = BBComp->GetValueAsBool(AICon->bNoWeaponKey);
	bLowHP = BBComp->GetValueAsBool(AICon->bLowHPKey);
	bLowAmmo = BBComp->GetValueAsBool(AICon->bLowAmmoKey);
	bInEnemyFOV = BBComp->GetValueAsBool(AICon->bInEnemyFOVKey);
	bWasEngage = BBComp->GetValueAsBool(AICon->bWasEngageKey);
}


//DecisionValue를 토대로 분기를 한다.
void UBTService_DecideWhatToDo::DecisionBranch()//UBehaviorTreeComponent& OwnerComp)
{
	// 
	// 
	//1. 교전 상관 없이 탄약, 체력, 무기  없는 경우 -> 도주
	//2. 교전중 탄약 또는 체력이 떨어진경우 -> 도주
	//3. (이 AI기준)EnemyChar의 Fov이내에 있고 탄약, 체력, 무기  충분 있는 경우 -> 교전
	//4. (이 AI기준)EnemyChar의 Fov이내에 없고 탄약, 체력, 무기  충분 있는 경우 -> 뒤 돌아가서 교전

	
	
	// 탄약, 무기가 없는 경우 무조건 교전을 피하고 Looting을 한다.
	//BBComp->GetValueAsBool()
	if (bNoAmmo || bNoWeapon)
	{
#if DECISION_BRANCH_DEBUG
		UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo :: Run and Farming"));
#endif
		AIItemFarming();
		return;
	}




	/* 교전 중이었으나, 체력이 일정 이하거나 탄약이 일정 개수 이하인 경우
	*  -> 랜덤값으로 근접 교전을 하거나 도망가는걸 결정한다. */
	if (bWasEngage)
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch/ WasEngage && low hp, low ammo"));
#endif
			//Decide Closed battle  OR  Run
		}
		else
		{
			AIOpenFire();
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch / WasEngage"));
#endif
			
			//Keep Battle
		}
	}
	//Target이 되는 Character의 FOV값에 현재 이 AI가 있는 경우(즉, Target이 이 AI를 식별 했을 경우)
	else if(bInEnemyFOV)
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch / In Enemy FOV && low hp, low ammo"));
#endif
			
			//Decide Closed battle  OR  Run
		}
		else
		{
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch /In Enemy FOV"));
#endif
			//Keep Battle
			AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey, true);
			
			
		}
	}
	else if (BBComp->GetValueAsBool(AICon->bHearEnemyKey))
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch /bOnlyDetectHearing && low hp, low ammo"));
#endif
			//Decide Closed battle  OR  Run
			
		}
		else
		{
#if DECISION_BRANCH_DEBUG
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch / bOnlyDetectHearing"));
#endif
			//AICon->UpdateBBCompBoolKey(AICon->bNeedToCheckKey,true);
			
			//Keep Battle
		}
	}
	else if (bInEnemyFOV == false)
	{
#if DECISION_BRANCH_DEBUG
		UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::DecisionBranch / bInEnemyFOV is false"));
#endif
	
	
	}
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


/* 현재 이 AI가 Target (Player || Enemy AI)의 FOV내에 있는지를 리턴한다. */
bool UBTService_DecideWhatToDo::IsThisAIinTargetFOV()//UBlackboardComponent* BBComp, AEnemyAIController* OwnerAICon, AEnemyCharacter* OwnerAI)
{
	bool bInFOV = false;
	float TargetHalfFov = 0.f;
	FVector TargetLocation;
	FVector TargetFowVec;
	AMainCharacter* EnemyPlayer = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	AEnemyCharacter* EnemyAI = Cast<AEnemyCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));

	//시야각은 절반으로 구한다.
	if (EnemyPlayer)
	{
		TargetLocation = EnemyPlayer->GetActorLocation();
		TargetFowVec = EnemyPlayer->GetActorForwardVector();
		TargetHalfFov = (Cast<AMainController>(EnemyPlayer->GetController())->PlayerCameraManager->GetFOVAngle() / 2.f);
		//UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::IsThisAIinTargetFOV /targetfov : %f"), TargetHalfFov);
	}
	//If, Enemy Character is AI, branch this if statement
	else if (EnemyAI)
	{
		TargetLocation = EnemyAI->GetActorLocation();
		TargetFowVec = EnemyAI->GetActorForwardVector();
		TargetHalfFov = Cast<AEnemyAIController>(EnemyAI->GetController())->SightConfig->PeripheralVisionAngleDegrees;
	}


	if (TargetHalfFov != 0.f && TargetLocation != FVector(0.f))
	{
		//AI를 기준으로 Target의 방향을 구한다.
		FVector DirTowardThisAIVec = (TargetLocation - AIChar->GetActorLocation()).GetSafeNormal();

		//AI에서의 Target 방향과 , Target의 ForwardVector를 내적하여 사이의 각을 구한다.
		float DotCalc = FVector::DotProduct(TargetFowVec, DirTowardThisAIVec);

		//Target의 시야각을 cos해서 바로 위에서 구한 타겟-AI간의 각도를 비교한다.
		//cos는 -1~1사이의 값을 가지고 두 값이 같은 방향에 있다면 1, 다르면 -1가 되기때문에
		//Target의 시야각이 사이각보다 크다면 볼 수 있는것.
		if (DotCalc > UKismetMathLibrary::Cos(TargetHalfFov))
		{
			bInFOV = false;
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::IsThisAIinTargetFOV /Out FOV"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo::IsThisAIinTargetFOV /InFOV"));
			bInFOV = true;
		}
	}

	return bInFOV;
}

bool UBTService_DecideWhatToDo::HasOtherWeapon()//AEnemyCharacter* OwnerAI)
{
	bool bChangeWeapon = false;

	for (int32 i = 1; i < 4; ++i)
	{
		bChangeWeapon = AIChar->ChangeWeapon(i);
		if (bChangeWeapon)
		{
			if (AIChar->GetTotalNumberofSameTypeAmmo() <= 0)
			{
				bChangeWeapon = false;
				break;
			}

		}
	}

	return bChangeWeapon;
}


void UBTService_DecideWhatToDo::AIOpenFire()
{
	ABaseCharacter* Target = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	//AICon->SetFocus(Target);
	AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey, true);
}


void UBTService_DecideWhatToDo::AIItemFarming()
{
	if (AICon->GetFocusActor() != nullptr)
	{
		AICon->ClearFocus(EAIFocusPriority::Default);
	}

	AICon->UpdateBBCompBoolKey(AICon->bTryFindObejctKey, true);

	AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey, false);
	AICon->UpdateBBCompBoolKey(AICon->bWasEngageKey, false);
	
}
