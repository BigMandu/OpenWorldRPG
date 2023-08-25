// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	bUseControllerRotationYaw = false;

	//AICon에서 CrowdFollowingComponent를 사용하기 때문에, RVO Avoidance algorithm을 비활성화 해야한다.
	//안그럼 CrowdFollowingComponent가 적용되지 않음.
	//그런데 키는게 더 좋은데? 걍 키도록 함.
	GetCharacterMovement()->bUseRVOAvoidance = true;//false;

	bHasPatrolPoints = false;
	RandomPatrolRadius = 1000.f;
	PatrolAcceptableRadius = 5.f;
	SightMaxAge = 15.f;
	HearingMaxAge = 10.f;

	bSeePlayer = false;
	bHearPlayer = false;

	Range = 1800.f;

	AIControllerClass = AEnemyAIController::StaticClass(); //AIController를 넣어주고
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //AIController의 소유시기를 정해줌.
	SetAIStatus(EAIStatus::EAS_Normal);


	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

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
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
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

//이미 장착하고있는 장비를 인지하면 True를 리턴, 그외는 false리턴.
bool AEnemyCharacter::CheckEquipped(AActor* Actor)
{
	if(Equipment)
	{
		for(auto Equipped : Equipment->EquipmentItems)
		{
			//if(Actor == Equipped->item)
			{
				return true;
			}
		}
	}
	return false;
}