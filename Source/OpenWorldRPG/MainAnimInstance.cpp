// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "MainCharacter.h"
#include "Item/Weapon.h"
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
		bIsinAir = Player->GetCharacterMovement()->IsFalling();

		FVector Speed = Player->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size(); //속도계산

		FRotator Rotate = Player->GetActorRotation();
		Direction = CalculateDirection(Speed, Rotate); //방향 계산

		/**************   Aim Offset **************/
		FRotator ControlRot = Player->GetControlRotation(); //컨트롤러 회전값
		FRotator ActorRot = Player->GetActorRotation(); //캐릭터 회전값

		FRotator Delta = (ControlRot - ActorRot).GetNormalized(); //방향을 구하고
		FRotator Current = FRotator(Pitch, Yaw, 0.f); //현재 pitch, yaw값을 갖고와 현재 회전값을 구하고
		
		FRotator NewRotate = FMath::RInterpTo(Current, Delta, GetWorld()->GetDeltaSeconds(), 15.f); //부드럽게 회전시킨다

		FRotator NewRotYaw = FRotator(0.f, NewRotate.Yaw, 0.f);
		FRotator NewRotPitch = FRotator(NewRotate.Pitch, 0.f, 0.f);
		
		Yaw = NewRotate.Yaw;//NewRotYaw.ClampAxis(90.f); //각각 회전의 제한을 걸어준다.
		Pitch = NewRotate.Pitch;//NewRotPitch.ClampAxis(90.f);
	}
}


void UMainAnimInstance::AnimNotify_StepSound()
{
	StepSound.Broadcast();
}