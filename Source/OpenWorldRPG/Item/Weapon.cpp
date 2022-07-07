// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "EquipmentComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Curves/CurveFloat.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h" //������
//#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "OpenWorldRPG/MainAnimInstance.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"


#define DEBUG 0

AWeapon::AWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetupAttachment(RootComponent);	

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;
	RifleAssign = ERifleAssign::ERA_MAX;

	bIsFiring = false;
	bLMBDown = false;
	bDetectLookInput = false;

	//MuzzleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffectComp"));
	//MuzzleEffectComp->SetupAttachment(GetRootComponent());
	MuzzleFlashSocketName = FName("muzzleflash");
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/* New Aim System, */
	if (MainCon != nullptr ) //if (OwningPlayer != nullptr)
	{
		if (MainCon->Main->EquippedWeapon == this)
		{
			UpdateAim();
			WeaponClipping();

			//DrawDebugLine(GetWorld(), GetActorForwardVector(), GetActorLocation() + GetActorRotation().Vector() * 100.f, FColor::Cyan, false, 1.f, 0, 2.f);
		}
	}
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CapsuleComp->SetHiddenInGame(false); //for debug

	//CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnCollisionBegin);
	//CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnCollisionEnd);

	//CapsuleComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); //Weapon�ݸ��� Object�� ����.
	//CapsuleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	//CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

bool AWeapon::StepEquip(AActor* Char)
{
	Super::StepEquip(Char);
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Char);
	check(BChar);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	UE_LOG(LogTemp, Warning, TEXT("Weapon Transform : %s"), *OriginalWeaponTransform.ToString());

	if (RifleAssign == ERifleAssign::ERA_MAX)
	{
		/* 1,2,3�� �������� Quick Swap�ϱ� ���� */
		if (EquipmentType == EEquipmentType::EET_Rifle) //�������̰�
		{
			if (BChar->PrimaryWeapon) //�̹� �ֹ��Ⱑ ������
			{
				RifleAssign = ERifleAssign::ERA_Sub;
				BChar->SubWeapon = this; //�ι���� ����
			}
			else //�ֹ��Ⱑ ������
			{
				RifleAssign = ERifleAssign::ERA_Primary;
				BChar->PrimaryWeapon = this; //�ֹ����
			}
		}
		else //�ǽ���
		{
			BChar->PistolWeapon = this;
		}
	}


	//��� �ִ� ���Ⱑ ���� ��� ���� Weapon�� �鵵�� �Ѵ�.
	if (BChar->EquippedWeapon == nullptr)
	{
		if (EquipmentType == EEquipmentType::EET_Rifle)
		{
			if (BChar->PrimaryWeapon)
			{
				BChar->ChangeWeapon(1);
			}
			else if (BChar->SubWeapon)
			{
				BChar->ChangeWeapon(2);
			}

		}
		else
		{
			BChar->ChangeWeapon(3);
		}

		//GunAttachToMesh(Main);
	}



	WeaponStat.FireRatePerSec = WeaponStat.FireRatePerMin / 60;
	WeaponStat.SecondPerBullet = 1 / WeaponStat.FireRatePerSec; //0.06

	//broadcast�� ���� AddEquipment �Լ��� ȣ���Ѵ�. �̹� �߰��Ǿ��ֱ� ������, broadcast����� �ϴ°� ����.
	BChar->Equipment->AddEquipment(this);
	
	GunAttachToMesh(BChar);
	return true;
}

void AWeapon::GunAttachToMesh(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	AMainCharacter* Main = Cast<AMainCharacter>(BChar);

	if (BChar)
	{
		/* ��� �־��� ���Ⱑ �ְ�, ����־��� ����� �� ���Ⱑ �ٸ����
		* ��� �־��� ���⸦ 'SubWeapon Attach' ���Ͽ� ������Ų��.
		*/
		if (BChar->EquippedWeapon && (BChar->EquippedWeapon != this))// && ItemState == EItemState::EIS_Spawn)
		{
			const USkeletalMeshSocket* AttachSocket;
			FTransform AttachTransform;

			if(BChar->EquippedWeapon->RifleAssign == ERifleAssign::ERA_Primary)
			{
				AttachSocket = BChar->GetMesh()->GetSocketByName("PrimaryWeaponAttachSocket");
				AttachTransform = PrimaryWeaponAttachTransform;
			}
			else
			{
				AttachSocket = BChar->GetMesh()->GetSocketByName("SubWeaponAttachSocket");
				AttachTransform = SubWeaponAttachTransform;
			}

			if (AttachSocket)
			{
				AttachSocket->AttachActor(BChar->EquippedWeapon, BChar->GetMesh());
				BChar->EquippedWeapon->SetActorRelativeTransform(AttachTransform);
			}
			/*SubSocket->AttachActor(this, Main->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);*/

		}
		
	

		// 1, 3��Ī ����� weapon attach�� �����Ѵ�. (Main���� ����)
		if (Main)
		{
			const USkeletalMeshSocket* TPSocket = Main->GetMesh()->GetSocketByName("WeaponGrip");
			const USkeletalMeshSocket* FPSocket = Main->FPMesh->GetSocketByName("WeaponGrip");

			if (TPSocket && FPSocket)
			{
				switch (Main->CameraMode)
				{
				case ECameraMode::ECM_FPS:
					if (FPSocket->AttachActor(this, Main->FPMesh))
					{
						SetActorRelativeTransform(FPMeshAttachTransform);

						//Main->BaseWeapTransform = SKMesh->GetRelativeTransform();
					}
					break;
				case ECameraMode::ECM_TPS:
					if (TPSocket->AttachActor(this, Main->GetMesh()))
					{
						SetActorRelativeTransform(MeshAttachTransform);
					}
					break;
				}

				SKMesh->SetHiddenInGame(false);
				Main->SetEquippedWeapon(this);
			}
		}

		//BChar�� null�� �ƴѰ�� -> AI���� �ش�
		if (BChar && Main == nullptr)
		{
			const USkeletalMeshSocket* WeaponSocket = BChar->GetMesh()->GetSocketByName("WeaponGrip");
			if (WeaponSocket)
			{
				if (WeaponSocket->AttachActor(this, BChar->GetMesh()))
				{
					SetActorRelativeTransform(MeshAttachTransform);
					//SKMesh->SetHiddenInGame(false);
					BChar->SetEquippedWeapon(this);
				}
			}

		}

	}
}

FTransform AWeapon::GetSightSocketTransform()
{
	//if have any sight device. return that location.
	
	FTransform ReturnTransform;

	ReturnTransform = SKMesh->GetSocketTransform("AimPos", ERelativeTransformSpace::RTS_World);

	return ReturnTransform;
}

/*
void AWeapon::FPS_AimAttachToMesh(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main && Main->EquippedWeapon)
	{
		if (Main->CameraMode == ECameraMode::ECM_FPS)
		{
			const USkeletalMeshSocket* FP_HipSocket = Main->FPMesh->GetSocketByName("WeaponGrip");
			const USkeletalMeshSocket* FP_AimSocket = Main->FPMesh->GetSocketByName("WeaponAimGrip");
			if (FP_HipSocket && FP_AimSocket)
			{
				switch (Main->AimMode)
				{
				case EAimMode::EAM_Aim:
					FP_AimSocket->AttachActor(this, Main->FPMesh);
					break;
				case EAimMode::EAM_NotAim:
					FP_HipSocket->AttachActor(this, Main->FPMesh);
					break;
				}
			}
		}
	}
}
*/

//void AWeapon::Drop()
//{
//	Super::Drop();
//
//	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
//	SKMesh->SetHiddenInGame(true);
//	OwningEquipment = false;
//	
//	
//}

void AWeapon::ChangeSafetyLever()
{
	if (EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_Safe:
			UE_LOG(LogTemp, Warning, TEXT("Semi Auto"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
			break;
		case EWeaponFiringMode::EWFM_SemiAuto:
			UE_LOG(LogTemp, Warning, TEXT("3-round burst"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_Burst;
			break;
		case EWeaponFiringMode::EWFM_Burst:
			UE_LOG(LogTemp, Warning, TEXT("Full Automatic"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_FullAuto;
			break;
		case EWeaponFiringMode::EWFM_FullAuto:
			UE_LOG(LogTemp, Warning, TEXT("Safe"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_Safe;
			break;
		}
	}
}


void AWeapon::TempNewWeaponState()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::TempNewWeaponState"));
	EWeaponState State = EWeaponState::EWS_Idle;

	if (bLMBDown)
	{
		if (CanFire())//�߻縦 �� �� �ִٸ�, Firing���� ���¸� �����Ѵ�.
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("TempState -> Firing"));
#endif
			State = EWeaponState::EWS_Firing;
		}
	}
	
	//�� �ӽ������� State�� Setting�ϱ� ���� SetWeaponState�Լ��� ȣ���Ѵ�.
	SetWeaponState(State);
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::SetWeaponState"));
	//������ �߻����̾���, ���̻� LMB�� ������ �ʴ´ٸ�
	if (CurrentWeaponState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		// ����� ������ ������ ��� �̻� �Ƚ����� ��ݵǵ��� ��.
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		//����� �ߴܵǾ�� �Ҷ� EndFiringȣ�� 
		 //�����϶��� ���д�-> Check Firing�Լ����� Firing�� ȣ���ϵ��� ��.
		if (bCanEndFire)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Call EndFiring func"));
#endif			
			CurrentWeaponState = NewState;
			EndFiring(); 
			
		}
		
	}
	//������ �߻����� �ʾҰ�, LMB�� �����ٸ�
	else if (CurrentWeaponState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{

#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Start Firing Call ControlFiring func"));
#endif			
		//����� �����Ѵ�.
		CurrentWeaponState = NewState;
		ControlFiring();	
	}
}


	/*
	* �� �� �ִ� ����
	* 1. ������ ������ �ƴϾ���Ѵ�.
	* 2. ������, ������� �ƴϾ�� �Ѵ�.
	* 3. ź���� �ѹ��̻� �־�� �Ѵ�.
	*
	* 4. �������� �ƴϾ�� �Ѵ�. //�̱���.
	* 5. ������Ʈ ���� �ƴϾ�� �Ѵ�. //�̱���
	*/
bool AWeapon::CanFire()
{
	bool bCanFire = false;
	
	
	if (ItemObj->bIsDestoryed == false)
	{
		if (WeaponFiringMode != EWeaponFiringMode::EWFM_Safe && CurrentWeaponState != EWeaponState::EWS_Reloading)
		{
			if (!(WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto && FireCount > 0))
			{
				bCanFire = true;
			}
		}
	}
	return bCanFire;
}

bool AWeapon::CanEndFire()
{
	return false;
}


   /* FiringMode�� ���� Delay�� �����.
	* semiauto�� ��� toggle�������.
	* burst�� ��� 3�� ���
	* Fullauto�� ��� ���ӻ��
	*
	* ���⼭ timerüũ�� �ѹ� ���ְ� ���Ŀ� ȣ��Ǵ� �Լ��� �� �ΰ��� ������ ��.
	*/
void AWeapon::StartFire()
{
	
	if (!bLMBDown)
	{
		bLMBDown = true;
		/*
		���� �������� ������ Shot�������� �־�����, idling���¿����� üũ�ؾ���.
		���� ������ �ִ� ���̾����� ����ؼ� ���� �Լ� ȣ��(�̰Ÿ��� �ٸ� �Լ�)
		�ƴϸ�, ������ ���� �Լ�ȣ���ؾ���.(�� �Լ� ���� �ٸ���)
		*/

		TempNewWeaponState();
	}
}

void AWeapon::StopFire()
{
	if (bLMBDown)
	{
		bLMBDown = false;
		TempNewWeaponState();
	}
}

void AWeapon::ControlFiring()
{
	/* SetWeaponState���� ȣ��ȴ�.
	* 
	* Timer�� ���� Firing�Լ��� ȣ���Ѵ�.
	* Timer�� �ð���
	* (������ �߻� �ð��� �߻��Ҷ��� ���� Ÿ��)
	* ������ �߻�ð� + �߻簣�� Ÿ�� = �߻� ���� �ð� �� �ȴ�.
	* �׷�, �� �߻� ���ɽð��� ���� Ÿ�Ӻ��� ũ�� �߻縦 �� �� ����.
	* ���� �߻� �ð���
	* �� �߻簡�ɽð��� ����Ÿ���� ���� ��������.
	* 
	* ���� ���, 1�ʿ� ��� 2���� ������ �����ٸ�
	* 3���� ����Ÿ�ӿ� �� �� �ִ�.
	* 3�ʶ� �� �� �ִµ� ������ ����Ÿ���� 2�ʶ�� �� �� ����.
	* 
	* ���� �߻�ð������� ���
	* 3�ʶ� �� ���ִµ�, ���� ����Ÿ���� 2�ʸ� ����
	* 1�ʰ� ���µ�. 1���Ŀ� 3�ʰ� �Ǿ� �� �� �ִ�.
	* �̸� ������ ��Ÿ���� �ȴ�.	
	*/

#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::ControlFiring"));
#endif			
	
	float WorldTime = GetWorld()->GetTimeSeconds();
	
	// ���� ����϶��� ������ �ð��� �� �߰��Ѵ�. 
	// Enum�� �ᵵ ������ if���� �ʹ� ������� boolean������ �ϳ� �߰��ѰŴ�.
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//������ �߻�ð��� ���� SPB (�߻�� �ʿ��� ��)�� ���� ���� worldTime���� ũ�ٸ�
	// ���� �߻� �� �� �����Ƿ� �߻簡�� �ð��� ���Ѵ�.

	//������� �߻� ���� �ð��� ���� �������� �˻��Ѵ�.
	if((bIsBurstmode && LastFireTime > 0 && LastFireTime + WeaponStat.SecondPerBullet *4 >WorldTime) ||
		(LastFireTime > 0 && LastFireTime + WeaponStat.SecondPerBullet > WorldTime))
	{
		//�߻� ���ɽð��� ���Ѵ�. 
		float RemainingTime = LastFireTime + WeaponStat.SecondPerBullet - WorldTime;

		if (bIsBurstmode) 
		{
			//���� ���� SecondPerBullet�� 4�� ���� ��.
			RemainingTime = (LastFireTime + WeaponStat.SecondPerBullet * 4) - WorldTime;
		}
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::Firing, RemainingTime, false);
		
	}
	else
	{
		//�߻簡 �����ϴٸ� �ٷ� ȣ���Ѵ�.
		Firing();
	}
	
}

void AWeapon::Firing()
{
	/* ControlFiring, ReFiring���� ȣ���. */
	/*
		ammo üũ, FireCount ����, �Լ� ���� Timer��� (������ �߻� �ð� ��Ͽ�)
		����ؼ� ������̶�� ReFiring�Լ� ȣ��
	*/

#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Firing function"));
#endif			
	bIsFiring = true;

	if (bDetectLookInput == false)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(GetInstigator());
		if (Main)
		{
			if (Main->bIsLookInput)
			{
				bDetectLookInput = true;
			}
		}
	}

	/* ù �߻�� ControlRotation���� �����Ѵ�. -> ��� ����� �����ϱ� ���� */
	if (FireCount == 0)
	{
		StartFiringRotation = GetInstigatorController()->GetControlRotation();
		//UE_LOG(LogTemp, Warning, TEXT("Save Start Firing Rotation"));
		//UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}

	WeaponFX();
	//BulletOut(); //Weapon Instant�� ������.
	New_BulletOut(); //Weapon Instant

	FireCount++;
	
	UE_LOG(LogTemp, Warning, TEXT("Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	
	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();
}

//ReFiring�� �� �� �ִ��� Ȯ���ϴ� �Լ���.
void AWeapon::ReFiring()
{
	//������ üũ�ϴ� �Լ��� ȣ���Ѵ�.
	bool bCanReFire = CheckRefire();
	if (bCanReFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReFiring:: Can Refire. call Firing"));
		Firing();
	}
	else
	{
		EndFiring();
	}
	
}

// Firing�� ������ ���� �������� �ʱ�ȭ �����ش�.
void AWeapon::EndFiring()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode�� ����
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;

	// ����� �������� ù ��� �������� �ǵ��� ���� ��� ,,, test�� ���� ��� ����� off.
	//AimInitialize();
}

//Refire�� �������� üũ�Ѵ�.
bool AWeapon::CheckRefire()
{
	bool bFlag = false;
	//�ݹ��� �������� �켱 üũ�ϰ�
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			//Burst����϶��� BurstRound���Ϸ� ������ refire�� �����ϴ�.
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponStat.BurstRound)
				{
					bFlag = true;
				}
				break;
			case EWeaponFiringMode::EWFM_FullAuto:
				bFlag = true;
				break;
			}
		}
	}
	return bFlag;
}

/*
FVector AWeapon::GetAimLocation_TEST()
{
	check(OwningPlayer)

	if (MainCon == nullptr)
	{
		AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);
		MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	}
	//FVector ReturnAim = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	FVector ReturnAim = FVector::ZeroVector;

	if(MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		//ReturnAim = FTransform(AimRot, AimLoc);
		ReturnAim = AimLoc;
	}
	return ReturnAim;
}
*/

// �÷��̾��� �������� (����Ʈ�� �ٶ󺸰� �ִ� ȸ����)�� Vector������ ���Ѵ�.
FTransform AWeapon::GetAimPosition()
{
	check(OwningPlayer)

	FTransform ReturnAim = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	//FVector ReturnAim = FVector::ZeroVector;

	if (MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		ReturnAim = FTransform(AimRot, AimLoc);
		//ReturnAim = AimRot.Vector();
	}

	return ReturnAim;
}

/* �� �Լ����� ���� ���� ��ŸƮ �����̼��� ���� ���� ���༭ ���� ź ���� ��ġ�� ���Ѵ�.*/
FVector AWeapon::GetTraceStartLocation(FVector Dir)
{
	check(OwningPlayer)
	check(MainCon)

	FVector ReturnLocation;
	FRotator Rot;

	//ClippingWall �Լ��� ����Ǹ�, ź ���� ��ġ�� Muzzle�� ��ġ�� �ٲ۴�.
	//�Լ��� ������� ������,  ź���� ��ġ�� Actor��(Actor-CamLo = Actor�� ����ġ�� �ȴ�) Dir�� ���̰��� �ȴ�.
	if(bIsHighReady)
	{
		FVector WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		ReturnLocation = WeaponMuzzleLocation; //GetActorLocation();
	}
	else
	{
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir);
	}
	
	return ReturnLocation;
}

FVector AWeapon::GetTraceEndLocation(FVector StartVector, FVector Dir)
{
	FVector ReturnVec;

	//ClippingWall�Լ��� ����Ǹ� ź�� ���� ��ġ�� Muzzle�� ����*�Ÿ��� �Ǹ�
	//�Լ��� ������� ������, WorldAimPosition�� �ȴ�.
	if(bIsHighReady)
	{
		FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);
		ReturnVec = StartVector + MuzzleRotation.Vector() * WeaponStat.WeaponRange;
	}
	else
	{
		ReturnVec = WorldAimPosition + Dir * 30.f;
	}
	return ReturnVec;
}



FHitResult AWeapon::BulletTrace(FVector StartTrace, FVector EndTrace)
{
	FHitResult Hit;

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator�� IgnoreActor�� �ϸ�ȴ�.

	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);

	return Hit;
}

/* ����� �ѱ� ����Ʈ */
void AWeapon::WeaponFX()
{
	//����
	if (FireSound)
	{

		//PlaySound2D�� ���� Actor�� ��ġ���� �Ҹ��� ���°� �ƴ϶� �����÷��̾� ���� �鸰��.
		//UWorld* world = GetWorld();
		//UGameplayStatics::PlaySound2D(world, FireSound);
		UGameplayStatics::SpawnSoundAttached(FireSound, OwningPlayer->GetRootComponent());
	}

	//�ѱ� ����Ʈ
	if (FireMuzzleEffect)
	{
		const USkeletalMeshSocket* MuzzleSocket = SKMesh->GetSocketByName(MuzzleFlashSocketName);
		
		if (MuzzleSocket)
		{
			FVector MuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireMuzzleEffect, MuzzleLocation, MuzzleRotation);
		}
	}

	//Animatoin ���.
	PlayWeaponAnimAndCamShake(FireAnimaton);
}

void AWeapon::PlayWeaponAnimAndCamShake(FWeaponAnim& Anim)
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer); 
	if (MainChar && CamShake)
	{
		if (MainChar->CameraMode == ECameraMode::ECM_FPS)
		{
			MainChar->PlayAnimMontage(Anim.FPSAnim);
			MainChar->MainController->ClientStartCameraShake(CamShake, 0.7f);
			//MainChar->MainController->ClientPlayCameraShake(CamShake, 0.7f);
		}
		else
		{
			MainChar->PlayAnimMontage(Anim.TPSAnim);
			MainChar->MainController->ClientStartCameraShake(CamShake, 1.0f);
			//MainChar->MainController->ClientPlayCameraShake(CamShake, 1.0f);
		}
	}
}

void AWeapon::AimInitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::AimInitialize"));
	//UE_LOG(LogTemp, Warning, TEXT("Save End Firing Location And Init"));
	EndFiringRotation = GetInstigatorController()->GetControlRotation();

	//fire�� ���콺 �Է��� �����Ǹ� AimInit�� �ܼ��� �Ʒ��� ������ �Ѵ�.
	if (bDetectLookInput)
	{
		bDetectLookInput = false;
		FRotator NewStartRot = FRotator(EndFiringRotation.Pitch - 3.f, EndFiringRotation.Yaw, EndFiringRotation.Roll);
		StartFiringRotation = NewStartRot;
	}
	//UE_LOG(LogTemp, Warning, TEXT("End Rotating val : %s"), *EndFiringRotation.ToString());
	Time = 0.f;
	AlphaTime = 0.f;
	GetWorldTimerManager().SetTimer(AimInitHandle, [=] {
		
		Time += GetWorld()->GetDeltaSeconds();
		AlphaTime = Time / 0.8f; // : Time/�ǵ��ƿ��� �ð�  (����)

		//FRotator LerpAimRotation = FMath::RInterpTo(EndFiringRotation, StartFiringRotation, GetWorld()->GetDeltaSeconds(), 20.f);
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);

		//UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());
		},GetWorld()->GetDeltaSeconds(),true);
}

void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();	
	if (OwningPlayer)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(OwningPlayer);
		if (BChar)
		{
			if (BChar->EquippedWeapon == this)
			{
				BChar->ChangeWeapon(0);
				BChar->EquippedWeapon = nullptr;
				//Main->SetEquippedWeapon(nullptr);
				RifleAssign = ERifleAssign::ERA_MAX;
			}

			if (BChar->PrimaryWeapon == this)
			{
				BChar->PrimaryWeapon = nullptr;
			}
			else if(BChar->SubWeapon == this)
			{
				BChar->SubWeapon = nullptr;
			}
			else if(BChar->PistolWeapon == this)
			{
				BChar->PistolWeapon = nullptr;
			}
		}
	}
}

void AWeapon::UpdateAim()
{
	FTransform AimPos = GetAimPosition();
	FVector EndVec = AimPos.GetLocation() + AimPos.GetRotation().Vector() * 3000.f;
	FHitResult Hit = BulletTrace(AimPos.GetLocation(), EndVec);

	/*if (bIsHighReady)
	{
		WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		WorldAimPosition = WeaponMuzzleLocation + AimPos.GetRotation().GetForwardVector() * WeaponStat.WeaponRange;
	}*/
	//else
	{
		if (Hit.bBlockingHit)
		{
			//WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			WorldAimPosition = Hit.Location;


			//FPS�������� Trace�� �������� �ѹ� �� ���� ������ ���� ��ġ�� AimPos�� �ű��.
			FHitResult WeaponHit = BulletTrace(MainCon->Main->CameraFPS->GetComponentLocation(), Hit.Location);
			//FHitResult WeaponHit = BulletTrace(WeaponMuzzleLocation, Hit.Location);
			if (WeaponHit.bBlockingHit)
			{
				WorldAimPosition = WeaponHit.Location;
			}
		}
		else
		{
			//DrawDebugPoint(GetWorld(), WorldAimPosition, 20.f, FColor::Blue, false, 0.1f);
			WorldAimPosition = EndVec;
		}
	}

	//DrawDebugPoint(GetWorld(), WorldAimPosition, 10.f, FColor::Green, false, -1.f);

}

void AWeapon::WeaponClipping()
{
	FTransform CurrentMeshAttachTransform;
	FVector CurrentMeshRightHand;
	if (ECameraMode::ECM_FPS == MainCon->Main->CameraMode)
	{
		CurrentMeshAttachTransform = FPMeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->FPMesh->GetSocketLocation("Hand_R");

	}
	else 
	{
		CurrentMeshAttachTransform = MeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->GetMesh()->GetSocketLocation("hand_r");
	}

	//LienTrace�� ���̴� Weapon�� ������ CapsuleComp����
	float Length = CapsuleComp->GetScaledCapsuleHalfHeight() * 4;

	//���⺤�ʹ� AimPosition���� �Ѵ�.
	FVector EndLo = CurrentMeshRightHand + GetAimPosition().GetRotation().GetForwardVector() * Length;
	TArray<AActor*> IgnoreActorList;
	IgnoreActorList.Add(GetInstigator());
	FHitResult Hit;

	ETraceTypeQuery TTQ = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CurrentMeshRightHand, EndLo, 5.f, TTQ, false,
		IgnoreActorList, EDrawDebugTrace::None, Hit, true);
	
	
	if(Hit.bBlockingHit)
	{
		FVector CurVec = CurrentMeshAttachTransform.GetTranslation();
		//FVector CalcVec = CurrentMeshAttachTransform.GetTranslation() - (NewY * 50.f);
		
		FRotator CurRot = CurrentMeshAttachTransform.GetRotation().Rotator();

		//Why Roll?? -> Skeleton���� ���⸦ �ְ� ���� ������ Roll���� �ٲ��� Roll�� �Ѱ���.
		// Hit.Distance / Length�ϰԵǸ� 0���� 1�� �����µ�.
		// Hit.Distance == Length -> 1�̵�. ��, 1�϶��� 0�̵ǰ� 0�϶��� 90���� �Ǿ����.
		// ������ 90�� ���߰�, 90�� �� ���ع����� 1�϶� 90�� �ǹ���. �׸��� ������ ������ -90�� �Ǿ� ���� �ö󰡴�
		// 0*90 = 0, 0.5*90 = 45, 1*90 = 90�̶� �� -90�� ��������,
		// �������� 0�϶� -90, 0.5�϶� -45, 1�϶� 0�� ����.
		FRotator CalcRot = FRotator(CurRot.Pitch, CurRot.Yaw, (CurRot.Roll + (Hit.Distance / Length)*90.f) - 90.f);
		//UE_LOG(LogTemp, Warning, TEXT("Calc Rot ; %s"), *CalcRot.ToString());

		
		//FTransform NewTransform = FTransform(CalcRot, CurVec);
		CurrentMeshAttachTransform = FTransform(CalcRot, CurVec);
		
		SetActorRelativeTransform(CurrentMeshAttachTransform);
		bIsHighReady = true;
	}
	else
	{
		SetActorRelativeTransform(CurrentMeshAttachTransform);
		bIsHighReady = false;
	}
}


/* Fix Clipping wall */
/*
void AWeapon::OnCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT(" Weapon Overlap begin"));

	if(OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		
		if(Main->TPAnimInstance->bBeginHighReady == false)
		{
			//Weaponclass�� �ִ� bIsHighReady�� AnimInstance���� ������ ���������� false�� �����ֵ��� ����.
			bIsHighReady = true;
			//Main->TPAnimInstance->bBeginHighReady = true;
			//Main->FPAnimInstance->bBeginHighReady = true;

			// New Fix Clipping wall
			//Tick���� Sphere Line Trace�� ��µ�, distance�� radius�� ���� ������.(�Ѹ��� �ٸ��� ������..)
			//hit�� �ִٸ�, Original Transform���� Location��  - (0.f,0.f, Curve.evaluate(hit.distance / distance)
			//�̷��� �ؼ� ���°��� NewTransform.location���� �������ָ� �ȴ�.
			


			FVector WeaponLocation = GetActorLocation();
			FVector OwnerLocation = Main->GetActorLocation();

		}
		
		//Main->TPAnimInstance->bEndHighReady = false;
		//Main->FPAnimInstance->bEndHighReady = false;

	}

}

void AWeapon::OnCollisionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT(" Weapon Overlap End"));

	if(OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		bIsHighReady = false;
		//Main->TPAnimInstance->bBeginHighReady = false;
		//Main->FPAnimInstance->bBeginHighReady = false;
		
		Main->TPAnimInstance->bEndHighReady = true;
		Main->FPAnimInstance->bEndHighReady = true;
	}
}
*/