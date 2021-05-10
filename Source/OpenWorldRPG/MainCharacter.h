// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "MainCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
	EPS_Dead		UMETA(DisplayName = "Dead"),
	EPS_Crouch		UMETA(DisplayName = "Crouch"),
	EPS_Normal		UMETA(DisplayName = "Normal"),
	EPS_Walk		UMETA(DisplayName = "Walk"),
	EPS_Sprint		UMETA(DisplayName = "Sprint"),

	EPS_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	ECM_TPS		UMETA(DisplayName = "TPS"),
	ECM_FPS		UMETA(DisplayName = "FPS"),
	
	ECM_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EAimMode : uint8
{
	EAM_Aim				UMETA(DisplayName = "Aim"),
	EAM_NotAim			UMETA(DisplayName = "NotAim"),

	EAM_MAX				UMETA(DisplayName = "DefaultMAX")
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
	class UCameraComponent* CameraTPS; //3인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraFPS; //1인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	const FVector TPSCam_Rel_Location = FVector(0.f, 30.f, 20.f);

	const float MAXCameraLength = 600.f;
	const float MINCameraLength = 250.f;
	float BeforeCameraLength;

	/********** enum **********/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EPlayerStatus MainChracterStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECameraMode CameraMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EAimMode AimMode;

	/********** Movement *************/
	const float MaxWalkSpeed = 600.f; //뛰는 속도
	const float MinWalkSpeed = 300.f; //걷는 속도
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bCrouchToggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bWalkToggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsWalking;

	/********* Input *********/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bAimToggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bIsAim;

	/********* Inventory ********/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	class UInventoryComponent* Inventory;
	

	/**************    Perception Source 관련   ******************/
	class UAIPerceptionStimuliSourceComponent* StimuliSourceComp;
	TSubclassOf<class UAISense_Sight> Sight;
	TSubclassOf<class UAISense_Hearing> Hearing;

	/**********  Sounds ************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	class USoundCue* StepSoundCue;

	/**********  Item & Combat 관련 ************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	class AActor* OverlappingActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	class AWeapon* EquippedWeapon;


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
	void SetMainCharacterStatus(EPlayerStatus Type);
	void SetCameraMode(ECameraMode Type);
	void SetAimMode(EAimMode Mode);

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

	/************ Input **********/
	void EKeyDown();
	void EKeyUp();

	void RMBDown();
	void RMBUp();

	/********** Sounds ********/
	void StepSound();

	/********** Perception ********/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;

	

	/********  Item 관련 ******/
	FORCEINLINE void SetOverlappingActor(AActor* Actor) { OverlappingActor = Actor; }

	UFUNCTION(BlueprintCallable)
	void UseItem(class UItem* Item);
};
