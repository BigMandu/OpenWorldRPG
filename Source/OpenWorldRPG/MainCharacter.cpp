// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"

#include "Perception/AISightTargetInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"

#include "OpenWorldRPG/Item/Interactive_Interface.h"
//#include "OpenWorldRPG/Item/InventoryComponent.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"

#include "DrawDebugHelpers.h" //디버깅용
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "BoneControllers/AnimNode_ModifyBone.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadSocketName = FName("headsocket");
	GripSocketName = FName("WeaponGrip");
	WeaponLeftHandSocketName = FName("LeftHandPos");


	/******** Movement ********/
	GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //회전속도
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //웅크리기를 할 수 있도록 true로 해준다.
	GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //웅크린 크기를 기본HalfHeight의 /1.6으로 지정한다.
	
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //걷기 기본설정은 false.
	

	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //웅크리기 키 Toggle을 true로 기본세팅 한다.
	bWalkToggle = true; //걷기 키 Toggle을 true로 세팅한다.
	bAimToggle = true; //조준키를 toggel true세팅.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;

	/* 카메라 관련 */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetRootComponent());

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = true;
	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3인칭 카메라를 살짝 우측으로 치우지게 한다.

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	/****** First Person Mesh *****/
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	/*** Weapon Grip socket Relative Lo, Ro ***/
	/*
		Rotation은 주의 해야함.X = Roll, Y = Pitch, Z = Yaw
		Rotation(Y,Z,X)순으로 기입.
	*/
	/*
	FVector(-2.999811, 5.476199, -1.011013));
	FRotator(8.32585, -105.164108, 10.820496).Quaternion()); //->Rifle을 X축에 맞춘 Mesh를 사용하지 않음.
	*/
	/*RifleRelativeLoRo.SetLocation(FVector(0, 0, 0));
	RifleRelativeLoRo.SetRotation(FRotator(200, 200, 200).Quaternion());*/
	
	/*PistolRelativeLoRo.SetLocation(FVector(-3.271253, 5.217403, -1.969826));
	PistolRelativeLoRo.SetRotation(FRotator(10.704832, 163.24559, -8.476412).Quaternion());*/

	/****** Item ****/
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("InventoryComp"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	ActiveInteractDistance = 200.f; //상호작용 아이템이 표시되는 최대거리.

	/********** 초기 세팅 ************/
	SetCameraMode(ECameraMode::ECM_TPS); //초기 카메라 모드는 3인칭 모드로.
	SetAimMode(EAimMode::EAM_NotAim);
	

	/******  Perception ****/
	StimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	
}

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

	PlayerInputComponent->BindAction("ChangeSafetyLever", IE_Pressed, this, &AMainCharacter::ChangeSafetyLever);

	/******** Weapon Quick Swap ***********/
	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AMainCharacter::ChangePrimaryWeapon);
	PlayerInputComponent->BindAction("Sub", IE_Pressed, this, &AMainCharacter::ChangeSubWeapon);
	PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &AMainCharacter::ChangePistolWeapon);

	/************** Interactive & Inventory key bind ************/

	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AMainCharacter::TabKeyDown);

	PlayerInputComponent->BindAction("Interactive", IE_Pressed, this, &AMainCharacter::EKeyDown);

}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Player의 AnimInstance를 불러온다. */
	TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); 
	FPAnimInstance = Cast<UMainAnimInstance>(FPMesh->GetAnimInstance());

	if (TPAnimInstance && FPAnimInstance)
	{
		/* 사운드는 TP Animation을 기준으로 출력한다. */ //AnimInstance의 StepSound_Notify에서 호출.
		TPAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); 


		TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
	}

	/**** Perception StimuliSource 제공 (Sight, Hearing Sense) ****/	
	StimuliSourceComp->bAutoRegister = true;
	StimuliSourceComp->RegisterForSense(Sight);
	StimuliSourceComp->RegisterForSense(Hearing);


	FPMesh->SetHiddenInGame(true);

	/* FPS Aim관련 기본 설정 값 저장*/
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
	
	//bIsLookInput 세팅
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

	//bIsAccelerating 세팅.
	FVector VecLength = FVector(0.f);
	if (GetVelocity().Size() > VecLength.Size())
	{
		bIsAccelerating = true;
	}
	else
	{
		bIsAccelerating = false;
	}


	/* 상호작용 Line Trace 변경 . ver2  아래 코드를 GetPlayerViewPoint함수를 이용해 간략화 시켰다. */
	FRotator Rot;
	MainController->GetPlayerViewPoint(Interact_LineTrace_StartLocation, Rot);
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * (MAXCameraLength + 800.f); //카메라Boom길이보다 더길게 끝나야한다.
	}
	
	/* 카메라 모드에 따른 상호작용 LineTrace 변경. ver 1 --> ver 2로 교체.*/
	/*
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_StartLocation = CameraFPS->GetComponentLocation();
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraFPS->GetComponentRotation().Vector() * 500.f;

		break;
	case ECameraMode::ECM_TPS:
	{
		Interact_LineTrace_StartLocation = CameraTPS->GetComponentLocation(); //TPS카메라를 기준으로 Trace를 시작한다.
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraTPS->GetComponentRotation().Vector() * (MAXCameraLength + 800.f); //카메라Boom길이보다 더길게 끝나야한다.
		break;
	}
	default:
		break;
	}
	*/



	/* 3인칭 모드에서 총이 있고 움직이고 있으면 시점을 방향으로 지정한다.*/
	if (EquippedWeapon && CameraMode == ECameraMode::ECM_TPS)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	

	/* 상호작용 텍스트를 띄움 */
	/* Static FHitResult를 리턴받아서 Interface 변환, 성공하면 Outline과 TEXT를 띄움.*/
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();
	if (HitActor)
	{
		if (InteractActor) //이미 InteractActor가 있고
		{
			if (HitActor != InteractActor) //지금 보고 있는 Actor와 InteractActor가 다르면
			{
				UnsetInteractActor(); //InteractActor를 Unset해준다.
			}
			else
			{
				//거리에 따라 InteractText가 업데이트 되기 때문에 추가 해준다.
				SetInteractActor(HitActor); 
			}
		}
		else //InteractActor가 없을 경우
		{
			AInteractable* Interactable = Cast<AInteractable>(HitActor);
			if (Interactable)
			{
				SetInteractActor(HitActor); //Interactable인 경우 Set을 해준다.
			}
		}
	}
	else
	{
		UnsetInteractActor();
	}
	//InteractActor가 다를경우 UnsetInteractActor 호출하자. (Outline이 안없어지는 버그가 있음.) -> 해결함.

	/* Weapon의 AimInitialize함수에서 사용하는 Timer 해제용 */
	if (EquippedWeapon)
	{
		//타이머가 작동중이고
		if (GetWorldTimerManager().IsTimerActive(EquippedWeapon->AimInitHandle))
		{
			//aim을 돌리거나, AlphaTime이 1초가 넘어가거나, 쏘는 중이라면
			if (bIsLookInput || EquippedWeapon->AlphaTime >= 1.f || EquippedWeapon->bIsFiring)
			{
				//에임 초기화 타이머를 초기화 시킨다.
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


/***************************** enum 함수********************************/
void AMainCharacter::SetMainCharacterStatus(EPlayerStatus Type) //플레이어의 상태
{
	MainChracterStatus = Type;
	switch (MainChracterStatus)
	{
	case EPlayerStatus::EPS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		bIsWalking = false;
		break;
	case EPlayerStatus::EPS_Walk:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		bIsWalking = true;
		break;
	case EPlayerStatus::EPS_Sprint:
		break;
	default:
		break;
	}
}

void AMainCharacter::SetCameraMode(ECameraMode Type) //플레이어 시점 상태 -> VKeyDN
{
	CameraMode = Type;
	switch (CameraMode)
	{
	case ECameraMode::ECM_TPS:
		CameraTPS->Activate();
		CameraFPS->Deactivate();

		/* 회전시 카메라에만 영향 가도록 설정 */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = false;

		/* Character Mesh 설정 */
		FPMesh->SetHiddenInGame(true);  //1인칭 Mesh 숨김
		GetMesh()->SetHiddenInGame(false); //3인칭 Mesh 안숨김

		if (EquippedWeapon) //현재 장착무기가 있으면 First,Third mesh에 따라 소켓에 부착.
		{
			EquippedWeapon->GunAttachToMesh(this);

			//장착 무기가 있으면, 컨트롤러의 회전에 따라 캐릭터를 회전하도록 한다.
			bUseControllerRotationYaw = true;
			//const USkeletalMeshSocket* TPSocket = GetMesh()->GetSocketByName(GripSocketName);
			//if (TPSocket)
			//{
			//	TPSocket->AttachActor(EquippedWeapon, GetMesh());
			//	FTransform socketform = TPSocket->GetSocketLocalTransform();
			//	/* weapon Type에 따른 weapon의 위치,회전 설정 */
			//	if (EquippedWeapon->WeaponType == EWeaponType::EWT_Rifle)
			//	{
			//		TPSocket->RelativeLocation = FVector(1, 1, 1); //RifleRelativeLoRo.GetLocation();
			//		socketform.SetLocation(RifleRelativeLoRo.GetLocation());
			//		socketform.SetRotation(RifleRelativeLoRo.GetRotation());
			//		
			//	}
			//	else
			//	{
			//		socketform.SetLocation(PistolRelativeLoRo.GetLocation());
			//		socketform.SetRotation(PistolRelativeLoRo.GetRotation());
			//		
			//	}
			//}
		}

		break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//카메라 회전시 캐릭터가 회전을 따라서 회전하도록 한다. 그래야 뒷걸음, 좌우 게걸음이 가능하다.
		bUseControllerRotationYaw = true;

		/*  Character Mesh 설정 */
		FPMesh->SetHiddenInGame(false);
		GetMesh()->SetHiddenInGame(true);
		

		/* 장착 무기 Mesh에 부착 */
		if (EquippedWeapon)
		{
			EquippedWeapon->GunAttachToMesh(this);
			//const USkeletalMeshSocket* FPSocket = FPMesh->GetSocketByName(GripSocketName);
			//if (FPSocket)
			//{
			//	FPSocket->AttachActor(EquippedWeapon, FPMesh);
			//	FTransform socketform = FPSocket->GetSocketLocalTransform();
			//	/* weapon Type에 따른 weapon의 위치,회전 설정 */
			//	if (EquippedWeapon->WeaponType == EWeaponType::EWT_Rifle)
			//	{
			//		socketform.SetLocation(RifleRelativeLoRo.GetLocation());
			//		socketform.SetRotation(RifleRelativeLoRo.GetRotation());
			//	}
			//	else
			//	{
			//		socketform.SetLocation(PistolRelativeLoRo.GetLocation());
			//		socketform.SetRotation(PistolRelativeLoRo.GetRotation());
			//	}
			//	
			//}
		}
		break;
	default:
		break;
	}
}

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
				GetCharacterMovement()->bOrientRotationToMovement = false; //움직인 방향을 진행방향으로 설정하지 않는다.

				bUseControllerRotationYaw = true; //3인칭에 Aim상태면 Yaw를 잡아준다.

				//TPS모드 + Aim상태일때 카메라를 살짝 앞으로 땡겨준다.
				BeforeCameraLength = CameraBoom->TargetArmLength; //현재 SprintArm의 길이를 저장한다.
				//float CameraLength = FMath::FInterpTo(BeforeCameraLength, MINCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);
				CameraBoom->TargetArmLength = MINCameraLength;
				
				CameraTPS->SetRelativeLocation(TPSCam_Aim_Rel_Location);
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				/* Aim mode + FPS모드 전용의 소켓을 추가,
				   해당소켓으로 부착시켜 주기 위해 함수를 호출한다.*/
				EquippedWeapon->FPS_AimAttachToMesh(this);
				FPSAimLocationAdjust();
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
				if(EquippedWeapon)
				{
					GetCharacterMovement()->bOrientRotationToMovement = false;
					bUseControllerRotationYaw = true; //false; //3인칭에 Aim상태가 아니면, Yaw를 풀어준다.
				}
				else
				{
					GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
					bUseControllerRotationYaw = false; //3인칭에 Aim상태가 아니면, Yaw를 풀어준다.
				}

				
				
				//땡긴 카메라를 다시 원복 시킨다.
				/*float CurrentLength = CameraBoom->TargetArmLength;
				float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
				CameraBoom->TargetArmLength = BeforeCameraLength;
				CameraTPS->SetRelativeLocation(TPSCam_Rel_Location);
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				/* Aim mode + FPS모드 전용의 소켓을 추가, 부착 시켰기 때문에
				원복을 해준다.*/
				EquippedWeapon->FPS_AimAttachToMesh(this);
				FPSAimLocationAdjust();
			}
			break;
		}
		default:
			break;
		}
	}
}


/****************************** 이동, 시점 관련 키 바인드 ***************************/

void AMainCharacter::TurnAtRate(float Rate)
{
	//매 delta초 마다 base rate만큼 회전
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
		//어느방향으로 갈지 찾고
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward벡터를 구한다.
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		//그 방향으로 value만큼 간다.
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

		//right 벡터
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
			SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
		}

		if (bIsCrouched == false)
		{
			Super::Crouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: Crouch"));
			SetMainCharacterStatus(EPlayerStatus::EPS_Crouch);
		}
	}
}

void AMainCharacter::MyUnCrouch()
{
	//Crouch를 Toggle로 사용하지 않을때만 Key Release  bind함수를 호출한다.
	if (bDisableInput == false)
	{
		if (bCrouchToggle == false && bIsCrouched)
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("UnCrouch:: UnCrouch"));
			SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
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
			SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			//bIsWalking = false; -> Set enum함수에서 변경해줬다.
			return;
		}

		if (bIsWalking == false && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: Walk"));
			SetMainCharacterStatus(EPlayerStatus::EPS_Walk);
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
			SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			//bIsWalking = false;		
		}
	}
}

void AMainCharacter::ScrollDN()
{
	if (bDisableInput == false)
	{
		//TPS모드일 경우에만 스크롤 다운, 업이 가능하도록 (카메라 멀게 , 가까이)
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

void AMainCharacter::FPSAimLocationAdjust()
{
	if (CameraFPS)
	{
		if (bIsAim)
		{
			//각 총마다 FPMesh의 올바른 위치에 부착하기 위해
			//Weapon Class에 Transform 변수를 만들어줬다.
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


/************** Interactive & Inventory Key bind 함수 ***********/
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

/*************************  Weapon, Item 관련 ***************************************************/

void AMainCharacter::UseItem(class AActor* Item)
{
	if (Item)
	{
		//Item->Use(this);
	}
}

void AMainCharacter::ChangeSafetyLever()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ChangeSafetyLever();
	}
}

void AMainCharacter::ChangeWeapon(int32 index)
{
	switch (index)
	{
	case 0:
		TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
		break;
	case 1:
		// 현재 장착하고 있는 무기가 Primary와 다를경우에만 변경. 일치하면 똑같은걸 장착할 필요가 없음.
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

void AMainCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		EquippedWeapon = Weapon;
		//EquippedWeapon->OnBeginHighReady.AddDynamic(this, &UMainAnimInstance::BeginHighReady);
		//EquippedWeapon->OnEndHighReady.AddDynamic(this, &AMainCharacter::EndHighReady);
		
	}
}



/* Rifle, Pistol의 LeftHand 위치 */
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

//void AMainCharacter::BeginHighReady()
//{
//	
//	const USkeletalMeshSocket* TP_ArmLeft = GetMesh()->GetSocketByName("upperarm_l");
//	const USkeletalMeshSocket* TP_ArmRight = GetMesh()->GetSocketByName("upperarm_r");
//
//	const USkeletalMeshSocket* FP_Arms = FPMesh->GetSocketByName("spine_03");
//	/*const USkeletalMeshSocket* FP_ArmLeft = FPMesh->GetSocketByName("UpperArm_L");
//	const USkeletalMeshSocket* FP_ArmRight = FPMesh->GetSocketByName("UpperArm_R");*/
//
//	FRotator NewRot = FRotator(0.f, 0.f, 200.f);
//
//	//TP_ArmLeft->RelativeRotation = NewRot;
//	FAnimNode_ModifyBone ModBone;
//	const FBoneReference Bone("upperarm_l");
//	ModBone.BoneToModify = Bone.BoneName;
//	ModBone.Rotation = FRotator(0.f, 0.f, 120.f);
//}
//
//void AMainCharacter::EndHighReady()
//{
//	
//}



/*************************  Interaction 관련 ***************************************************/

/* Static을 감지해서 FHitResult를 리턴 */
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(5.f, 5.f); //살짝 넓게 해서 탐지가 쉽게 했다.
	Params.AddIgnoredActor(this);

	 
	//GetWorld()->LineTraceSingleByChannel(Hit, StartLo, EndLo, ECollisionChannel::ECC_WorldStatic, Params); // LineTraceSingle에서 
	
	GetWorld()->SweepSingleByChannel(Hit, StartLo, EndLo, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, Shape, Params); //SweepSingle로 변경, 캡슐형태의 모양으로 LineTrace.
	
	/* debug */
	//DrawDebugLine(GetWorld(), StartLo, EndLo, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
	//DrawDebugCapsule(GetWorld(), (StartLo+EndLo)/2, Shape.GetCapsuleHalfHeight(),Shape.GetCapsuleRadius(),FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Green, false, 0.8f, (uint8)nullptr, 2.f);

	return Hit;
}


void AMainCharacter::SetInteractActor(AActor* Actor)
{
	//MainController->ShowInteractText(); //특정거리내에 Text가 뜨도록 변경함. 아래 코드.
	//if (InteractActor == nullptr) //붙어있으면 바뀌지 않기때문에 조건문 삭제.
	{
		InteractActor = Actor;
		AInteractable* InActor = Cast<AInteractable>(InteractActor);
		if (InActor)
		{
			InActor->SetOutline();

			//InteractActor의 Static Mesh의 위치를 기준으로 거리를 구한다.

			//InteractActor->GetActorLocation()
			if ((GetActorLocation() - InActor->Mesh->GetComponentLocation()).Size() <= ActiveInteractDistance) //특정거리 이내면 Text Show.
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
		AInteractable* InActor = Cast<AInteractable>(InteractActor);
		if (InActor)
		{
			InActor->UnsetOutline();
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
		AInteractable* InActor = Cast<AInteractable>(InteractActor);
		if (Interface && InActor)
		{
			if ((GetActorLocation() - InActor->Mesh->GetComponentLocation()).Size() <= ActiveInteractDistance)
			{
				//Interface->OnInteract.Broadcast();
				Interface->Interaction(this);
			}
		}
	}
	
}

/******************************************************************************/
void AMainCharacter::StepSound()
{
	//UE_LOG(LogTemp, Warning, TEXT("AnimNotify -> AMainChar:: StepSound()"));

	if (StepSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSoundCue, GetActorLocation());
	}
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);

}

//bool AMainCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const

bool AMainCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	FHitResult HitResult;
	FVector PlayerLocation;
	bool bResult = false;

	const USkeletalMeshSocket* Head = GetMesh()->GetSocketByName(HeadSocketName);
	if (Head) //Head socket이 있으면 이 socket의 위치를, 
	{
		PlayerLocation = GetMesh()->GetSocketLocation(HeadSocketName);
	}
	else PlayerLocation = GetActorLocation(); //없으면 Actor의 위치를(정가운데)


	/* FCollisionObjectQueryParams에는 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)를 사용할거다.
	* 이를 사용하기 위해서는 To do this, use ECC_TO_BITFIELD to convert to bit field 이렇게 하라고 한다.        */

	//WorldDynamic, WorldStatic, IgnoreActor를 (관측자의 위치에서 Player의 위치의 범위) LineTrace로 감지. 
	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, PlayerLocation
		, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_WorldStatic))
		, FCollisionQueryParams(FName(TEXT("SightSense")), true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	//위에서 감지된게 아니거나, Actor가 Player라면
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


