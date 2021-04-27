// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
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

	MinWalkSpeed = 300.f; //걷는 속도는 Crouch기본속도랑 똑같게 해줬다.
	MaxWalkSpeed = 600.f; //뛰는속도는 600으로 해준다(스프린트 속도 아님)
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //걷기 기본설정은 false.
	

	/********** Input ***********/
	bCrouchToggle = true; //웅크리기 키 Toggle을 true로 기본세팅 한다.
	bWalkToggle = true; //걷기 키 Toggle을 true로 세팅한다.
	

	/* 카메라 관련 */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	SetCameraMode(ECameraMode::ECM_TPS); //초기 카메라 모드는 3인칭 모드로.

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

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);

	PlayerInputComponent->BindAction("ScrollDN", IE_Pressed, this, &AMainCharacter::ScrollDN);
	PlayerInputComponent->BindAction("ScrollUP", IE_Pressed, this, &AMainCharacter::ScrollUP);

	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AMainCharacter::VKeyDN);

	/************** Interactive & Inventory key bind ************/

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
	if (Controller != NULL && Value != 0.f)
	{
		//어느방향으로 갈지 찾고
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward벡터를 구한다.
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		//그 방향으로 value만큼 간다.
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != NULL && Value != 0.f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);
		
		//right 벡터
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MyCrouch()
{
	if (bCrouchToggle && bIsCrouched)
	{
		Super::UnCrouch();
		UE_LOG(LogTemp, Warning, TEXT("Crouch:: UnCrouch"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Normal);
	}

	if (MainAnimInstance->MovementSpeed <= 10 && bIsCrouched == false)
	{
		Super::Crouch();
		UE_LOG(LogTemp, Warning, TEXT("Crouch:: Crouch"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Crouch);
	}
}

void AMainCharacter::MyUnCrouch()
{
	//Crouch를 Toggle로 사용하지 않을때만 Key Release  bind함수를 호출한다.
	if (bCrouchToggle == false && bIsCrouched)
	{
		Super::UnCrouch();
		UE_LOG(LogTemp, Warning, TEXT("UnCrouch:: UnCrouch"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Normal);	
	}
}

void AMainCharacter::Walk()
{
	
	if (bWalkToggle && bIsWalking == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk:: UnWalk"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Normal);
		//bIsWalking = false; -> Set enum함수에서 변경해줬다.
		return;
	}
	
	if (bIsWalking == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk:: Walk"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Walk);
		//bIsWalking = true;
		return;
	}
}

void AMainCharacter::UnWalk()
{
	if (bWalkToggle == false && bIsWalking == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnWalk:: UnWalk"));
		SetMainCharacterStatus(EMainChracterStatus::EMCS_Normal);
		//bIsWalking = false;		
	}
}

void AMainCharacter::ScrollDN()
{
	if (CameraBoom->TargetArmLength <= MINCameraLength)
	{
		CameraBoom->TargetArmLength = MINCameraLength;
	}
	else CameraBoom->TargetArmLength -= 50;

}

void AMainCharacter::ScrollUP()
{
	if (CameraBoom->TargetArmLength >= MAXCameraLength)
	{
		CameraBoom->TargetArmLength = MAXCameraLength;
	}
	else CameraBoom->TargetArmLength += 50;
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

/***************************** enum 함수********************************/
void AMainCharacter::SetMainCharacterStatus(EMainChracterStatus Type) //플레이어의 상태
{
	MainChracterStatus = Type;
	switch (MainChracterStatus)
	{
	case EMainChracterStatus::EMCS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		bIsWalking = false;
		break;
	case EMainChracterStatus::EMCS_Walk:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		bIsWalking = true;
		break;
	case EMainChracterStatus::EMCS_Sprint:
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
		CameraBoom->TargetArmLength = 450.f;
		CameraBoom->bUsePawnControlRotation = true;
		Camera->bUsePawnControlRotation = false;

		/* 회전시 카메라에만 영향 가도록 설정 */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = false;

		BaseTurnRate = 45.f;
		BaseLookupRate = 45.f;
		break;

	case ECameraMode::ECM_FPS:
		CameraBoom->TargetArmLength = 0.f;
		break;
	default:
		break;
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

/************** Interactive & Inventory Key bind 함수 ***********/

void AMainCharacter::EKeyDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Player:: E Key Down"));
	if (OverlappingActor)
	{
		AWeapon* Weapon = Cast<AWeapon>(OverlappingActor);
		if (Weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player:: EKeyDown -> Weapon. Weapon is : %s"), *(Weapon->GetFName().ToString()));
		}
		
	}
}

void AMainCharacter::EKeyUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Player:: E Key Up"));
}