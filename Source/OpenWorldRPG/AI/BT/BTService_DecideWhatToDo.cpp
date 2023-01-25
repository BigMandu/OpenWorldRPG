// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/BT/BTService_DecideWhatToDo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/AI/EnemyCharacter.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"

#include "OpenWorldRPG/GameData/StatManagementComponent.h"



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

	/*�������� ���Ⱑ ���� ��� */
	if (AIChar->EquippedWeapon == nullptr)
	{

		BBComp->SetValueAsBool(AICon->bNoWeaponKey,true);
		//bNoWeapon = true;
	}


	/*����� ������, źâ�� Ư�� ���� ������ ��� (1źâ ���� ����)  �Ǵ� Ammo�� �ƿ� ���� ���	*/
	if (bNoWeapon == false)
	{
		int32 APM = AIChar->EquippedWeapon->WeaponDataAsset->WeaponStat.AmmoPerMag;
		if (AIChar->GetTotalNumberofSameTypeAmmo() < APM / 2)
		{
			if (AIChar->GetTotalNumberofSameTypeAmmo() <= 0)
			{
				//�ٸ� ���⿡�� ammo�� ������ bNoAmmo�� �����Ѵ�.
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




	/* ü���� ���� ����(10 �ۼ�Ʈ ����)�� ��� */
	if (AIChar->StatManagementComponent->GetHealthRatio() <= 10.f)
	{
		BBComp->SetValueAsBool(AICon->bLowHPKey, true);
		//bLowHP = true;
	}



	/* �����ϴ� ���̾��� ���*/
	if (BBComp->GetValueAsBool(AICon->bCanAttackKey))
	{
		BBComp->SetValueAsBool(AICon->bWasEngageKey, true);
		//bWasEngage = true;
		
	}


	/* �� AI�� Target�� �þ� �ȿ� �ִ� ���*/
	if (!BBComp->GetValueAsBool(AICon->bHearEnemyKey))
	{
		BBComp->SetValueAsBool(AICon->bInEnemyFOVKey, IsThisAIinTargetFOV());
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


//DecisionValue�� ���� �б⸦ �Ѵ�.
void UBTService_DecideWhatToDo::DecisionBranch()//UBehaviorTreeComponent& OwnerComp)
{
	// 
	// 
	//1. ���� ��� ���� ź��, ü��, ����  ���� ��� -> ����
	//2. ������ ź�� �Ǵ� ü���� ��������� -> ����
	//3. (�� AI����)EnemyChar�� Fov�̳��� �ְ� ź��, ü��, ����  ��� �ִ� ��� -> ����
	//4. (�� AI����)EnemyChar�� Fov�̳��� ���� ź��, ü��, ����  ��� �ִ� ��� -> �� ���ư��� ����

	
	
	// ź��, ���Ⱑ ���� ��� ������ ������ ���ϰ� Looting�� �Ѵ�.
	//BBComp->GetValueAsBool()
	if (bNoAmmo || bNoWeapon)
	{
#if BTSDEBUG 
		UE_LOG(LogTemp, Warning, TEXT("UBTService_DecideWhatToDo :: Run and Farming"));
#endif
		AIItemFarming();
		return;
	}




	/* ���� ���̾�����, ü���� ���� ���ϰų� ź���� ���� ���� ������ ���
	*  -> ���������� ���� ������ �ϰų� �������°� �����Ѵ�. */
	if (bWasEngage)
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
			UE_LOG(LogTemp, Warning, TEXT("WasEngage && low hp, low ammo"));
			//Decide Closed battle  OR  Run
		}
		else
		{
			AIOpenFire();
			
			UE_LOG(LogTemp, Warning, TEXT("WasEngage"));
			//Keep Battle
		}
	}
	else if(bInEnemyFOV)
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
			UE_LOG(LogTemp, Warning, TEXT("In Enemy FOV && low hp, low ammo"));
			//Decide Closed battle  OR  Run
		}
		else
		{
			//Keep Battle
			AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey, true);
			UE_LOG(LogTemp, Warning, TEXT("In Enemy FOV"));
			
		}
	}
	else if (BBComp->GetValueAsBool(AICon->bHearEnemyKey))
	{
		if (!bNoAmmo && (bLowHP || bLowAmmo))
		{
			//Decide Closed battle  OR  Run
			UE_LOG(LogTemp, Warning, TEXT("bOnlyDetectHearing && low hp, low ammo"));
		}
		else
		{
			//AICon->UpdateBBCompBoolKey(AICon->bNeedToCheckKey,true);
			UE_LOG(LogTemp, Warning, TEXT("bOnlyDetectHearing"));
			//Keep Battle
		}
	}
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


/* ���� �� AI�� Target (Player || Enemy AI)�� FOV���� �ִ����� �����Ѵ�. */
bool UBTService_DecideWhatToDo::IsThisAIinTargetFOV()//UBlackboardComponent* BBComp, AEnemyAIController* OwnerAICon, AEnemyCharacter* OwnerAI)
{
	bool bInFOV = false;
	float TargetHalfFov = 0.f;
	FVector TargetLocation;
	FVector TargetFowVec;
	AMainCharacter* EnemyPlayer = Cast<AMainCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));
	AEnemyCharacter* EnemyAI = Cast<AEnemyCharacter>(BBComp->GetValueAsObject(AICon->EnemyKey));

	if (EnemyPlayer)
	{
		TargetLocation = EnemyPlayer->GetActorLocation();
		TargetFowVec = EnemyPlayer->GetActorForwardVector();
		TargetHalfFov = (Cast<AMainController>(EnemyPlayer->GetController())->PlayerCameraManager->GetFOVAngle() / 2.f);
		UE_LOG(LogTemp, Warning, TEXT("targetfov : %f"), TargetHalfFov);
	}
	else if (EnemyAI)
	{
		TargetLocation = EnemyAI->GetActorLocation();
		TargetFowVec = EnemyAI->GetActorForwardVector();
		//�þ߰� �ҷ��;���.Perception ����
	}


	if (TargetHalfFov != 0.f && TargetLocation != FVector(0.f))
	{
		FVector DirTowardThisAIVec = (TargetLocation - AIChar->GetActorLocation()).GetSafeNormal();

		float DotCalc = FVector::DotProduct(TargetFowVec, DirTowardThisAIVec);
		if (DotCalc > UKismetMathLibrary::Cos(TargetHalfFov))
		{
			bInFOV = false;
		}
		else
		{
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
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
	}

	AICon->UpdateBBCompBoolKey(AICon->bTryFindObejctKey, true);

	AICon->UpdateBBCompBoolKey(AICon->bCanAttackKey, false);
	AICon->UpdateBBCompBoolKey(AICon->bWasEngageKey, false);
	
}
