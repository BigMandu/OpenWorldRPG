// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "MainCharacter.generated.h"

class AActor;
class AMainController;
class AWeapon;
class ALootBox;

class UAISense_Sight;
class UAISense_Hearing;
class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class UInputComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UMainAnimInstance;
class USpringArmComponent;
class USkeletalMeshComponent;
class USoundCue;
class UNewInventoryComponent;


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
	
	UMainAnimInstance* TPAnimInstance;
	UMainAnimInstance* FPAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	AMainController* MainController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* FPMesh;

	/************ Socket Name ************/
	FName HeadSocketName;
	FName GripSocketName;
	FName WeaponLeftHandSocketName;

	/**********   카메라 *************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraTPS; //3인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraFPS; //1인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	const FVector TPSCam_Rel_Location = FVector(0.f, 30.f, 20.f);

	const float MAXCameraLength = 600.f;
	const float MINCameraLength = 100.f;
	float BeforeCameraLength;

	/* FPS Aim을 위한 저장값*/
	float BaseCamFPSfov;
	FTransform BaseFPMeshTransform;
	FTransform BaseWeapTransform;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsLookInput;


	/********* 입력 막아주기 *********/
	bool bDisableInput; //Widget을 보고 있을때 true로 만들어 특정키 입력을 막는다.

	/********* Input *********/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bAimToggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bIsAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bTabKeyDown;

	/********* Inventory ********/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UInventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UNewInventoryComponent* NewInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UEquipmentComponent* Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	ALootBox* InteractLootBox;

	/**************    Perception Source 관련   ******************/
	UAIPerceptionStimuliSourceComponent* StimuliSourceComp;
	TSubclassOf<UAISense_Sight> Sight;
	TSubclassOf<UAISense_Hearing> Hearing;

	/**********  Sounds ************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	USoundCue* StepSoundCue;

	/**********  Interactive 관련 ************/
	//Minimum value of Active Interaction Distance.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
	float ActiveInteractDistance;

	FVector Interact_LineTrace_StartLocation;
	FVector Interact_LineTrace_EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	AActor* InteractActor;

	/************* Weapon 관련 ***************/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon")
	//FTransform RifleRelativeLoRo;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon")
	//FTransform PistolRelativeLoRo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* EquippedWeapon;

	/* Weapon Quick Swap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* SubWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PistolWeapon;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
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

	void MyJump();
	void MyStopJumping();
	
	void MyCrouch();
	void MyUnCrouch();

	void Walk();
	void UnWalk();

	void ScrollDN();
	void ScrollUP();

	void VKeyDN();

	/************ Input **********/
	void EKeyDown();
	void TabKeyDown();

	void LMBDown();
	void LMBUp();

	/* Item & Weapon 관련 */
	void RMBDown();
	void RMBUp();
	
	void FPSAimLocationAdjust();

	void ChangePrimaryWeapon();
	void ChangeSubWeapon();
	void ChangePistolWeapon();

	/* Change Weapon Firing Mode*/
	void ChangeSafetyLever();

	void ChangeWeapon(int32 index);

	UFUNCTION(BlueprintCallable)
	void UseItem(AActor* Item);

	/********  Hand ik ******/
	UFUNCTION(BlueprintCallable)
	FTransform LeftHandik();


	/********  Interaction 관련 ******/

	FHitResult InteractableLineTrace(const FVector& StartLo, const FVector& EndLo);

	void SetInteractActor(AActor* Actor);

	void UnsetInteractActor();

	void Interactive();

	/********** Sounds ********/
	void StepSound();

	/********** Perception ********/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const;
	//virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;

};
