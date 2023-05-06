// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE(FStepSoundDelegate);
//DECLARE_MULTICAST_DELEGATE_OneParam(FThrowDelegate,ABaseCharacter*);
DECLARE_MULTICAST_DELEGATE(FThrowDelegate);
//DECLARE_MULTICAST_DELEGATE(FStartADS);

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
	FThrowDelegate ThrowDelegate;
	//FStartADS StartADS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsinAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsJumpkeyDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsSprint;

	/* fix Clipping wall */
	bool bBeginHighReady;
	bool bEndHighReady;

	class APawn* Pawn;

	UPROPERTY(BlueprintReadOnly)
	class ABaseCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	int32 WeaponTypeNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	float LeftHandAlpha = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK)
	FVector LeftHandLocation;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator TP_HighReadyRotator_Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator TP_HighReadyRotator_Right;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoneMove)
	FRotator FP_HighReadyRotator;



	///////////////////////////////////////////////////////////////
	/************************   Functions    *********************/
	/////////////////////////////////////////////////////////////// 

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimProperties)
	void UpdateAnimationProperties();

	UFUNCTION()
	void AnimNotify_StepSound();

	UFUNCTION()
	void AnimNotify_throw();

	UFUNCTION()
	void AnimNotify_EndReload();

	UFUNCTION()
	void AnimNotify_EndEquipping();

	UFUNCTION()
	void AnimNotify_AttachWeapon();


	/*UFUNCTION()
	void AnimNotify_ADS();*/

	void SetHandIK();

	void SetHighReady();

	void SetLeftHandIKAlpha(float Alpha);

	void SetWeaponTypeNumber(int32 number);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
	bool IsReadyToThrow();

	/*UFUNCTION()
	void BeginHighReady();
	UFUNCTION()
	void EndHighReady();*/

	
};
