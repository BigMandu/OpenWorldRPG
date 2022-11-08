// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE(FStepSoundDelegate);

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
private:

	FRotator TP_NewALRot;
	FRotator TP_NewARRot;
	FRotator FP_NewRot;
	
public:

	FStepSoundDelegate StepSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Yaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsinAir;

	class APawn* Pawn;

	UPROPERTY(BlueprintReadOnly)
	class ABaseCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	int32 WeaponTypeNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	float LeftHandAlpha = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	FVector LeftHandLocation;


	/* fix Clipping wall */
	bool bBeginHighReady;
	bool bEndHighReady;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator TP_HighReadyRotator_Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator TP_HighReadyRotator_Right;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator FP_HighReadyRotator;

	 

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimProperties)
	void UpdateAnimationProperties();

	UFUNCTION()
	void AnimNotify_StepSound();

	void SetHandIK();

	void SetHighReady();

	/*UFUNCTION()
	void BeginHighReady();
	UFUNCTION()
	void EndHighReady();*/

	
};
