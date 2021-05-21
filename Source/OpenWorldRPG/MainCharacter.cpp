// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon.h"
#include "Item/Item.h"
#include "Item/InventoryComponent.h"
#include "Item/Interactable.h"
#include "Item/Interactive_Interface.h"
#include "DrawDebugHelpers.h" //������

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/******** Movement ********/
	GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //ȸ���ӵ�
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //��ũ���⸦ �� �� �ֵ��� true�� ���ش�.
	GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //��ũ�� ũ�⸦ �⺻HalfHeight�� /1.6���� �����Ѵ�.
	
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //�ȱ� �⺻������ false.
	

	/********** Input ***********/
	bCrouchToggle = true; //��ũ���� Ű Toggle�� true�� �⺻���� �Ѵ�.
	bWalkToggle = true; //�ȱ� Ű Toggle�� true�� �����Ѵ�.
	bAimToggle = true; //����Ű�� toggel true����.

	bIsAim = false;
	bTapKeyDown = false;

	/* ī�޶� ���� */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetMesh(), FName("headsocket"));

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = false;
	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3��Ī ī�޶� ��¦ �������� ġ������ �Ѵ�.

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	/****** Inventory ****/
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	/********** �ʱ� enum ����************/
	SetCameraMode(ECameraMode::ECM_TPS); //�ʱ� ī�޶� ���� 3��Ī ����.
	SetAimMode(EAimMode::EAM_NotAim);

	/******  Perception ****/
	StimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/************** Movement & sight key bind ***************/
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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

	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AMainCharacter::TapKeyDown);

	PlayerInputComponent->BindAction("Interactive", IE_Pressed, this, &AMainCharacter::EKeyDown);
	PlayerInputComponent->BindAction("Interactive", IE_Released, this, &AMainCharacter::EKeyUp);

}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); //Player�� AnimInstance�� �ҷ��´�.
	if (MainAnimInstance)
	{
		MainAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); //AnimInstance�� StepSound_Notify���� ȣ��.
	}

	/**** Perception StimuliSource ���� (Sight, Hearing Sense) ****/	
	StimuliSourceComp->bAutoRegister = true;
	StimuliSourceComp->RegisterForSense(Sight);
	StimuliSourceComp->RegisterForSense(Hearing);



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

	//bIsAccelerating ����.
	FVector VecLength = FVector(0.f);
	if (GetVelocity().Size() > VecLength.Size())
	{
		bIsAccelerating = true;
	}
	else
	{
		bIsAccelerating = false;
	}


}


/***************************** enum �Լ�********************************/
void AMainCharacter::SetMainCharacterStatus(EPlayerStatus Type) //�÷��̾��� ����
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

void AMainCharacter::SetCameraMode(ECameraMode Type) //�÷��̾� ���� ���� -> VKeyDN
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
		if (EquippedWeapon)
		{
			bUseControllerRotationYaw = true;
		}
		else bUseControllerRotationYaw = false; //-> ����������϶��� false, ��������϶��� true�� ����߰ڴ�.


		break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//ī�޶� ȸ���� ĳ���Ͱ� ȸ���� ���� ȸ���ϵ��� �Ѵ�. �׷��� �ް���, �¿� �԰����� �����ϴ�.
		bUseControllerRotationYaw = true;


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
			//TPS��� + Aim�����϶� ī�޶� ��¦ ������ �����ش�.

			BeforeCameraLength = CameraBoom->TargetArmLength; //���� SprintArm�� ���̸� �����Ѵ�.
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
			//���� ī�޶� �ٽ� ���� ��Ų��.
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
	}
}

void AMainCharacter::MyCrouch()
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

void AMainCharacter::MyUnCrouch()
{
	//Crouch�� Toggle�� ������� �������� Key Release  bind�Լ��� ȣ���Ѵ�.
	if (bCrouchToggle == false && bIsCrouched)
	{
		Super::UnCrouch();
		UE_LOG(LogTemp, Warning, TEXT("UnCrouch:: UnCrouch"));
		SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
	}
}

void AMainCharacter::Walk()
{
	
	if (bWalkToggle && bIsWalking && AimMode != EAimMode::EAM_Aim)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk:: UnWalk"));
		SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
		//bIsWalking = false; -> Set enum�Լ����� ���������.
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

void AMainCharacter::UnWalk()
{
	if (bWalkToggle == false && bIsWalking == true && AimMode != EAimMode::EAM_Aim)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnWalk:: UnWalk"));
		SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
		//bIsWalking = false;		
	}
}

void AMainCharacter::ScrollDN()
{
	//TPS����� ��쿡�� ��ũ�� �ٿ�, ���� �����ϵ��� (ī�޶� �ְ� , ������)
	if (CameraMode == ECameraMode::ECM_TPS) 
	{
		if (CameraBoom->TargetArmLength <= MINCameraLength)
		{
			CameraBoom->TargetArmLength = MINCameraLength;
		}
		else CameraBoom->TargetArmLength -= 50;
	}
}

void AMainCharacter::ScrollUP()
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

void AMainCharacter::VKeyDN()
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


/************** Interactive & Inventory Key bind �Լ� ***********/
void AMainCharacter::RMBDown()
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

void AMainCharacter::RMBUp()
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


void AMainCharacter::TapKeyDown()
{
	bTapKeyDown = true;
	//GetController()->
}

void AMainCharacter::EKeyDown()
{
	if (OverlappingActor)
	{
		AWeapon* Weapon = Cast<AWeapon>(OverlappingActor);
		if (Weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player:: EKeyDown -> Weapon. Weapon is : %s"), *(Weapon->GetFName().ToString()));
			SetOverlappingActor(nullptr);
			EquippedWeapon = Weapon;
			Weapon->Equip(this);
		}

	}
	Interactive();
}

void AMainCharacter::EKeyUp()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player:: E Key Up"));
}

/*************************  Item  ���� ***************************************************/

void AMainCharacter::Interactive()
{

	FVector Start;
	FVector End;

	switch (CameraMode) //ī�޶� ��忡 ���� Line Trace ����.
	{
	case ECameraMode::ECM_FPS:
		Start = CameraFPS->GetComponentLocation();
		End = Start + CameraFPS->GetComponentRotation().Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
	{
		const USkeletalMeshSocket* Head = GetMesh()->GetSocketByName(FName("headsocket"));
		check(Head)
			Start = GetMesh()->GetSocketLocation(FName("headsocket"));
		End = Start + CameraTPS->GetComponentRotation().Vector() * 500.f;
		break;
	}
	default:
		break;
	}

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);

	bool bInteractable = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
	if (bInteractable)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			IInteractive_Interface* Interface = Cast<IInteractive_Interface>(HitActor); //AInteractable* InteractActor = Cast<AInteractable>(HitActor);
			if (Interface) 
			{
				InteractActor = HitActor;
				Interface->Interaction(InteractActor);
				//UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *InteractActor->GetName());
			}
			
			
		}
		
	}
}


void AMainCharacter::UseItem(class AItem* Item)
{
	if (Item)
	{
		Item->Use(this);
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
	if (Head) //Head socket�� ������ �� socket�� ��ġ��, 
	{
		PlayerLocation = GetMesh()->GetSocketLocation(FName("headsocket"));
	}
	else PlayerLocation = GetActorLocation(); //������ Actor�� ��ġ��(�����)


	/* FCollisionObjectQueryParams���� 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)�� ����ҰŴ�.
	* �̸� ����ϱ� ���ؼ��� To do this, use ECC_TO_BITFIELD to convert to bit field �̷��� �϶�� �Ѵ�.        */

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