// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Player = Cast<AMainCharacter>(Pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr || Player == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Player = Cast<AMainCharacter>(Pawn);
		}
	}

	if (Player)
	{
		FVector Speed = Player->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsinAir = Player->GetCharacterMovement()->IsFalling();
	}
}


void UMainAnimInstance::AnimNotify_StepSound()
{
	StepSound.Broadcast();
}