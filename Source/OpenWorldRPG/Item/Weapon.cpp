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
#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#define DEBUG 0

AWeapon::AWeapon() : Super()
{
	//SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	//RootComponent = SKMesh;
	//Mesh->SetupAttachment(GetRootComponent());
	//SKMesh->SetHiddenInGame(true);

	bIsFiring = false;
	bLMBDown = false;
	bDetectLookInput = false;

	//MuzzleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffectComp"));
	//MuzzleEffectComp->SetupAttachment(GetRootComponent());
	MuzzleFlashSocketName = FName("muzzleflash");

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;

	RifleAssign = ERifleAssign::ERA_MAX;
}

//void AWeapon::SetOwningPlayer(AActor* Actor)
//{
//	if (OwningPlayer != Actor)
//	{
//		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
//		if (Main)
//		{
//			OwningPlayer = Main;
//			SetInstigator(Main); //Instigator ����.
//		}
//	}
//}

void AWeapon::Equip(AActor* Char)
{
	Super::Equip(Char);
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	check(Main);

	if (RifleAssign == ERifleAssign::ERA_MAX)
	{
		/* 1,2,3�� �������� Quick Swap�ϱ� ���� */
		if (EquipmentType == EEquipmentType::EET_Rifle) //�������̰�
		{
			if (Main->PrimaryWeapon) //�̹� �ֹ��Ⱑ ������
			{
				RifleAssign = ERifleAssign::ERA_Sub;
				Main->SubWeapon = this;// (AWeapon*)this; //�ι���� ����
			}
			else //�ֹ��Ⱑ ������
			{
				RifleAssign = ERifleAssign::ERA_Primary;
				Main->PrimaryWeapon = this;// (AWeapon*)this; //�ֹ����
			}
		}
		else //�ǽ���
		{
			Main->PistolWeapon = this; // (AWeapon*)this;
		}
	}


	//��� �ִ� ���Ⱑ ���� ��� ���� Weapon�� �鵵�� �Ѵ�.
	if (Main->EquippedWeapon == nullptr)
	{
		if (EquipmentType == EEquipmentType::EET_Rifle)
		{
			if (Main->PrimaryWeapon)
			{
				Main->ChangeWeapon(1);
			}
			else if (Main->SubWeapon)
			{
				Main->ChangeWeapon(2);
			}

		}
		else
		{
			Main->ChangeWeapon(3);
		}

		//GunAttachToMesh(Main);
	}



	WeaponStat.FireRatePerSec = WeaponStat.FireRatePerMin / 60;
	WeaponStat.SecondPerBullet = 1 / WeaponStat.FireRatePerSec; //0.06

	//broadcast�� ���� AddEquipment �Լ��� ȣ���Ѵ�. �̹� �߰��Ǿ��ֱ� ������, broadcast����� �ϴ°� ����.
	Main->Equipment->AddEquipment(this);
	
	GunAttachToMesh(Char);
	
}

void AWeapon::GunAttachToMesh(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		/* �̹� ��� �ִ� ���Ⱑ �ְ�, ����ִ� ����� �� ���Ⱑ �ٸ����
		* �̹� ��� �ִ� ���⸦ 'SubWeapon Attach' ���Ͽ� ������Ų��.
		*/
		if (Main->EquippedWeapon && (Main->EquippedWeapon != this))// && ItemState == EItemState::EIS_Spawn)
		{
			const USkeletalMeshSocket* SubSocket = Main->GetMesh()->GetSocketByName("SubWeaponAttachSocket");

			SubSocket->AttachActor(Main->EquippedWeapon, Main->GetMesh());
			Main->EquippedWeapon->SetActorRelativeTransform(MeshAttachTransform);

			/*SubSocket->AttachActor(this, Main->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);*/

		}
		
	
		const USkeletalMeshSocket* TPSocket = Main->GetMesh()->GetSocketByName("WeaponGrip");
		const USkeletalMeshSocket* FPSocket = Main->FPMesh->GetSocketByName("WeaponGrip");

		if (TPSocket && FPSocket)
		{
			switch (Main->CameraMode)
			{
			case ECameraMode::ECM_FPS:

				//AttachToActor(Main,)
				if (FPSocket->AttachActor(this, Main->FPMesh))
				{
					Main->BaseWeapTransform = SKMesh->GetRelativeTransform();
				}
				break;
			case ECameraMode::ECM_TPS:
				if (TPSocket->AttachActor(this, Main->GetMesh()))
				{

				}
				break;
			}				

			//SKMesh->SetVisibility(true);
			SKMesh->SetHiddenInGame(false);
			Main->EquippedWeapon = this;
		}		

	}
}


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
		/* ����� ������ ������ ��� �̻� �Ƚ����� ��ݵǵ��� ��. */
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		/* ����� �ߴܵǾ�� �Ҷ� EndFiringȣ�� 
		 �����϶��� ���д�, Check Firing�Լ����� Firing�� ȣ���ϵ��� ��.*/
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

bool AWeapon::CanFire()
{
	bool bCanFire = false;
	
	/*
	* �� �� �ִ� ����
	* ������ ������ �ƴϾ���Ѵ�.
	* ������, ������� �ƴϾ�� �Ѵ�. 
	* ź���� �ѹ��̻� �־�� �Ѵ�.
	* 
	* �������� �ƴϾ�� �Ѵ�. //�̱���.
	* ������Ʈ ���� �ƴϾ�� �Ѵ�. //�̱���
	*/
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

void AWeapon::StartFire()
{
	/* ���⼭ FiringMode�� ���� Delay�� �����.
	*	semiauto�� ��� toggle�������.
	*	burst�� ��� 3�� ���
	*  Fullauto�� ��� ���ӻ��
	* 
	* ���⼭ timerüũ�� �ѹ� ���ְ� ���Ŀ� ȣ��Ǵ� �Լ��� �� �ΰ��� ������ ��.
	*/
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
	
	/* ���� ����϶��� ������ �ð��� �� �߰��Ѵ�. */
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//������� ��߻� ���� �ð��� �����ش�.
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
		//UE_LOG(LogTemp, Warning, TEXT("No time, Call firing directly"));
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
		UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}

	WeaponFX();
	//BulletOut(); //Weapon Instant�� ������.
	New_BulletOut(); //Weapon Instant

	FireCount++;
	
	UE_LOG(LogTemp, Warning, TEXT("Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	
	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();

	/*
	bool bCanReFire = CheckRefire();
	if (bCanReFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firing:: Can Refire. Set Timer"));
		//GetWorldTimerManager().SetTimer(FiringTimer,this, &AWeapon::Firing, WeaponStat.SecondPerBullet, false);
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	}
	else
	{
		//Out of Ammo
		// Check Ammo�� ���� �ѹ� �� �Ѵ�. 
		// Burst mode�϶��� ź�� ����ص�, �ݹ� Ƚ���� ���� ������ ���Ҷ��� �����, 
		// Out of Ammo�Լ��� ������ ȣ���ϴ°� ��ġ ����.
		// 
		// Burst mode�϶��� ź�� üũ�� �ѹ� �� �ؾ���.
		//
		//AimInitialize();
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)//EWeaponFiringMode::EWFM_Burst)
		{
			UE_LOG(LogTemp, Warning, TEXT("Firing:: Burst mode, Can NOT Refire, Call EndFiring"));
			EndFiring();
			//TempNewWeaponState(); //EndFiring�� ȣ������ �ʰ� �̷��� �غ�
			//CurrentWeaponState = EWeaponState::EWS_Idle;
		}
		else if (WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto)
		{
			UE_LOG(LogTemp, Warning, TEXT("Firing:: Semi auto, Can NOT Refire, Call EndFiring"));
			TempNewWeaponState();
		}
		
	}	
	*/	

}

void AWeapon::ReFiring()
{
	//���⼭�� �ݹ� Ƚ�� (FireCount)�� ���� Firingȣ������ ���� ��������.
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

void AWeapon::EndFiring()
{
	/* Firing�� ������ ���� �������� �ʱ�ȭ �����ش�. */
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	//GetWorldTimerManager().ClearTimer(RecoilHandle);
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode�� ����
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;
	/* ����� �������� ù ��� �������� �ǵ��� ���� ���*/
	AimInitialize();
}

bool AWeapon::CheckRefire()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::CheckRefire"));
	bool bFlag = false;
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Safe:
			case EWeaponFiringMode::EWFM_SemiAuto:
				//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can't Refire"));
				break;
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponStat.BurstRound)
				{
					//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can Refire"));
					bFlag = true;
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can't Refire"));
				}
				break;
			case EWeaponFiringMode::EWFM_FullAuto:
				//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can Refire"));
				bFlag = true;
				break;
			}
		}
	}
	return bFlag;
}

FVector AWeapon::GetAimRotation()
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);

	AMainController* MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	FVector ReturnAim = FVector::ZeroVector;
	
	if (MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		ReturnAim = AimRot.Vector();
	}

	return ReturnAim;
}

FVector AWeapon::GetTraceStartLocation(FVector& Dir)
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);
	
	AMainController* MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	FVector ReturnLocation = FVector::ZeroVector;

	if (MainCon)
	{
		FRotator Rot;
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);

		/*
		* FVector CalcDistance = OwningPlayer->GetActorLocation() - ReturnLocation;
		float DotP = FVector::DotProduct(CalcDistance, Dir);
		UE_LOG(LogTemp, Warning, TEXT("======================================="));
		UE_LOG(LogTemp, Warning, TEXT("ViewPoint Loc : %s , ViewPoint Rot : %s"), *ReturnLocation.ToString(), *Dir.ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation : %s"), *OwningPlayer->GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation - ViewPoint Loc  = %s"), *CalcDistance.ToString());
		UE_LOG(LogTemp, Warning, TEXT("size : %f, OR : %f, Dot : %f"), CalcDistance.Size(), (CalcDistance | Dir), DotP);
		UE_LOG(LogTemp, Warning, TEXT("---------------------------------------"));
		//UE_LOG(LogTemp, Warning, TEXT("Result not - ViewPoint Loc= %s"), *(ReturnLocation + Dir * OwningPlayer->GetActorLocation()).ToString());
		UE_LOG(LogTemp, Warning, TEXT(" Red  Result = %s"), *(ReturnLocation + Dir * CalcDistance).ToString());
		
		FVector tempLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation);
		FVector NewTempLocation = ReturnLocation + Dir * 700;
		FVector SizeTempLocation = ReturnLocation + Dir * CalcDistance.Size();
		
		FVector dotLocation = ReturnLocation + Dir * DotP;
		*/
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir );
		
		/*
		DrawDebugSphere(GetWorld(), ReturnLocation, 12.f, 6, FColor::Green, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), tempLocation, 12.f, 6, FColor::Red, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), SizeTempLocation, 12.f, 6, FColor::Purple, true, 2.f, (uint8)nullptr, 2.f);
		//DrawDebugSphere(GetWorld(), dotLocation, 12.f, 6, FColor::Blue, true, 2.f, (uint8)nullptr, 2.f);

		UE_LOG(LogTemp, Warning, TEXT("Purple Result= %s"), *SizeTempLocation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Blue Result= %s"), *dotLocation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Green Result = %s"), *ReturnLocation.ToString());
		*/
	}
	
	return ReturnLocation;
}


FHitResult AWeapon::BulletTrace(FVector& StartTrace, FVector& EndTrace)
{
	FHitResult Hit;

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator�� IgnoreActor�� �ϸ�ȴ�.

	
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);


	/* debug */
	/*if (GetInstigator())
	{
		FString str = GetInstigator()->GetFName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Isnt : GetInstigator: %s"), *str);
	}
	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);
	*/	

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
			SKMesh->GetSocketLocation(MuzzleFlashSocketName);
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
	if (bDetectLookInput)
	{
		bDetectLookInput = false;
		FRotator NewStartRot = FRotator(EndFiringRotation.Pitch - 5.f, EndFiringRotation.Yaw, EndFiringRotation.Roll);
		StartFiringRotation = NewStartRot;
	}
	UE_LOG(LogTemp, Warning, TEXT("End Rotating val : %s"), *EndFiringRotation.ToString());
	Time = 0.f;
	AlphaTime = 0.f;
	GetWorldTimerManager().SetTimer(AimInitHandle, [=] {
		//UE_LOG(LogTemp, Warning, TEXT("AimInit AlphaTime : %f"), AlphaTime);
		Time += GetWorld()->GetDeltaSeconds();
		AlphaTime = Time / 0.6f; // Time/�ǵ��ƿ��� �ð� (����)
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);
		/*UE_LOG(LogTemp, Warning, TEXT("Time : %f"), Time);
		UE_LOG(LogTemp, Warning, TEXT("AlphaTime : %f"), AlphaTime);
		UE_LOG(LogTemp, Warning, TEXT("StartFiringRotation : %s"), *StartFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("EndFiringRotation : %s"), *EndFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());*/
		},GetWorld()->GetDeltaSeconds(),true);
}

void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();	
	if (OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		if (Main)
		{
			if (Main->EquippedWeapon == this)
			{
				Main->ChangeWeapon(0);
				Main->EquippedWeapon = nullptr;
				RifleAssign = ERifleAssign::ERA_MAX;
			}

			if (Main->PrimaryWeapon == this)
			{
				Main->PrimaryWeapon = nullptr;
			}
			else if(Main->SubWeapon == this)
			{
				Main->SubWeapon = nullptr;
			}
			else if(Main->PistolWeapon == this)
			{
				Main->PistolWeapon = nullptr;
			}
		}
	}
}
