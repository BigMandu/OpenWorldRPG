// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "Perception/AISenseConfig_Sight.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	UE_LOG(LogTemp, Warning, TEXT("UBTTask_Attack::Attack!"));
	
	BBComp = OwnerComp.GetBlackboardComponent();
	AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AIChar = Cast<AEnemyCharacter>(AICon->GetCharacter());

	AIChar->SetAIStatus(EAIStatus::EAS_Attack);	
	AIChar->SetAimMode(EAimMode::EAM_Aim);
	

	FiringWeapon();

	return Result;
}


void UBTTask_Attack::FiringWeapon()
{
	CheckWeapon();
	AIChar->EquippedWeapon->StartFire();
}


void UBTTask_Attack::CheckWeapon()
{
	AIChar->EquippedWeapon->ChangeSafetyLeverForAI(EWeaponFiringMode::EWFM_SemiAuto);
	AIChar->EquippedWeapon->bLMBDown = false;

	if (AIChar->EquippedWeapon->AmmoLeftInMag <= 0)
	{
		AIChar->EquippedWeapon->Reload();
	}

}