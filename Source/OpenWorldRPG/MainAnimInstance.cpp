// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "MainCharacter.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "Item/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "Engine/SkeletalMeshSocket.h"

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

		if (Player->EquippedWeapon)
		{
			if (bBeginHighReady)
			{
				BeginHighReady();
			}

			if (bEndHighReady)
			{
				EndHighReady();
			}
		}
	}
}


void UMainAnimInstance::AnimNotify_StepSound()
{
	StepSound.Broadcast();
	/*FAnimNode_ModifyBone ModBone;
	FBoneReference Bone("")
	ModBone.BoneToModify = Bone.BoneName*/
}


void UMainAnimInstance::BeginHighReady()
{	
	if(Player)
	{
		TP_NewALRot = FRotator(0.f, 0.f, -90.f);
		TP_NewARRot = FRotator(0.f, 0.f, -90.f);

		/* TPMesh�� Rotation��*/
		TP_HighReadyRotator_Left = FMath::RInterpTo(TP_HighReadyRotator_Left, TP_NewALRot, GetWorld()->GetDeltaSeconds(), 1.0f);
		TP_HighReadyRotator_Right = FMath::RInterpTo(TP_HighReadyRotator_Right, TP_NewARRot, GetWorld()->GetDeltaSeconds(), 5.0f);


		/* FPMesh�� Rotation �� */


		FP_NewRot = FRotator(60.f, 0.f, 0.f);
		FP_HighReadyRotator = FMath::RInterpTo(FP_HighReadyRotator, FP_NewRot, GetWorld()->GetDeltaSeconds(), 5.0f);
		

		//UE_LOG(LogTemp, Warning, TEXT("FP_HighReadyRotator : %s"), *FP_HighReadyRotator.ToString());
	}
	


	
}
void UMainAnimInstance::EndHighReady()
{

	/* TP bone Trans */
	TP_NewALRot = FRotator(0.f, 0.f, 0.f);
	TP_NewARRot = FRotator(0.f, 0.f, 0.f);

	TP_HighReadyRotator_Left = FMath::RInterpTo(TP_HighReadyRotator_Left, FRotator(0.f), GetWorld()->GetDeltaSeconds(), 1.0f);
	TP_HighReadyRotator_Right = FMath::RInterpTo(TP_HighReadyRotator_Right, FRotator(0.f), GetWorld()->GetDeltaSeconds(), 1.5f);


	/* FP Mesh Trans */
	const FRotator FP_CurrentRot = Player->FPMesh->GetRelativeRotation();
	FP_HighReadyRotator = FMath::RInterpTo(FP_HighReadyRotator, FRotator(0.f), GetWorld()->GetDeltaSeconds(), 1.5f);



	/* 0�� ��������� bEndHighReady�� false�� �ٲ��� �� ������ �����Ѵ�. */
	if(TP_HighReadyRotator_Right.IsNearlyZero(0.01) || FP_HighReadyRotator.IsNearlyZero(0.01))
	{
		bEndHighReady = false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AnimInst : End High Ready : %s"), *FP_HighReadyRotator.ToString());
}