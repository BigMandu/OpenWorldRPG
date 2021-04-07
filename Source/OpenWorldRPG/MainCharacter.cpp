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
//#include "Perception/AISense.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //ȸ���ӵ�
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.2f;


	/* ī�޶� ���� */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->bUsePawnControlRotation = true;


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	/* ȸ���� ī�޶󿡸� ���� ������ ���� */
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	StimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	/*PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);*/

}


// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	if (MainAnimInstance)
	{
		MainAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound);
	}

	StimuliSourceComp->bAutoRegister = true;
	StimuliSourceComp->RegisterForSense(Sight);
	StimuliSourceComp->RegisterForSense(Hearing);
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//bIsAccelerating
	FVector VecLength = FVector(0.f);
	if (GetCharacterMovement()->GetCurrentAcceleration().Size() > VecLength.Size())
	{
		bIsAccelerating = true;
	}
	else
	{
		bIsAccelerating = false;
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	//�� delta���� base rate��ŭ ȸ��
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
		//����������� ���� ã��
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward���͸� ���Ѵ�.
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		//�� �������� value��ŭ ����.
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != NULL && Value != 0.f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);
		
		//right ����
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

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
	bool bResult = false;
	/*FCollisionObjectQueryParams���� 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)�� ����ҰŴ�.
	* �̸� ����ϱ� ���ؼ��� To do this, use ECC_TO_BITFIELD to convert to bit field �̷��� �϶�� �Ѵ�.
	*/
	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation()
		, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_WorldStatic))
		, FCollisionQueryParams(FName(TEXT("SightSense")), true, IgnoreActor));

	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player:: Catch"));
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;
		UE_LOG(LogTemp, Warning, TEXT("OutSeenLocation : %s, OutSightStrength : %f"), *OutSeenLocation.ToString(), OutSightStrength);
		bResult = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player:: Hiding"));
		OutSightStrength = 1;
	}
	
	return bResult;
}