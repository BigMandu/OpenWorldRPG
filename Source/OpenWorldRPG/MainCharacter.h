// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "MainCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickSlotUse, EQuickSlotNumber, SlotNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompassBeginUse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompassEndUse);

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
class UInventoryComponent; //안씀
class UNewInventoryComponent;
class UEquipmentComponent;
class UStatManagementComponent;
class UCraftSystemComponent;


UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	ECM_TPS		UMETA(DisplayName = "TPS"),
	ECM_FPS		UMETA(DisplayName = "FPS"),
	
	ECM_MAX		UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class OPENWORLDRPG_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	/* UQuickSlotWidget::BindSlotWidget에서 bind */
	FOnQuickSlotUse OnQuickSlotUse;

	/* CoreUsableItem에서 broadcast,*/
	FCompassBeginUse CompassBeginUse;
	FCompassEndUse CompassEndUse;
	bool bIsUsingCompass;

	const FName WeaponFPSLeftHandSocketName = FName("FPSLeftHandPos");


	UPROPERTY(EditAnywhere)
	class USceneCaptureComponent2D* CharacterCapture;

	/* Anim Instance */
	UMainAnimInstance* FPAnimInstance;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	//AMainController* MainController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* FPMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* ShadowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	UCraftSystemComponent* CraftSysComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character)
	USkeletalMeshComponent* CharacterMaskClothes;
	const FName MaskSocketName = FName("MaskClothes");

	UPROPERTY()
	float TempVar_OriginInputYawScale;
	UPROPERTY()
	float TempVar_OriginInputPitchScale;

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

	FTransform FPMeshOriginTransform;
	//아래는 안쓰는 변수들.
	
	/* FPS Aim을 위한 저장값*/
	FTransform BaseFPMeshTransform;
	FTransform BaseWeapTransform;

	/********** enum **********/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECameraMode CameraMode;

	


	/********* Input *********/
	bool bBlockMoveRight = false;
	bool bMoveForward = false;
	bool bMoveRight = false;

	//for Sprint
	bool bMovingStraightForward = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bSprintKeyDown;
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsJumpKeyDown;*/
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

	/********* 입력 막아주기 *********/
	bool bDisableInput; //Widget을 보고 있을때 true로 만들어 특정키 입력을 막는다.
	bool bDisableInteractionLineTrace;
	
	/**********  Interactive 관련 ************/
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
	FTimerHandle T_CompassHandle;
	FTimerHandle T_ADSTimerHandle;
	FTimerHandle T_CameraHandle;

	float ADSTime;
	float ADSAlphaTime;

	float CamTime;
	float CamAlphaTime;

private:
	bool CanSprint();	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	void CameraUpdate(const FVector& CamLoc, const FRotator& CamRot);

	virtual void Destroyed() override;


	void InteractionLineTrace();

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	

	

	/***** Timer 관련 ******/
	void ClearSprintUpTimer();
	
	/********   Movement 함수 *******/
	void MoveForward(float Value);
	void MoveRight(float Value);

	virtual void Sprint() override;
	virtual void UnSprint() override;

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

	virtual void ChangeWeaponTypeNumber(int32 number) override;

	/************ Input **********/
	void EKeyDown();
	void TabKeyDown();

	void LMBDown();
	void LMBUp();

	/* Item & Weapon 관련 */
	
	void RMBDown();
	void RMBUp();

	virtual void SetEquippedWeapon(AWeapon* Weapon) override;

	//called when rmbdown
	virtual void SetAimMode(EAimMode Mode) override;
	
	UFUNCTION()
	void FPSADS();
	void FPSnotADS();
	FTransform GetFpsAdsPosition();


	//called when v key down
	void SetCameraMode(ECameraMode Type);	
	void LerpCamera(UCameraComponent* VarTPScam, float TargetBoomLength, FVector TargetCamRelativeLocation, float TargetFOV);
	void LerpCamera(UCameraComponent* VarFPScam, float TargetFOV);

	/**CameraMode에 따라 HoldingItem을 Attach시킨다.
	 * V key를 누를 때 마다 호출된다.
	 */
	void ReAttachHoldingItem();

	void FPSAimLocationAdjust();

	
	virtual void PlayAnimation(UAnimMontage* TPAnim, UAnimMontage* FPAnim) override;
	virtual void StopAnimation() override;

	/* Sound */
	virtual void StepSound() override;
	virtual void SpeakSound(USoundCue* Sound) override;
	virtual void PlayReloadSound(EPlayReloadSound ReloadSoundType) override;


	/* bind function. CompassBeginUse, EndUse*/
	UFUNCTION()
	void CompassStart();
	UFUNCTION()
	void CompassStop();


	//FPMesh때문에 override해서 FPAnim을 따로 갱신해준다.
	virtual bool ChangeWeapon(int32 index) override;


	/********  Interaction 관련 ******/

	FHitResult InteractableLineTrace(const FVector& StartLo, const FVector& EndLo);

	void SetInteractActor(AActor* Actor);

	void UnsetInteractActor();

	void Interactive();

	
	FTransform LeftHandik();
	virtual void SetLeftHandIK(float Alpha);

	virtual void Die() override;

	/* Quick Slot */
	void QuickSlotNum4();
	void QuickSlotNum5();
	void QuickSlotNum6();
	void QuickSlotNum7();
	void QuickSlotNum8();
	void QuickSlotNum9();
};
