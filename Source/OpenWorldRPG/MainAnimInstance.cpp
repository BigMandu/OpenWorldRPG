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

		/* TPMesh의 Rotation값*/
		TP_HighReadyRotator_Left = FMath::RInterpTo(TP_HighReadyRotator_Left, TP_NewALRot, GetWorld()->GetDeltaSeconds(), 1.0f);
		TP_HighReadyRotator_Right = FMath::RInterpTo(TP_HighReadyRotator_Right, TP_NewARRot, GetWorld()->GetDeltaSeconds(), 5.0f);


		/* FPMesh의 Rotation 값 */


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



	/* 0에 가까워지면 bEndHighReady를 false로 바꿔줘 재 실행을 방지한다. */
	if(TP_HighReadyRotator_Right.IsNearlyZero(0.01) || FP_HighReadyRotator.IsNearlyZero(0.01))
	{
		bEndHighReady = false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AnimInst : End High Ready : %s"), *FP_HighReadyRotator.ToString());
}