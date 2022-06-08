// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "Item/EquipmentComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	bUseControllerRotationYaw = false;


	bHasPatrolPoints = false;
	RandomPatrolRadius = 1000.f;
	PatrolAcceptableRadius = 5.f;
	SightMaxAge = 15.f;
	HearingMaxAge = 10.f;

	bSeePlayer = false;
	bHearPlayer = false;

	Range = 1800.f;

	AIControllerClass = AEnemyAIController::StaticClass(); //AIController�� �־��ְ�
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //AIController�� �����ñ⸦ ������.

}

// Called to bind functionality to input
//void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}


// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCharacter::SetAIStatus(EAIStatus Status)
{
	AIStatus = Status;
	switch(AIStatus)
	{
	case EAIStatus::EAS_Patrol:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		break;
	case EAIStatus::EAS_Scout:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		break;
	case EAIStatus::EAS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		break;
	case EAIStatus::EAS_Attack:
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		break;
	case EAIStatus::EAS_Dead:
		break;
	}
}

//�̹� �����ϰ��ִ� ��� �����ϸ� True�� ����, �׿ܴ� false����.
bool AEnemyCharacter::CheckEquipped(AActor* Actor)
{
	if(Equipment)
	{
		for(auto Equipped : Equipment->EquipmentItems)
		{
			if(Actor == Equipped)
			{
				return true;
			}
		}
	}
	return false;
}