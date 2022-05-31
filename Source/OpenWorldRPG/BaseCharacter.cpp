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

	GetCharacterMovement()->bOrientRotationToMovement = true; //������ ���� = ����������� ����
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //ȸ���ӵ�
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //��ũ���⸦ �� �� �ֵ��� true�� ���ش�.
	GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //��ũ�� ũ�⸦ �⺻HalfHeight�� /1.6���� �����Ѵ�.

	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //�ȱ� �⺻������ false.


	
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
		/* ����� TP Animation�� �������� ����Ѵ�. */ //AnimInstance�� StepSound_Notify���� ȣ��.
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
			// ���� �����ϰ� �ִ� ���Ⱑ Primary�� �ٸ���쿡�� ����. ��ġ�ϸ� �Ȱ����� ������ �ʿ䰡 ����.
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

