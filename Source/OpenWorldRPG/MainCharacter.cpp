// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Item/InventoryComponent.h"
#include "Item/Interactive_Interface.h"
#include "Item/Interactable.h"
#include "Item/Weapon.h"

#include "DrawDebugHelpers.h" //디버깅용

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadSocketName = FName("headsocket");
	WeaponGripSocketName = FName("WeaponGrip");


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

	/****** Item ****/
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	ActiveInteractDistance = 200.f;

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


	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBUp);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);

	PlayerInputComponent->BindAction("ScrollDN", IE_Pressed, this, &AMainCharacter::ScrollDN);
	PlayerInputComponent->BindAction("ScrollUP", IE_Pressed, this, &AMainCharacter::ScrollUP);

	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AMainCharacter::VKeyDN);

	/************** Interactive & Inventory key bind ************/

	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AMainCharacter::TabKeyDown);

	PlayerInputComponent->BindAction("Interactive", IE_Pressed, this, &AMainCharacter::EKeyDown);
	PlayerInputComponent->BindAction("Interactive", IE_Released, this, &AMainCharacter::EKeyUp);

}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); //Player의 AnimInstance를 불러온다.
	if (MainAnimInstance)
	{
		MainAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); //AnimInstance의 StepSound_Notify에서 호출.
	}

	/**** Perception StimuliSource 제공 (Sight, Hearing Sense) ****/	
	StimuliSourceComp->bAutoRegister = true;
	StimuliSourceComp->RegisterForSense(Sight);
	StimuliSourceComp->RegisterForSense(Hearing);

	



FPMesh->SetHiddenInGame(true);

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

	/* 카메라 모드에 따라 상호작용 LineTrace 변경 */
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_StartLocation = CameraFPS->GetComponentLocation();
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraFPS->GetComponentRotation().Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
	{
		FVector CamLocation = CameraTPS->GetComponentLocation();
		Interact_LineTrace_StartLocation = CamLocation; //TPS카메라를 기준으로 Trace를 시작한다.
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraTPS->GetComponentRotation().Vector() * (MAXCameraLength + 800.f); //카메라Boom길이보다 더길게 끝나야한다.
		break;
	}
	default:
		break;
	}

	
	
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();
	if (HitActor)
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(HitActor);
		if (Interface)
		{
			SetInteractActor(HitActor);
		}
		else
		{
			UnsetInteractActor();
		}
	}
	else
	{
		UnsetInteractActor();
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

		//->비전투모드, 전투모드는 없애기로하고, TPS일때 Aim mode일때만 Rotation Yaw를 true로 변경해줌.
		/*
		if (EquippedWeapon)
		{
			bUseControllerRotationYaw = true;
		}
		else bUseControllerRotationYaw = false; //-> 비전투모드일때는 false, 전투모드일때는 true로 해줘야겠다. -> Aim, NotAim상태로만 변경이 되도록 했다.
		*/

		/* Character Mesh 설정 */
		FPMesh->SetHiddenInGame(true);  //1인칭 Mesh 숨김
		GetMesh()->SetHiddenInGame(false); //3인칭 Mesh 안숨김

		if (EquippedWeapon) //현재 장착무기가 있으면 First,Third mesh에 따라 소켓에 부착.
		{
			const USkeletalMeshSocket* TPSocket = GetMesh()->GetSocketByName("WeaponGrip");
			if (TPSocket)
			{
				TPSocket->AttachActor(EquippedWeapon, GetMesh());
			}
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
			const USkeletalMeshSocket* FPSocket = FPMesh->GetSocketByName("WeaponGrip");
			if (FPSocket)
			{
				FPSocket->AttachActor(EquippedWeapon, FPMesh);
			}
		}
		break;
	default:
		break;
	}
}

void AMainCharacter::SetAimMode(EAimMode Mode)
{
	AimMode = Mode;
	switch (AimMode)
	{
	case EAimMode::EAM_Aim:
	{
		bIsAim = true;
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			bUseControllerRotationYaw = true; //3인칭에 Aim상태면 Yaw를 잡아준다.
			

			//TPS모드 + Aim상태일때 카메라를 살짝 앞으로 땡겨준다.
			BeforeCameraLength = CameraBoom->TargetArmLength; //현재 SprintArm의 길이를 저장한다.
			//float CameraLength = FMath::FInterpTo(BeforeCameraLength, MINCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);
			CameraBoom->TargetArmLength = MINCameraLength;
		}
		break;
	}
	case EAimMode::EAM_NotAim:
	{
		bIsAim = false;
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			bUseControllerRotationYaw = false; //3인칭에 Aim상태가 아니면, Yaw를 풀어준다.
			//땡긴 카메라를 다시 원복 시킨다.
			/*float CurrentLength = CameraBoom->TargetArmLength;
			float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
			CameraBoom->TargetArmLength = BeforeCameraLength;
		}
		break;
	}
	default:
		break;
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

		if (MainAnimInstance->MovementSpeed <= 10 && bIsCrouched == false)
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
			else CameraBoom->TargetArmLength -= 50;
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
			else CameraBoom->TargetArmLength += 50;
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


/************** Interactive & Inventory Key bind 함수 ***********/
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

void AMainCharacter::EKeyUp()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player:: E Key Up"));
}

/*************************  Interaction 관련 ***************************************************/

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
	UE_LOG(LogTemp, Warning, TEXT("InteractActor is Valid"));
	//MainController->ShowInteractText(); //특정거리내에 Text가 뜨도록 변경.
	//if (InteractActor == nullptr) //거리를 계속해서 update해야하므로 조건제거.
	{
		InteractActor = Actor;
		AInteractable* InActor = Cast<AInteractable>(InteractActor);
		if (InActor)
		{
			InActor->SetOutline();
		}
		if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance) //특정거리 이내면 Text Show.
		{
			MainController->ShowInteractText();
		}
	}
}

void AMainCharacter::UnsetInteractActor()
{
	//UE_LOG(LogTemp, Warning, TEXT("InteractActor is Invalid"));
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
}

void AMainCharacter::Interactive()
{
	if (InteractActor)
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(InteractActor);
		if (Interface && (GetActorLocation()-InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
		{
			Interface->Interaction(this);
		}
	}
	
}


void AMainCharacter::UseItem(class AActor* Item)
{
	if (Item)
	{
		//Item->Use(this);
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


bool AMainCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	FHitResult HitResult;
	FVector PlayerLocation;
	bool bResult = false;

	const USkeletalMeshSocket* Head = GetMesh()->GetSocketByName(FName("headsocket"));
	if (Head) //Head socket이 있으면 이 socket의 위치를, 
	{
		PlayerLocation = GetMesh()->GetSocketLocation(FName("headsocket"));
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


FTransform AMainCharacter::LeftHandik()
{
	FTransform Transform;
	if (EquippedWeapon)
	{
		FVector LeftTransVector;
		FRotator LeftTransRotation;
		
		Transform = EquippedWeapon->WeaponMesh->GetSocketTransform(FName("LeftHandPos"), ERelativeTransformSpace::RTS_World);
		return Transform;
	}
	
	return Transform;
}