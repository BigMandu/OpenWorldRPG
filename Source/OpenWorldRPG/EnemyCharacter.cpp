// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	bUseControllerRotationYaw = false;

	bHasPatrolPoints = false;
	RandomPatrolRadius = 1000.f;
	PatrolAcceptableRadius = 5.f;
	SightMaxAge = 15.f;
	HearingMaxAge = 10.f;

	AIControllerClass = AEnemyAIController::StaticClass(); //AIController를 넣어주고
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //AIController의 소유시기를 정해줌.

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


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


