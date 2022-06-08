// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
//#include "OpenWorldRPG/Item/Interactive_Interface.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h" //������


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//To base
	/*HeadSocketName = FName("headsocket");
	GripSocketName = FName("WeaponGrip");
	WeaponLeftHandSocketName = FName("LeftHandPos");*/


	/******** Movement ********/
	//GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //ȸ���ӵ�
	//GetCharacterMovement()->JumpZVelocity = 540.f;
	//GetCharacterMovement()->AirControl = 0.2f;

	//GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //��ũ���⸦ �� �� �ֵ��� true�� ���ش�.
	//GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //��ũ�� ũ�⸦ �⺻HalfHeight�� /1.6���� �����Ѵ�.
	//
	//GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	//GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	//bIsWalking = false; //�ȱ� �⺻������ false.
	

	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //��ũ���� Ű Toggle�� true�� �⺻���� �Ѵ�.
	bWalkToggle = true; //�ȱ� Ű Toggle�� true�� �����Ѵ�.
	bAimToggle = true; //����Ű�� toggel true����.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;

	/* ī�޶� ���� */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetRootComponent());

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = true;
	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3��Ī ī�޶� ��¦ �������� ġ������ �Ѵ�.

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeTransform(CameraFPSTransform);//SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	/****** First Person Mesh *****/
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	/*** Weapon Grip socket Relative Lo, Ro ***/
	/*
		Rotation�� ���� �ؾ���.X = Roll, Y = Pitch, Z = Yaw
		Rotation(Y,Z,X)������ ����.
	*/
	/*
	FVector(-2.999811, 5.476199, -1.011013));
	FRotator(8.32585, -105.164108, 10.820496).Quaternion()); //->Rifle�� X�࿡ ���� Mesh�� ������� ����.
	*/
	/*RifleRelativeLoRo.SetLocation(FVector(0, 0, 0));
	RifleRelativeLoRo.SetRotation(FRotator(200, 200, 200).Quaternion());*/
	
	/*PistolRelativeLoRo.SetLocation(FVector(-3.271253, 5.217403, -1.969826));
	PistolRelativeLoRo.SetRotation(FRotator(10.704832, 163.24559, -8.476412).Quaternion());*/

	/****** Item ****/
	/*Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("InventoryComp"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));*/

	ActiveInteractDistance = 200.f; //��ȣ�ۿ� �������� ǥ�õǴ� �ִ�Ÿ�.

	/********** �ʱ� ���� ************/
	SetCameraMode(ECameraMode::ECM_TPS); //�ʱ� ī�޶� ���� 3��Ī ����.
	SetAimMode(EAimMode::EAM_NotAim);
}


void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Player�� AnimInstance�� �ҷ��´�. */
	//TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); 
	FPAnimInstance = Cast<UMainAnimInstance>(FPMesh->GetAnimInstance());

	if (FPAnimInstance)//TPAnimInstance && FPAnimInstance)
	{
		/* ����� TP Animation�� �������� ����Ѵ�. */ //AnimInstance�� StepSound_Notify���� ȣ��.
		//TPAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); 
		//TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
	}


	FPMeshOriginTransform = FPMesh->GetRelativeTransform();
	FPMesh->SetHiddenInGame(true);

	/* FPS Aim���� �⺻ ���� �� ����*/
	BaseCamTPSfov = CameraTPS->FieldOfView;
	BaseCamFPSfov = CameraFPS->FieldOfView;
	BaseFPMeshTransform = FPMesh->GetRelativeTransform();


	//GetCapsuleComponent()->SetCollisionProfileName(FName("MainChar"));
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	//GetCapsuleComponent()->SetCollisionProfileName("MainChar");
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainController = Cast<AMainController>(GetController());
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//bIsLookInput ����
	float turnvalue = GetInputAxisValue("Turn");
	float lookvalue = GetInputAxisValue("LookUp");
	if (turnvalue == 0.f && lookvalue == 0.f)
	{
		bIsLookInput = false;
	}
	else
	{
		bIsLookInput = true;
	}

	//bIsAccelerating ����., To base
	//FVector VecLength = FVector(0.f);
	//if (GetVelocity().Size() > VecLength.Size())
	//{
	//	bIsAccelerating = true;
	//}
	//else
	//{
	//	bIsAccelerating = false;
	//}


	/* ��ȣ�ۿ� Line Trace ���� . ver2  �Ʒ� �ڵ带 GetPlayerViewPoint�Լ��� �̿��� ����ȭ ���״�. */
	FRotator Rot;
	MainController->GetPlayerViewPoint(Interact_LineTrace_StartLocation, Rot);
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * (MAXCameraLength + 800.f); //ī�޶�Boom���̺��� ����� �������Ѵ�.
	}
	
	/* ī�޶� ��忡 ���� ��ȣ�ۿ� LineTrace ����. ver 1 --> ver 2�� ��ü.*/
	/*
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_StartLocation = CameraFPS->GetComponentLocation();
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraFPS->GetComponentRotation().Vector() * 500.f;

		break;
	case ECameraMode::ECM_TPS:
	{
		Interact_LineTrace_StartLocation = CameraTPS->GetComponentLocation(); //TPSī�޶� �������� Trace�� �����Ѵ�.
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraTPS->GetComponentRotation().Vector() * (MAXCameraLength + 800.f); //ī�޶�Boom���̺��� ����� �������Ѵ�.
		break;
	}
	default:
		break;
	}
	*/



	/* 3��Ī ��忡�� ���� �ְ� �����̰� ������ ������ �������� �����Ѵ�.*/
	if (EquippedWeapon && CameraMode == ECameraMode::ECM_TPS)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	

	/* ��ȣ�ۿ� �ؽ�Ʈ�� ��� */
	/* Static FHitResult�� ���Ϲ޾Ƽ� Interface ��ȯ, �����ϸ� Outline�� TEXT�� ���.*/
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();
	if (HitActor)
	{
		if (InteractActor) //�̹� InteractActor�� �ְ�
		{
			if (HitActor != InteractActor) //���� ���� �ִ� Actor�� InteractActor�� �ٸ���
			{
				UnsetInteractActor(); //InteractActor�� Unset���ش�.
			}
			else
			{
				//�Ÿ��� ���� InteractText�� ������Ʈ �Ǳ� ������ �߰� ���ش�.
				SetInteractActor(HitActor); 
			}
		}
		else //InteractActor�� ���� ���
		{
			//AInteractable* Interactable = Cast<AInteractable>(HitActor);
			//if (Interactable)
			{
				//SetInteractActor(HitActor); //Interactable�� ��� Set�� ���ش�.
				SetInteractActor(HitActor); //Interactable�� ��� Set�� ���ش�.
			}
		}
	}
	else
	{
		UnsetInteractActor();
	}
	//InteractActor�� �ٸ���� UnsetInteractActor ȣ������. (Outline�� �Ⱦ������� ���װ� ����.) -> �ذ���.

	/* Weapon�� AimInitialize�Լ����� ����ϴ� Timer ������ */
	if (EquippedWeapon)
	{
		//Ÿ�̸Ӱ� �۵����̰�
		if (GetWorldTimerManager().IsTimerActive(EquippedWeapon->AimInitHandle))
		{
			//aim�� �����ų�, AlphaTime�� 1�ʰ� �Ѿ�ų�, ��� ���̶��
			if (bIsLookInput || EquippedWeapon->AlphaTime >= 1.f || EquippedWeapon->bIsFiring)
			{
				//���� �ʱ�ȭ Ÿ�̸Ӹ� �ʱ�ȭ ��Ų��.
				UE_LOG(LogTemp, Warning, TEXT("MainChar:: Clear AimInit Timer"));
				GetWorldTimerManager().ClearTimer(EquippedWeapon->AimInitHandle);
			}
		}
		if (EquippedWeapon->RecoilAlphaTime >= 1.f)
		{
			GetWorldTimerManager().ClearTimer(EquippedWeapon->RecoilHandle);
		}
		
	}
	
	
}


/***************************** enum �Լ�********************************/
//void AMainCharacter::SetMainCharacterStatus(EPlayerStatus Type) //�÷��̾��� ����
//{
//	MainChracterStatus = Type;
//	switch (MainChracterStatus)
//	{
//	case EPlayerStatus::EPS_Normal:
//		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
//		bIsWalking = false;
//		break;
//	case EPlayerStatus::EPS_Walk:
//		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
//		bIsWalking = true;
//		break;
//	case EPlayerStatus::EPS_Sprint:
//		break;
//	default:
//		break;
//	}
//}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/************** Movement & sight key bind ***************/
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::MyStopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);

	PlayerInputComponent->BindAction("ScrollDN", IE_Pressed, this, &AMainCharacter::ScrollDN);
	PlayerInputComponent->BindAction("ScrollUP", IE_Pressed, this, &AMainCharacter::ScrollUP);

	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AMainCharacter::VKeyDN);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBUp);

	PlayerInputComponent->BindAction("ChangeSafetyLever", IE_Pressed, this, &ABaseCharacter::ChangeSafetyLever);

	/******** Weapon Quick Swap ***********/
	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &ABaseCharacter::ChangePrimaryWeapon);
	PlayerInputComponent->BindAction("Sub", IE_Pressed, this, &ABaseCharacter::ChangeSubWeapon);
	PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &ABaseCharacter::ChangePistolWeapon);

	/************** Interactive & Inventory key bind ************/

	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AMainCharacter::TabKeyDown);

	PlayerInputComponent->BindAction("Interactive", IE_Pressed, this, &AMainCharacter::EKeyDown);

}


void AMainCharacter::FPSAimLocationAdjust()
{
	if (CameraFPS)
	{
		if (bIsAim)
		{
			//�� �Ѹ��� FPMesh�� �ùٸ� ��ġ�� �����ϱ� ����
			//Weapon Class�� Transform ������ ��������.
			const FTransform NewFPMeshTransform = EquippedWeapon->CharFPMeshTransform;
			const FTransform NewWeaponTransform = EquippedWeapon->WeapSKMeshTransform;
			CameraFPS->SetFieldOfView(70.f);
			//FPMesh->SetRelativeLocation(NewFPMeshLocation);
			FPMesh->SetRelativeTransform(NewFPMeshTransform);
			EquippedWeapon->SKMesh->SetRelativeTransform(NewWeaponTransform);
		}
		else
		{
			CameraFPS->SetFieldOfView(BaseCamFPSfov);
			FPMesh->SetRelativeTransform(BaseFPMeshTransform);
			EquippedWeapon->SKMesh->SetRelativeTransform(BaseWeapTransform);
		}
	}
}


//�÷��̾� ���� ���� -> VKeyDN
void AMainCharacter::SetCameraMode(ECameraMode Type)
{
	CameraMode = Type;
	switch (CameraMode)
	{
	case ECameraMode::ECM_TPS:
		CameraTPS->Activate();
		CameraFPS->Deactivate();

		/* ȸ���� ī�޶󿡸� ���� ������ ���� */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = true;

		/* Character Mesh ���� */
		FPMesh->SetHiddenInGame(true);  //1��Ī Mesh ����
		GetMesh()->SetHiddenInGame(false); //3��Ī Mesh �ȼ���
		break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//ī�޶� ȸ���� ĳ���Ͱ� ȸ���� ���� ȸ���ϵ��� �Ѵ�. �׷��� �ް���, �¿� �԰����� �����ϴ�.
		bUseControllerRotationYaw = true;

		/*  Character Mesh ���� */
		FPMesh->SetHiddenInGame(false);
		GetMesh()->SetHiddenInGame(true);
		break;
	default:
		break;
	}

	/* ���� ���� Mesh�� ���� */
	if (EquippedWeapon)
	{
		EquippedWeapon->GunAttachToMesh(this);
	}

}

//RMB Down
void AMainCharacter::SetAimMode(EAimMode Mode)
{
	AimMode = Mode;
	if (EquippedWeapon)
	{
		switch (AimMode)
		{
		case EAimMode::EAM_Aim:
		{
			bIsAim = true;
			EquippedWeapon->bIsAiming = true;
			GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;

			if (CameraMode == ECameraMode::ECM_TPS)
			{
				//TPS��� + Aim�����϶��� ī�޶� ��¦ ������ �����ش�.
				//���� SprintArm�� ���̸� �����Ѵ�.
				BeforeCameraLength = CameraBoom->TargetArmLength;
				//float CameraLength = FMath::FInterpTo(BeforeCameraLength, MINCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);
				CameraBoom->TargetArmLength = MINCameraLength + 20.f;
				CameraTPS->SetRelativeLocation(TPSCam_Aim_Rel_Location);
				CameraTPS->FieldOfView = 77.f;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//��Ȯ�� ��ġ�� �޾ƿ��� ���� Animation�� ����.
				FPMesh->bPauseAnims = true;

				//EquippedWeapon���� SightSocket�� Get�ϴ� �Լ��� ȣ���Ѵ�.
				FTransform SightTransform = EquippedWeapon->GetSightSocketTransform();
				FTransform FPMeshWorld = FPMesh->GetComponentTransform();


				//SightSocket����  FPMesh�� Relative Transform�� ���Ѵ�.
				//�̷��� ���� Offset�� Mesh�� Transform�� �����Ѵ�.

				//explain Make Relative Transform
				//Example: ChildOffset = MakeRelativeTransform(Child.GetActorTransform(), Parent.GetActorTransform())
				//This computes the relative transform of the Child from the Parent.


				FTransform  Offset = FPMeshWorld.GetRelativeTransform(SightTransform);

				// Weapon�� Clipping�� ���� Trace�� FPMesh�϶� Aim, NotAim�� �����ϱ� ���� Aim�϶� Mesh�� ������ �� ������ �Ѵ�.
				// (���� Aim���� FPMesh�� �ڷ� ������ Clipping������ ���Ͼ� ����.)
				FTransform OffsetWithoutX = FTransform(Offset.GetRotation(), FVector(5.f, Offset.GetTranslation().Y, Offset.GetTranslation().Z));


				//FPMesh�� �ش� Offset��ŭ �̵�, Camera�� �߾ӿ� ������ �Ѵ�.
				FPMesh->SetRelativeTransform(OffsetWithoutX);
				CameraFPS->FieldOfView = 75.f;


				//�Ʒ��� �Ⱦ�.
				/* Aim mode + FPS��� ������ ������ �߰�,
				   �ش�������� �������� �ֱ� ���� �Լ��� ȣ���Ѵ�.*/
				   //EquippedWeapon->FPS_AimAttachToMesh(this);
				   //FPSAimLocationAdjust();
			}
			break;
		}
		case EAimMode::EAM_NotAim:
		{
			bIsAim = false;
			EquippedWeapon->bIsAiming = false;
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

			if (CameraMode == ECameraMode::ECM_TPS)
			{
				if (EquippedWeapon)
				{
					GetCharacterMovement()->bOrientRotationToMovement = false;
					bUseControllerRotationYaw = true; //false; //3��Ī�� Aim���°� �ƴϸ�, Yaw�� Ǯ���ش�.
				}
				else
				{
					GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
					bUseControllerRotationYaw = false; //3��Ī�� Aim���°� �ƴϸ�, Yaw�� Ǯ���ش�.
				}



				//���� ī�޶� �ٽ� ���� ��Ų��.
				/*float CurrentLength = CameraBoom->TargetArmLength;
				float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
				CameraBoom->TargetArmLength = BeforeCameraLength;
				CameraTPS->SetRelativeLocation(TPSCam_Rel_Location);
				CameraTPS->FieldOfView = BaseCamTPSfov;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				FPMesh->bPauseAnims = false;
				FPMesh->SetRelativeTransform(FPMeshOriginTransform);

				CameraFPS->FieldOfView = BaseCamFPSfov;
				/* Aim mode + FPS��� ������ ������ �߰�, ���� ���ױ� ������
				������ ���ش�.*/
				//EquippedWeapon->FPS_AimAttachToMesh(this);
				//FPSAimLocationAdjust();
			}
			break;
		}
		default:
			break;
		}
	}
}


/****************************** �̵�, ���� ���� Ű ���ε� ***************************/

void AMainCharacter::TurnAtRate(float Rate)
{
	//�� delta�� ���� base rate��ŭ ȸ��
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::MoveForward(float Value)
{
	if (MainController != NULL && Value != 0.f)
	{
		//����������� ���� ã��
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward���͸� ���Ѵ�.
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//�� �������� value��ŭ ����.
		AddMovementInput(Direction, Value);
		/*UE_LOG(LogTemp, Warning, TEXT("my  ForVec = %s"), *Direction.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Actor::ForVec = %s"), *GetActorForwardVector().ToString());*/
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (MainController != NULL && Value != 0.f)
	{

		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		//right ����
		FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
		/*UE_LOG(LogTemp, Warning, TEXT("my  RigVec = %s"), *Direction.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Actor::RigVec = %s"), *GetActorRightVector().ToString());*/
	}
}

void AMainCharacter::MyJump()
{
	if (bDisableInput == false)
	{
		Super::Jump();
	}
}
void AMainCharacter::MyStopJumping()
{
	Super::StopJumping();
}

void AMainCharacter::MyCrouch()
{
	if (bDisableInput == false)
	{
		if (bCrouchToggle && bIsCrouched)
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: UnCrouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}

		if (bIsCrouched == false)
		{
			Super::Crouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: Crouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Crouch);
			SetCharacterStatus(ECharacterStatus::EPS_Crouch);
		}
	}
}

void AMainCharacter::MyUnCrouch()
{
	//Crouch�� Toggle�� ������� �������� Key Release  bind�Լ��� ȣ���Ѵ�.
	if (bDisableInput == false)
	{
		if (bCrouchToggle == false && bIsCrouched)
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("UnCrouch:: UnCrouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}
	}
}

void AMainCharacter::Walk()
{
	if (bDisableInput == false)
	{
		if (bWalkToggle && bIsWalking && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: UnWalk"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
			//bIsWalking = false; -> Set enum�Լ����� ���������.
			return;
		}

		if (bIsWalking == false && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: Walk"));
			SetCharacterStatus(ECharacterStatus::EPS_Walk);
			//SetMainCharacterStatus(EPlayerStatus::EPS_Walk);
			//bIsWalking = true;
			return;
		}
	}
}

void AMainCharacter::UnWalk()
{
	if (bDisableInput == false)
	{
		if (bWalkToggle == false && bIsWalking == true && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnWalk:: UnWalk"));
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			//bIsWalking = false;		
		}
	}
}

void AMainCharacter::ScrollDN()
{
	if (bDisableInput == false)
	{
		//TPS����� ��쿡�� ��ũ�� �ٿ�, ���� �����ϵ��� (ī�޶� �ְ� , ������)
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			if (CameraBoom->TargetArmLength <= MINCameraLength)
			{
				CameraBoom->TargetArmLength = MINCameraLength;
			}
			else CameraBoom->TargetArmLength -= 25;
		}
	}
}

void AMainCharacter::ScrollUP()
{
	if (bDisableInput == false)
	{
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			if (CameraBoom->TargetArmLength >= MAXCameraLength)
			{
				CameraBoom->TargetArmLength = MAXCameraLength;
			}
			else CameraBoom->TargetArmLength += 25;
		}
	}
}

void AMainCharacter::VKeyDN()
{
	if (bDisableInput == false)
	{
		switch (CameraMode)
		{
		case ECameraMode::ECM_TPS:
			SetCameraMode(ECameraMode::ECM_FPS);
			break;
		case ECameraMode::ECM_FPS:
			SetCameraMode(ECameraMode::ECM_TPS);
			break;
		default:
			break;
		}
	}
}


/************** Interactive & Inventory Key bind �Լ� ***********/
void AMainCharacter::LMBDown()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StartFire();
		}
	}
}

void AMainCharacter::LMBUp()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StopFire();
		}
	}
}

void AMainCharacter::RMBDown()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon != nullptr)
		{
			if (EquippedWeapon->bIsHighReady == false)
			{
				if (bAimToggle && bIsAim)
				{
					SetAimMode(EAimMode::EAM_NotAim);
					UE_LOG(LogTemp, Warning, TEXT("Aim off"));
				}
				else
				{
					SetAimMode(EAimMode::EAM_Aim);
					UE_LOG(LogTemp, Warning, TEXT("Aim on"));
				}
			}
		}
	}
}

void AMainCharacter::RMBUp()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon != nullptr)
		{
			if (bAimToggle == false)
			{
				SetAimMode(EAimMode::EAM_NotAim);
				UE_LOG(LogTemp, Warning, TEXT("Aim off"));
			}
		}
	}
}


void AMainCharacter::TabKeyDown()
{
	bTabKeyDown = true;
	UE_LOG(LogTemp, Warning, TEXT("Tab key down"));

	MainController->ToggleInventory();
}

void AMainCharacter::EKeyDown()
{
	if (bDisableInput == false)
	{
		Interactive();
	}
}

/*************************  Weapon, Item ���� ***************************************************/

/*
void AMainCharacter::UseItem(class AActor* Item)
{
	if (Item)
	{
		//Item->Use(this);
	}
}*/

//void AMainCharacter::ChangeSafetyLever()
//{
//	if (EquippedWeapon)
//	{
//		EquippedWeapon->ChangeSafetyLever();
//	}
//}


void AMainCharacter::ChangeWeapon(int32 index)
{
	//Super::ChangeWeapon(index);
	switch (index)
	{
	case 0:
		TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
		break;
	case 1:
		// ���� �����ϰ� �ִ� ���Ⱑ Primary�� �ٸ���쿡�� ����. ��ġ�ϸ� �Ȱ����� ������ �ʿ䰡 ����.
		if (PrimaryWeapon && (PrimaryWeapon != EquippedWeapon))
		{
			PrimaryWeapon->GunAttachToMesh(this);
			if (TPAnimInstance && FPAnimInstance)
			{
				TPAnimInstance->WeaponTypeNumber = 1;
				FPAnimInstance->WeaponTypeNumber = 1;
				//EquippedWeapon = PrimaryWeapon;
			}
		}
		break;
	case 2:
		if (SubWeapon && (SubWeapon != EquippedWeapon))
		{
			SubWeapon->GunAttachToMesh(this);
			if (TPAnimInstance && FPAnimInstance)
			{
				TPAnimInstance->WeaponTypeNumber = 1;
				FPAnimInstance->WeaponTypeNumber = 1;
				//EquippedWeapon = SubWeapon;
			}
		}
		break;
	case 3:
		if (PistolWeapon && (PistolWeapon != EquippedWeapon))
		{
			PistolWeapon->GunAttachToMesh(this);
			if (TPAnimInstance && FPAnimInstance)
			{
				TPAnimInstance->WeaponTypeNumber = 2;
				FPAnimInstance->WeaponTypeNumber = 2;
				//EquippedWeapon = PistolWeapon;
			}
		}
		break;
	}
}

/*
void AMainCharacter::ChangePrimaryWeapon()
{
	ChangeWeapon(1);
}
void AMainCharacter::ChangeSubWeapon()
{
	ChangeWeapon(2);
}
void AMainCharacter::ChangePistolWeapon()
{
	ChangeWeapon(3);
}
*/

/* To Base
void AMainCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		EquippedWeapon = Weapon;
		
		//EquippedWeapon->OnBeginHighReady.AddDynamic(this, &UMainAnimInstance::BeginHighReady);
		//EquippedWeapon->OnEndHighReady.AddDynamic(this, &AMainCharacter::EndHighReady);
		
	}
}*/



/* Rifle, Pistol�� LeftHand ��ġ */
/* To Base
FTransform AMainCharacter::LeftHandik()
{
	FTransform Transform;
	if (EquippedWeapon)
	{
		FVector LeftTransVector;
		FRotator LeftTransRotation;

		Transform = EquippedWeapon->SKMesh->GetSocketTransform(WeaponLeftHandSocketName, ERelativeTransformSpace::RTS_World);
		return Transform;
	}

	return Transform;
}
*/

/*************************  Interaction ���� ***************************************************/

/* Static�� �����ؼ� FHitResult�� ���� */
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(5.f, 5.f); //��¦ �а� �ؼ� Ž���� ���� �ߴ�.
	Params.AddIgnoredActor(this);

	 
	//GetWorld()->LineTraceSingleByChannel(Hit, StartLo, EndLo, ECollisionChannel::ECC_WorldStatic, Params); // LineTraceSingle���� 
	
	//GetWorld()->SweepSingleByChannel(Hit, StartLo, EndLo, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, Shape, Params);
	GetWorld()->SweepSingleByChannel(Hit, StartLo, EndLo, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, Shape, Params); //SweepSingle�� ����, ĸ�������� ������� LineTrace.
	
	/* debug */
	//DrawDebugLine(GetWorld(), StartLo, EndLo, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
	//DrawDebugCapsule(GetWorld(), (StartLo+EndLo)/2, Shape.GetCapsuleHalfHeight(),Shape.GetCapsuleRadius(),FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Green, false, 0.8f, (uint8)nullptr, 2.f);

	return Hit;
}


void AMainCharacter::SetInteractActor(AActor* Actor)
{
	//MainController->ShowInteractText(); //Ư���Ÿ����� Text�� �ߵ��� ������. �Ʒ� �ڵ�.
	//if (InteractActor == nullptr) //�پ������� �ٲ��� �ʱ⶧���� ���ǹ� ����.
	{
		InteractActor = Actor;
		//AInteractable* InActor = Cast<AInteractable>(InteractActor);
		IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
		if (InterfaceActor)//InActor)
		{
			InterfaceActor->SetOutline();

			//InteractActor�� Static Mesh�� ��ġ�� �������� �Ÿ��� ���Ѵ�.

			//InteractActor->GetActorLocation()
			//if ((GetActorLocation() - InActor->Mesh->GetComponentLocation()).Size() <= ActiveInteractDistance) //Ư���Ÿ� �̳��� Text Show.
			if((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
			{
				MainController->ShowInteractText();
			}

			/* debug */
			//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Valid"));
		}		
	}
	
}

void AMainCharacter::UnsetInteractActor()
{
	if (InteractActor)
	{
		//AInteractable* InActor = Cast<AInteractable>(InteractActor);
		//if (InActor)
		IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
		if(InterfaceActor)
		{
			InterfaceActor->UnsetOutline();
			//InActor->UnsetOutline();
		}
		InteractActor = nullptr;
		MainController->HideInteractText();
	}
	/* debug */
	//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Invalid"));
}

void AMainCharacter::Interactive()
{
	if (InteractActor)
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(InteractActor);
		//AInteractable* InActor = Cast<AInteractable>(InteractActor);
		if (Interface)// && InActor)
		{
			//if ((GetActorLocation() - InActor->Mesh->GetComponentLocation()).Size() <= ActiveInteractDistance)
			if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
			{
				//Interface->OnInteract.Broadcast();
				Interface->Interaction(this);
			}
		}
	}
	
}

/******************************************************************************/
/*
void AMainCharacter::StepSound()
{
	//UE_LOG(LogTemp, Warning, TEXT("AnimNotify -> AMainChar:: StepSound()"));

	if (StepSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSoundCue, GetActorLocation());
	}

	

	//UAISense_Hearing* HearingClass = Cast<UAISense_Hearing>(Hearing->GetClass());
	//if (HearingClass)
	//{
	//	HearingClass->ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
	//}
	
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);

}
*/

/*
bool AMainCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	
	FHitResult HitResult;
	FVector PlayerLocation;
	bool bResult = false;

	const USkeletalMeshSocket* Head = GetMesh()->GetSocketByName(HeadSocketName);
	if (Head) //Head socket�� ������ �� socket�� ��ġ��, 
	{
		PlayerLocation = GetMesh()->GetSocketLocation(HeadSocketName);
	}
	else PlayerLocation = GetActorLocation(); //������ Actor�� ��ġ��(�����)


	// FCollisionObjectQueryParams���� 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)�� ����ҰŴ�.
	// �̸� ����ϱ� ���ؼ��� To do this, use ECC_TO_BITFIELD to convert to bit field �̷��� �϶�� �Ѵ�. 

	//WorldDynamic, WorldStatic, IgnoreActor�� (�������� ��ġ���� Player�� ��ġ�� ����) LineTrace�� ����. 
	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, PlayerLocation
		, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_WorldStatic))
		, FCollisionQueryParams(FName(TEXT("SightSense")), true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	//������ �����Ȱ� �ƴϰų�, Actor�� Player���
	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = PlayerLocation;
		OutSightStrength = 1;
		bResult = true;
		//UE_LOG(LogTemp, Warning, TEXT("Player:: Catch"));
		//UE_LOG(LogTemp, Warning, TEXT("OutSeenLocation : %s, OutSightStrength : %f"), *OutSeenLocation.ToString(), OutSightStrength);
	}
	else
	{		
		OutSightStrength = 0;
		//UE_LOG(LogTemp, Warning, TEXT("Player:: Hiding"));
	}
	return bResult;
}
*/