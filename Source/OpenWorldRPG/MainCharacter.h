// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMainChracterStatus : uint8
{
	EMCS_Dead		UMETA(DisplayName = "Dead"),
	EMCS_Crouch		UMETA(DisplayName = "Crouch"),
	EMCS_Normal		UMETA(DisplayName = "Normal"),
	EMCS_Walk		UMETA(DisplayName = "Walk"),
	EMCS_Sprint		UMETA(DisplayName = "Sprint"),

	EMCS_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	ECM_TPS		UMETA(DisplayName = "TPS"),
	ECM_FPS		UMETA(DisplayName = "FPS"),
	
	ECM_MAX		UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API AMainCharacter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()
public:
	AMainCharacter();
	
	class UMainAnimInstance* MainAnimInstance;

	/**********   카메라 *************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	const float MAXCameraLength = 900.f;
	const float MINCameraLength = 350.f;

	/********** enum **********/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMainChracterStatus MainChracterStatus;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECameraMode CameraMode;

	/********** Movement *************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MinWalkSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bCrouchToggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bWalkToggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsWalking;

	/**************    Perception Source 관련   ******************/
	class UAIPerceptionStimuliSourceComponent* StimuliSourceComp;
	TSubclassOf<class UAISense_Sight> Sight;
	TSubclassOf<class UAISense_Hearing> Hearing;

	/**********  Sounds ************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	class USoundCue* StepSoundCue;

	/**********  Item 관련 ************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	class AActor* OverlappingActor;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	/************** Enum 함수 **************/
	void SetMainCharacterStatus(EMainChracterStatus Type);
	void SetCameraMode(ECameraMode Type);

	/********   Movement 함수 *******/
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void MyCrouch();
	void MyUnCrouch();

	void Walk();
	void UnWalk();

	void ScrollDN();
	void ScrollUP();

	void VKeyDN();

	/********** Sounds ********/
	void StepSound();

	/********** Perception ********/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;

	/************ Input **********/

	void EKeyDown();
	void EKeyUp();

	/********  Item 관련 ******/
	FORCEINLINE void SetOverlappingActor(AActor* Actor) { OverlappingActor = Actor; }
};
