// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "MainCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickSlotUse, EQuickSlotNumber, SlotNum);

class AActor;
class AMainController;
class AWeapon;
class AContainer;

class UAISense_Sight;
class UAISense_Hearing;
//class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;
class UInputComponent;
class UMainAnimInstance;
class USkeletalMeshComponent;
class UPoseableMeshComponent;

/* Custom Actor Component */
class UInventoryComponent; //�Ⱦ�
class UNewInventoryComponent;
class UEquipmentComponent;
class UStatManagementComponent;


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

	FOnQuickSlotUse OnQuickSlotUse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	AMainController* MainController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* FPMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* FPLowerLegMesh;



	/**********   ī�޶� *************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraTPS; //3��Ī ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraFPS; //1��Ī ī�޶�

	//1��Ī ī�޶��� �ʱ� ��ġ��.
	FTransform CameraFPSTransform = FTransform(FRotator(0.f,0.f,0.f),FVector(-7.f,11.8f,57.f));
	//1��Ī ī�޶��� FOV��.
	float BaseCamFPSfov;

	//3��Ī ī�޶��� Aim�� ���� ��ġ��.
	float BaseCamTPSfov;
	const FVector TPSCam_Rel_Location = FVector(0.f, 0.f, 8.f);
	const FVector TPSCam_Aim_Rel_Location = FVector(0.f, 18.5f, -8.f);
	const float MAXCameraLength = 170.f;
	const float MINCameraLength = 80.f;
	float BeforeCameraLength;

	//�Ʒ��� �Ⱦ��� ������.
	FTransform FPMeshOriginTransform;
	/* FPS Aim�� ���� ���尪*/
	FTransform BaseFPMeshTransform;
	FTransform BaseWeapTransform;

	/********** enum **********/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECameraMode CameraMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EAimMode AimMode;

	/********* Input *********/
	bool bMoveForward = false;
	bool bMoveRight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bSprintKeyDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsJumpKeyDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsLookInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bTabKeyDown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bCrouchToggle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bWalkToggle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bAimToggle;

	/********* �Է� �����ֱ� *********/
	bool bDisableInput; //Widget�� ���� ������ true�� ����� Ư��Ű �Է��� ���´�.

	
	/**********  Interactive ���� ************/
	//Minimum value of Active Interaction Distance.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
	float ActiveInteractDistance;

	FVector Interact_LineTrace_StartLocation;
	FVector Interact_LineTrace_EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	AActor* InteractActor;

	/************** Timer **************/
	FTimerHandle T_SprintKeyDown;
	FTimerHandle T_SprintKeyUp;

	
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

	/************** Enum �Լ� **************/
	//void SetMainCharacterStatus(EPlayerStatus Type); //To base
	void SetCameraMode(ECameraMode Type);
	void SetAimMode(EAimMode Mode);

	/***** Timer ���� ******/
	void ClearSprintUpTimer();

	/********   Movement �Լ� *******/
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Sprint();
	void UnSprint();

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

	/* Item & Weapon ���� */
	void RMBDown();
	void RMBUp();
	
	void FPSAimLocationAdjust();


	//FPMesh������ override�ؼ� FPAnim�� ���� �������ش�.
	virtual void ChangeWeapon(int32 index) override;


	/********  Interaction ���� ******/

	FHitResult InteractableLineTrace(const FVector& StartLo, const FVector& EndLo);

	void SetInteractActor(AActor* Actor);

	void UnsetInteractActor();

	void Interactive();



	/* Quick Slot */
	void QuickSlotNum4();
	void QuickSlotNum5();
	void QuickSlotNum6();
	void QuickSlotNum7();
	void QuickSlotNum8();
	void QuickSlotNum9();
};
