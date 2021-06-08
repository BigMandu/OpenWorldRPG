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
	class AMainCharacter* Player;


	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimProperties)
	void UpdateAnimationProperties();

	UFUNCTION()
	void AnimNotify_StepSound();

	
};
