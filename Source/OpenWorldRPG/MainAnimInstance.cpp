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
		MovementSpeed = LateralSpeed.Size(); //�ӵ����

		FRotator Rotate = Player->GetActorRotation();
		Direction = CalculateDirection(Speed, Rotate); //���� ���

		/**************   Aim Offset **************/
		FRotator ControlRot = Player->GetControlRotation(); //��Ʈ�ѷ� ȸ����
		FRotator ActorRot = Player->GetActorRotation(); //ĳ���� ȸ����

		FRotator Delta = (ControlRot - ActorRot).GetNormalized(); //������ ���ϰ�
		FRotator Current = FRotator(Pitch, Yaw, 0.f); //���� pitch, yaw���� ����� ���� ȸ������ ���ϰ�
		
		FRotator NewRotate = FMath::RInterpTo(Current, Delta, GetWorld()->GetDeltaSeconds(), 15.f); //�ε巴�� ȸ����Ų��

		FRotator NewRotYaw = FRotator(0.f, NewRotate.Yaw, 0.f);
		FRotator NewRotPitch = FRotator(NewRotate.Pitch, 0.f, 0.f);
		
		Yaw = NewRotate.Yaw;//NewRotYaw.ClampAxis(90.f); //���� ȸ���� ������ �ɾ��ش�.
		Pitch = NewRotate.Pitch;//NewRotPitch.ClampAxis(90.f);
	}
}


void UMainAnimInstance::AnimNotify_StepSound()
{
	StepSound.Broadcast();
}