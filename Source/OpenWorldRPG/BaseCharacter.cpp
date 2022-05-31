// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "MainAnimInstance.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"



// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //회전속도
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //웅크리기를 할 수 있도록 true로 해준다.
	GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //웅크린 크기를 기본HalfHeight의 /1.6으로 지정한다.

	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //걷기 기본설정은 false.


	
	InventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("InventoryComp"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	if (TPAnimInstance)
	{
		/* 사운드는 TP Animation을 기준으로 출력한다. */ //AnimInstance의 StepSound_Notify에서 호출.
		TPAnimInstance->StepSound.AddUObject(this, &ABaseCharacter::StepSound);
		TPAnimInstance->WeaponTypeNumber = 0;
	}

	SetTeamType(TeamType);
}

void ABaseCharacter::SetTeamType(ETeamType Team)
{
	TeamType = Team;
}

void ABaseCharacter::SetCharacterStatus(ECharacterStatus Type)
{
	ChracterStatus = Type;
	switch (ChracterStatus)
	{
	case ECharacterStatus::EPS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		bIsWalking = false;
		break;
	case ECharacterStatus::EPS_Walk:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		bIsWalking = true;
		break;
	case ECharacterStatus::EPS_Sprint:
		break;
	default:
		break;
	}
}

void ABaseCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		EquippedWeapon = Weapon;
	}
}


void ABaseCharacter::UseItem(class AActor* Item)
{
	if (Item)
	{
		//Item->Use(this);
	}
}

void ABaseCharacter::ChangeSafetyLever()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ChangeSafetyLever();
	}
}

void ABaseCharacter::ChangeWeapon(int32 index)
{
	if (TPAnimInstance)
	{
		switch (index)
		{
		case 0:
			TPAnimInstance->WeaponTypeNumber = 0;
			//FPAnimInstance->WeaponTypeNumber = 0;
			break;
		case 1:
			// 현재 장착하고 있는 무기가 Primary와 다를경우에만 변경. 일치하면 똑같은걸 장착할 필요가 없음.
			if (PrimaryWeapon && (PrimaryWeapon != EquippedWeapon))
			{
				PrimaryWeapon->GunAttachToMesh(this);
				if (TPAnimInstance && FPAnimInstance)
				{
					TPAnimInstance->WeaponTypeNumber = 1;
					//FPAnimInstance->WeaponTypeNumber = 1;
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
}

void ABaseCharacter::ChangePrimaryWeapon()
{
	ChangeWeapon(1);
}
void ABaseCharacter::ChangeSubWeapon()
{
	ChangeWeapon(2);
}
void ABaseCharacter::ChangePistolWeapon()
{
	ChangeWeapon(3);
}




FTransform ABaseCharacter::LeftHandik()
{
	FTransform Transform;
	if (EquippedWeapon)
	{
		Transform = EquippedWeapon->SKMesh->GetSocketTransform(WeaponLeftHandSocketName, ERelativeTransformSpace::RTS_World);
		return Transform;
	}

	return Transform;
}

void ABaseCharacter::StepSound()
{
	if (StepSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSoundCue, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
	}
}

bool ABaseCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible,
	int32* UserData) const
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


	// FCollisionObjectQueryParams에는 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)를 사용할거다.
	// 이를 사용하기 위해서는 To do this, use ECC_TO_BITFIELD to convert to bit field 이렇게 하라고 한다. 

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

