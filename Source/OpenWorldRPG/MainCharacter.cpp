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
#include "DrawDebugHelpers.h" //디버깅용

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	bCrouchToggle = true; //웅크리기 키 Toggle을 true로 기본세팅 한다.
	bWalkToggle = true; //걷기 키 Toggle을 true로 세팅한다.
	bAimToggle = true; //조준키를 toggel true세팅.

	bIsAim = false;
	bTapKeyDown = false;

	/* 카메라 관련 */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetMesh(), FName("headsocket"));

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = false;
	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3인칭 카메라를 살짝 우측으로 치우지게 한다.

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	/****** Inventory ****/
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	/********** 초기 enum 세팅************/
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

	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); //Player의 AnimInstance를 불러온다.
	if (MainAnimInstance)
	{
		MainAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); //AnimInstance의 StepSound_Notify에서 호출.
	}

	/**** Perception StimuliSource 제공 (Sight, Hearing Sense) ****/	
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
		if (EquippedWeapon)
		{
			bUseControllerRotationYaw = true;
		}
		else bUseControllerRotationYaw = false; //-> 비전투모드일때는 false, 전투모드일때는 true로 해줘야겠다.


		break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//카메라 회전시 캐릭터가 회전을 따라서 회전하도록 한다. 그래야 뒷걸음, 좌우 게걸음이 가능하다.
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
	//Crouch를 Toggle로 사용하지 않을때만 Key Release  bind함수를 호출한다.
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


/************** Interactive & Inventory Key bind 함수 ***********/
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

/*************************  Item  관련 ***************************************************/

void AMainCharacter::Interactive()
{

	FVector Start;
	FVector End;

	switch (CameraMode) //카메라 모드에 따라 Line Trace 변경.
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