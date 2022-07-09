// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "MainCharacter.generated.h"

class AActor;
class AMainController;
class AWeapon;
class AContainer;

class UAISense_Sight;
class UAISense_Hearing;
//class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class UInputComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UMainAnimInstance;
class USpringArmComponent;
class USkeletalMeshComponent;
class USoundCue;
class UNewInventoryComponent;
class UPoseableMeshComponent;

//UENUM(BlueprintType)
//enum class EPlayerStatus : uint8
//{
//	EPS_Dead		UMETA(DisplayName = "Dead"),
//	EPS_Crouch		UMETA(DisplayName = "Crouch"),
//	EPS_Normal		UMETA(DisplayName = "Normal"),
//	EPS_Walk		UMETA(DisplayName = "Walk"),
//	EPS_Sprint		UMETA(DisplayName = "Sprint"),
//
//	EPS_MAX		UMETA(DisplayName = "DefaultMAX")
//};

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
class OPENWORLDRPG_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AMainCharacter();

	//To Base
	//UMainAnimInstance* TPAnimInstance;
	//UMainAnimInstance* FPAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	AMainController* MainController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* FPMesh;

	/************ Socket Name ************/ //To Base
	/*FName HeadSocketName;
	FName GripSocketName;
	FName WeaponLeftHandSocketName;*/

	/**********   카메라 *************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraTPS; //3인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraFPS; //1인칭 카메라

	//1인칭 카메라의 초기 위치값.
	FTransform CameraFPSTransform = FTransform(FRotator(0.f,0.f,0.f),FVector(-7.f,11.8f,57.f));
	//1인칭 카메라의 FOV값.
	float BaseCamFPSfov;

	//3인칭 카메라의 Aim을 위한 위치값.
	float BaseCamTPSfov;
	const FVector TPSCam_Rel_Location = FVector(0.f, 0.f, 8.f);
	const FVector TPSCam_Aim_Rel_Location = FVector(0.f, 18.5f, -8.f);
	const float MAXCameraLength = 170.f;
	const float MINCameraLength = 80.f;
	float BeforeCameraLength;

	//아래는 안쓰는 변수들.
	FTransform FPMeshOriginTransform;
	/* FPS Aim을 위한 저장값*/
	FTransform BaseFPMeshTransform;
	FTransform BaseWeapTransform;

	/********** enum **********/
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	//EPlayerStatus MainChracterStatus; //To base

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECameraMode CameraMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EAimMode AimMode;

	/********** Movement *************/
	//const float MaxWalkSpeed = 600.f; //뛰는 속도
	//const float MinWalkSpeed = 300.f; //걷는 속도
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	//bool bIsAccelerating; //To base

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bCrouchToggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bWalkToggle;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	//bool bIsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsLookInput;


	/********* 입력 막아주기 *********/
	bool bDisableInput; //Widget을 보고 있을때 true로 만들어 특정키 입력을 막는다.

	/********* Input *********/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bAimToggle;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bIsAim;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bTabKeyDown;

	/********* Inventory ********/
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UInventoryComponent* Inventory;*/


	//To Base
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UNewInventoryComponent* InventoryComp; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UEquipmentComponent* Equipment;*/

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	AContainer* InteractLootBox;*/


	/**********  Sounds ************/
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	//USoundCue* StepSoundCue; //To Base

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

	//To Base
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* EquippedWeapon;*/

	/* Weapon Quick Swap */
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* SubWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PistolWeapon;*/


	
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
	//void SetMainCharacterStatus(EPlayerStatus Type); //To base
	void SetCameraMode(ECameraMode Type);
	void SetAimMode(EAimMode Mode);

	/********   Movement 함수 *******/
	void MoveForward(float Value);
	void MoveRight(float Value);


	//Tobase
	void MyJump();
	void MyStopJumping();
	
	void MyCrouch();
	void MyUnCrouch();

	void Walk();
	void UnWalk();
	//here


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

	//void SetEquippedWeapon(AWeapon* Weapon);
	/*void ChangePrimaryWeapon();
	void ChangeSubWeapon();
	void ChangePistolWeapon();*/

	/* Change Weapon Firing Mode*/
	//void ChangeSafetyLever();


	//FPMesh때문에 override해서 FPAnim을 따로 갱신해준다.
	virtual void ChangeWeapon(int32 index) override;

	//UFUNCTION(BlueprintCallable)
	//void UseItem(AActor* Item);

	/********  Hand ik ******/
	/*UFUNCTION(BlueprintCallable)
	FTransform LeftHandik();*/


	/********  Interaction 관련 ******/

	FHitResult InteractableLineTrace(const FVector& StartLo, const FVector& EndLo);

	void SetInteractActor(AActor* Actor);

	void UnsetInteractActor();

	void Interactive();


	//To base

	/********** Sounds ********/
	//void StepSound();

	/********** Perception ********/
	//virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const;
	

};
