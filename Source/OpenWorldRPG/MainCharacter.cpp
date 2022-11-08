// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/UI/CharacterStatusWidget.h"
#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
//#include "OpenWorldRPG/Item/Interactive_Interface.h"


#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h" //������


// Sets default values
AMainCharacter::AMainCharacter() : Super()
{
	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //��ũ���� Ű Toggle�� true�� �⺻���� �Ѵ�.
	bWalkToggle = true; //�ȱ� Ű Toggle�� true�� �����Ѵ�.
	bAimToggle = true; //����Ű�� toggel true����.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;
	ActiveInteractDistance = 200.f; //��ȣ�ۿ� �������� ǥ�õǴ� �ִ�Ÿ�.

	/* ī�޶� ���� */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3��Ī ī�޶� ��¦ �������� ġ������ �Ѵ�.

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

	/****** First Person *****/
	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetRootComponent());

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeTransform(CameraFPSTransform);//SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	FPLowerLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPLegMesh"));
	FPLowerLegMesh->SetupAttachment(GetRootComponent());
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
	
	//FPMesh->HideBoneByName(FName("spine_02"));
	FPLowerLegMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_l"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_r"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->SetHiddenInGame(true);

	FPLowerLegMesh->SetCastShadow(false);
	FPLowerLegMesh->SetOnlyOwnerSee(true);
	FPLowerLegMesh->SetReceivesDecals(false);

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

	/********** �ʱ� ���� ************/
	SetCameraMode(ECameraMode::ECM_TPS); //�ʱ� ī�޶� ���� 3��Ī ����.
	SetAimMode(EAimMode::EAM_NotAim);

	MainController = Cast<AMainController>(GetController());


	/****** Main Hud�� Widget�� Event Bind ********/
	UMainHud* MainHud = Cast<UMainHud>(MainController->MainHud);
	if (MainHud)
	{
		UCharacterInventoryWidget* CharWidget = MainHud->NewInventoryWidget->CharInvWidget;
		if (CharWidget)
		{
			CharWidget->InitializeInventory(this);
			CharWidget->BindingAdditional(MainHud->NewInventoryWidget);
		}

		if(StatManagementComponent)
		{
			MainHud->CharStatWidget->BindStatManager(StatManagementComponent);
		}
		
		
		if (MainHud->WeaponStatus)
		{
			MainHud->WeaponStatus->BindWeaponWidget(this);
		}

		if (MainHud->QuickSlot)
		{
			MainHud->QuickSlot->BindSlotWidget(this);
		}

	}
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


	//���߿� Event�� �ٲ����
	/* 3��Ī ��忡�� ���� �ְ� �����̰� ������ ������ �������� �����Ѵ�.*/
	//if (EquippedWeapon && CameraMode == ECameraMode::ECM_TPS)
	//{
	//	bUseControllerRotationYaw = true;
	//	GetCharacterMovement()->bOrientRotationToMovement = false;
	//}
	

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

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/************** Movement & sight key bind ***************/
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	/* Bind Action */
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBUp);

	PlayerInputComponent->BindAction("ChangeSafetyLever", IE_Pressed, this, &ABaseCharacter::ChangeSafetyLever);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::MyStopJumping);
	
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);


	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::UnSprint);

	PlayerInputComponent->BindAction("QSlot_4", IE_Pressed, this, &AMainCharacter::QuickSlotNum4);
	PlayerInputComponent->BindAction("QSlot_5", IE_Pressed, this, &AMainCharacter::QuickSlotNum5);
	PlayerInputComponent->BindAction("QSlot_6", IE_Pressed, this, &AMainCharacter::QuickSlotNum6);
	PlayerInputComponent->BindAction("QSlot_7", IE_Pressed, this, &AMainCharacter::QuickSlotNum7);
	PlayerInputComponent->BindAction("QSlot_8", IE_Pressed, this, &AMainCharacter::QuickSlotNum8);
	PlayerInputComponent->BindAction("QSlot_9", IE_Pressed, this, &AMainCharacter::QuickSlotNum9);



	/************ Camera *************/
	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AMainCharacter::VKeyDN);
	PlayerInputComponent->BindAction("ScrollDN", IE_Pressed, this, &AMainCharacter::ScrollDN);
	PlayerInputComponent->BindAction("ScrollUP", IE_Pressed, this, &AMainCharacter::ScrollUP);

	
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
		UE_LOG(LogTemp,Warning,TEXT("MainChar::SetCamMode / Setting ControllerRotation "));
		GetCharacterMovement()->bOrientRotationToMovement = false;
		UE_LOG(LogTemp, Warning, TEXT("MainChar::SetCamMode / Setting OrientRotationToMov "));
		/* Character Mesh ���� */
		FPMesh->SetHiddenInGame(true);  //1��Ī Mesh ����
		GetMesh()->SetHiddenInGame(false); //3��Ī Mesh �ȼ���

		GetMesh()->SetRenderInMainPass(true);
		FPLowerLegMesh->SetHiddenInGame(true);

		/* ������ ��� ��� HIde�ϱ� */
		for (auto Equipped : Equipment->EquipmentItems)
		{
			if (Equipped && Equipped->Equipment)
			{
				Equipped->Equipment->SKMesh->SetRenderInMainPass(true);
				//Equipped->Equipment->SetActorHiddenInGame(true); //SetHidden(true);
			}
		}

	break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//ī�޶��� Yawȸ���� ���� ĳ������ Yaw Mesh�� ȸ���ϵ��� �Ѵ�. �׷��� �ް���, �¿� �԰����� �����ϴ�.
		bUseControllerRotationYaw = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;

		/*  Character Mesh ���� */
		FPMesh->SetHiddenInGame(false);
		FPLowerLegMesh->SetHiddenInGame(false);

		GetMesh()->SetHiddenInGame(true);
		//bRenderInMainPass�� false�� �ϰԵǸ� Rendering�� ���� ������, Shadow�� ������ �ȴ�.
		// �̰� ����Ϸ��� Mesh�� Show�Ǿ�� �Ѵ�. SetHiddenInGame�� True�� �ϸ� �ȵ�.	
		GetMesh()->SetRenderInMainPass(false);

	

		UCustomPDA* CPDA;
		/* ������ ��� ��� HIde�ϱ� */
		for (auto Equipped : Equipment->EquipmentItems)
		{
			if (Equipped && Equipped->Equipment)
			{
				CPDA = Cast<UCustomPDA>(Equipped->Equipment->ItemSetting.DataAsset);

				if (CPDA && 
					(CPDA->EquipmentType == EEquipmentType::EET_Rifle
					|| CPDA->EquipmentType == EEquipmentType::EET_Pistol))
				{
					continue;
				}
				Equipped->Equipment->SKMesh->SetRenderInMainPass(false);
				//Equipped->Equipment->SetActorHiddenInGame(true); //SetHidden(true);
			}
		}

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
				//���� SpringArm�� ���̸� �����Ѵ�.
				BeforeCameraLength = CameraBoom->TargetArmLength;
				//float CameraLength = FMath::FInterpTo(BeforeCameraLength, MINCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);
				CameraBoom->TargetArmLength = MINCameraLength + 20.f;
				CameraTPS->SetRelativeLocation(TPSCam_Aim_Rel_Location);
				CameraTPS->FieldOfView = 77.f;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//��Ȯ�� ��ġ�� �޾ƿ��� ���� Animation�� ����.
				//FPMesh->bPauseAnims = true;

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
				//FPMesh->SetRelativeTransform(OffsetWithoutX); //�ӽ÷� �ּ�ó��
				//CameraFPS->FieldOfView = 75.f;
				//CameraFPS->SetWorldTransform(SightTransform);
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
				//if (EquippedWeapon)
				//{
				//	GetCharacterMovement()->bOrientRotationToMovement = false;
				//	bUseControllerRotationYaw = true; //false; //3��Ī�� Aim���°� �ƴϸ�, Yaw�� Ǯ���ش�.
				//}
				//else
				//{
				//	GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
				//	bUseControllerRotationYaw = false; //3��Ī�� Aim���°� �ƴϸ�, Yaw�� Ǯ���ش�.
				//}



				//���� ī�޶� �ٽ� ���� ��Ų��.
				/*float CurrentLength = CameraBoom->TargetArmLength;
				float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
				CameraBoom->TargetArmLength = BeforeCameraLength;
				CameraTPS->SetRelativeLocation(TPSCam_Rel_Location);
				CameraTPS->FieldOfView = BaseCamTPSfov;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//FPMesh->bPauseAnims = false;
				//FPMesh->SetRelativeTransform(FPMeshOriginTransform);

				CameraFPS->FieldOfView = BaseCamFPSfov;
				
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
	bMoveForward = false;
	if (MainController != NULL && Value != 0.f)
	{
		bMoveForward = true;
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
	bMoveRight = false;
	if (MainController != NULL && Value != 0.f)
	{
		bMoveRight = true;
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		//right ����
		FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
		/*UE_LOG(LogTemp, Warning, TEXT("my  RigVec = %s"), *Direction.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Actor::RigVec = %s"), *GetActorRightVector().ToString());*/
	}
}

void AMainCharacter::Sprint()
{
	//FTimerHandle Timer;
	if(GetWorldTimerManager().IsTimerActive(T_SprintKeyDown) == false &&
	bIsCrouched ==false && bDisableInput == false && GetCharacterMovement()->IsFalling() == false && bIsAim == false )
	{
		bSprintKeyDown = true;
		//Up Timer �ʱ�ȭ
		GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
		GetWorldTimerManager().SetTimer(T_SprintKeyDown,[&]
		{
			StatManagementComponent->StaminaManage(bSprintKeyDown);
		},GetWorld()->GetDeltaSeconds(),true);
	}
}
void AMainCharacter::UnSprint()
{
	if (GetWorldTimerManager().IsTimerActive(T_SprintKeyUp) == false)
	{
		bSprintKeyDown = false;
		//Down Timer �ʱ�ȭ
		GetWorldTimerManager().ClearTimer(T_SprintKeyDown);

		GetWorldTimerManager().SetTimer(T_SprintKeyUp, [&]
		{
				StatManagementComponent->StaminaManage(bSprintKeyDown);
		}, GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMainCharacter::ClearSprintUpTimer()
{
	GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
}


void AMainCharacter::MyJump()
{
	if (bDisableInput == false)
	{
		bIsJumpKeyDown = true;
		Super::Jump();
	}
}
void AMainCharacter::MyStopJumping()
{
	bIsJumpKeyDown = false;
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
			if (EquippedWeapon->bIsHighReady == false && bIsSprinting == false)
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
	OnGetAmmo.Broadcast(EquippedWeapon);
}

void AMainCharacter::EKeyDown()
{
	if (bDisableInput == false)
	{
		Interactive();
	}
}

/*************************  Weapon, Item ���� ***************************************************/
void AMainCharacter::ChangeWeapon(int32 index)
{
	Super::ChangeWeapon(index);

	//FPMode�� AnimUpdate�� ���� �����Ѵ�.
	switch (index)
	{
	case 0:
		FPAnimInstance->WeaponTypeNumber = 0;
		break;
	case 1:
		if (PrimaryWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
		}
		break;
	case 2:
		if (SubWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
		}
		break;
	case 3:
		if (PistolWeapon)
		{
			FPAnimInstance->WeaponTypeNumber = 2;			
		}
		break;
	}

	//For WeaponStatus Widget
	OnChangeWeapon.Broadcast(EquippedWeapon);
	OnGetAmmo.Broadcast(EquippedWeapon);
}

/*************************  Interaction ���� ***************************************************/

/* Static�� �����ؼ� FHitResult�� ���� */
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(10.f, 10.f); //��¦ �а� �ؼ� Ž���� ���� �ߴ�.
	Params.AddIgnoredActor(this);

	//SweepSingle�� ����, ĸ�������� ������� LineTrace.	
	GetWorld()->SweepSingleByChannel(Hit, StartLo, EndLo, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, Shape, Params); 
	
	/* debug */
	//DrawDebugLine(GetWorld(), StartLo, EndLo, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
	//DrawDebugCapsule(GetWorld(), (StartLo+EndLo)/2, Shape.GetCapsuleHalfHeight(),Shape.GetCapsuleRadius(),FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Green, false, 0.8f, (uint8)nullptr, 2.f);

	return Hit;
}


void AMainCharacter::SetInteractActor(AActor* Actor)
{
	InteractActor = Actor;

	IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
	if (InterfaceActor)
	{
		InterfaceActor->SetOutline();

		//InteractActor�� Static Mesh�� ��ġ�� �������� �Ÿ��� ���Ѵ�.
		if((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
		{
			MainController->ControlInteractText(true);
		}

		/* debug */
		//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Valid"));
	}			
}

void AMainCharacter::UnsetInteractActor()
{
	if (InteractActor)
	{
		IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
		if(InterfaceActor)
		{
			InterfaceActor->UnsetOutline();
		}
		InteractActor = nullptr;
		MainController->ControlInteractText(false);
	}
	/* debug */
	//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Invalid"));
}

void AMainCharacter::Interactive()
{
	if (InteractActor)
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(InteractActor);
		if (Interface)
		{
			if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
			{

				Interface->Interaction(this);
			}
		}
	}
	
}



void AMainCharacter::QuickSlotNum4()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N4);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N4);
}
void AMainCharacter::QuickSlotNum5()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N5);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N5);
}
void AMainCharacter::QuickSlotNum6()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N6);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N6);
}
void AMainCharacter::QuickSlotNum7()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N7);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N7);
}
void AMainCharacter::QuickSlotNum8()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N8);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N8);
}
void AMainCharacter::QuickSlotNum9()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N9);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N9);
}