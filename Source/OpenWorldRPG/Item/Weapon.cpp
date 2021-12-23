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


AWeapon::AWeapon() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	SKMesh->SetHiddenInGame(true);

	bIsFiring = false;
	bLMBDown = false;

	//MuzzleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffectComp"));
	//MuzzleEffectComp->SetupAttachment(GetRootComponent());
	MuzzleFlashSocketName = FName("muzzleflash");

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;
}

void AWeapon::SetOwningPlayer(AActor* Actor)
{
	if (OwningPlayer != Actor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
		if (Main)
		{
			OwningPlayer = Main;
			SetInstigator(Main); //Instigator ����.
		}
	}
}

void AWeapon::Equip(AActor* Char)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	bool bFlag = false;
	if (Main)
	{
		if (CheckSendToInventory(Main)) //�κ��丮�� ��������
		{
			bFlag = true;
		}

		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
		case EWeaponType::EWT_Pistol:
			if (!bFlag) //Inventory�� ������ �ʾ����� ���� ��Ų��.
			{				
				/* 1,2,3�� �������� Quick Swap�ϱ� ���� */
				if (WeaponType == EWeaponType::EWT_Rifle) //�������̰�
				{
					if (Main->PrimaryWeapon) //�̹� �ֹ��Ⱑ ������
					{
						Main->SubWeapon = this; //�ι���� ����
					}
					else //�ֹ��Ⱑ ������
					{
						Main->PrimaryWeapon = this; //�ֹ����
					}
				}
				else //�ǽ���
				{
					Main->PistolWeapon = this;
				}

				//�ƹ� ���Ⱑ ���� ��� ���� Weapon�� ������Ų��.
				if (Main->EquippedWeapon == nullptr)
				{
					if (WeaponType == EWeaponType::EWT_Rifle)
					{
						Main->ChangeWeapon(1);
					}
					else
					{
						Main->ChangeWeapon(3);
					}

					GunAttachToMesh(Main);
				}
				/*
				//�Ʒ� �������� �ϴ°� ���� ���� �������� �ϴ°� �� ������.
				if (GetItemState() == EItemState::EIS_Spawn) //���� �ִ� Item�̸�,
				{
						����Ÿ�� ���Ⱑ �̹� ���� -> �����ִ� Item�� Inventory��
						���� Ÿ�� ���Ⱑ ���� -> �����ִ� Item�� ����.

						���⼭ Main�� EquipCompo��Ʈ���� �˻��ؾ���. ����� true or false����.

				}
				else //�̹� Inventory�� �ִ� Item�̸�
				{
					 ���� Ÿ�� ���Ⱑ �̹� ���� -> ���� ���⸦ Inventory��, ��Item�� ����
					 ���� Ÿ�� ���Ⱑ ���� -> ���� �ִ� Item�� ����


				}
				//ī�޶� ��忡 ���� 1��Ī, 3��Ī Mesh�� ������Ų��.
				GunAttachToMesh(Main); //�Ʒ��ڵ带 �Լ��� ��ü.
				*/
				/*if (Main->CameraMode == ECameraMode::ECM_FPS)
				{
					FPSocket->AttachActor(this, Main->FPMesh);
				}
				else
				{
					TPSocket->AttachActor(this, Main->GetMesh());
				}*/
			}
			break;
		case EWeaponType::EWT_Helmet:
			if(!bFlag)
			{
				//����
				SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
			}
			break;
		case EWeaponType::EWT_Vest:
			if (!bFlag)
			{
				//����
				SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
			}
			break;
		}		

		
		if (!bFlag) //Inventory�� ������ �ʾ�����
		{
			UE_LOG(LogTemp, Warning, TEXT("AWeapon::Equip Success"));

			//Main�� �ִ� Equipment�� Add���ش�.
			Main->Equipment->AddEquipment(this);
			SetOwningPlayer(Main);

			Mesh->SetSimulatePhysics(false);
			Mesh->SetEnableGravity(false);

			Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


			/*if (EquippedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
			}*/
		}
	}
}

bool AWeapon::CheckSendToInventory(AMainCharacter* Main)
{
	check(Main);

	//�� ����� ���� Ÿ���� ���Ⱑ �̹� �����Ǿ��ְ� (Rifle�� ��� 2�� ��������)
	if (Main->Equipment->IsWeaponExist(this))
	{
		if (GetItemState() == EItemState::EIS_Spawn || GetItemState() == EItemState::EIS_Drop) //���忡 �ִ� �����
		{
			//OwningEquipment�� null�� �������ش�.
			OwningEquipment = nullptr;

			//Inventory�� �̵��ؾ���.
			Pickup(Main);
			return true;
		}
		else //Pickup���¸� (Inventory�� �ִ� ������) -> ��� ���� ���� �Լ��� �����ؾ��ҵ��ϴ�.
		{
			AWeapon* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
			if (Beforeweapon != nullptr)
			{
				Beforeweapon->OwningEquipment = nullptr;
				Beforeweapon->Pickup(Main); //���� ���⸦ Inventory�� ������. 
			}
			UE_LOG(LogTemp, Warning, TEXT("Weapon::CheckSendToInventory. something wrong."));
			return false;
		}
	}
	return false;
}

void AWeapon::GunAttachToMesh(AMainCharacter* Main)
{
	if (Main)
	{
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
					/*if (Main->bIsAim)
					{
						FPS_AimAttachToMesh(Main);
					}*/
					/*if (WeaponType == EWeaponType::EWT_Rifle)
					{
						SetActorRelativeLocation(Main->RifleRelativeLoRo.GetLocation());
						SetActorRelativeRotation(Main->RifleRelativeLoRo.GetRotation());
					}
					else
					{
						SetActorRelativeLocation(Main->PistolRelativeLoRo.GetLocation());
						SetActorRelativeRotation(Main->PistolRelativeLoRo.GetRotation());
					}*/
				}
				
				//setactorrelativ
				break;
			case ECameraMode::ECM_TPS:
				if (TPSocket->AttachActor(this, Main->GetMesh()))
				{
					/*if (WeaponType == EWeaponType::EWT_Rifle)
					{
						SetActorRelativeLocation(Main->RifleRelativeLoRo.GetLocation());
						SetActorRelativeRotation(Main->RifleRelativeLoRo.GetRotation());
					}
					else
					{
						SetActorRelativeLocation(Main->PistolRelativeLoRo.GetLocation());
						SetActorRelativeRotation(Main->PistolRelativeLoRo.GetRotation());
					}*/
				}
				break;
			}

			if (Main->EquippedWeapon)//�̹� ����ִ� ���Ⱑ �������
			{
				//�ش� ������ SkMesh�� �Ⱥ��̰� �Ѵ�.
				Main->EquippedWeapon->SKMesh->SetHiddenInGame(true);
			}

			SKMesh->SetHiddenInGame(false);
			Main->EquippedWeapon = this;
		}

	}
}


void AWeapon::FPS_AimAttachToMesh(AMainCharacter* Main)
{
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

void AWeapon::Drop()
{
	Super::Drop();

	/* Weapon Rifle�� ����ɶ�
	*  ��� �� ������ ������... ����� �غ��� collision�� ����� ���õǴµ���
	*  ������. - �����ľ� �Ұ�.
	*/
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;
	
	
}

void AWeapon::ChangeSafetyLever()
{
	if (WeaponType == EWeaponType::EWT_Rifle)
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

	//�� if�� ���ǿ� ���� CanFire�Լ��� �߰� ����.
	if (bLMBDown)
	{
		if (CanFire())//�߻縦 �� �� �ִٸ�, Firing���� ���¸� �����Ѵ�.
		{
			UE_LOG(LogTemp, Warning, TEXT("TempState -> Firing"));
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
			bCanEndFire = false;
		}


		/* ����� �ߴܵǾ�� �Ҷ� EndFiringȣ�� 
		 �����϶��� ���д�, Check Firing�Լ����� Firing�� ȣ���ϵ��� ��.*/
		if (bCanEndFire)
		{
			CurrentWeaponState = NewState;
			EndFiring();
		}
		
	}
	//������ �߻����� �ʾҰ�, LMB�� �����ٸ�
	else if (CurrentWeaponState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{
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
	if (WeaponFiringMode != EWeaponFiringMode::EWFM_Safe && CurrentWeaponState != EWeaponState::EWS_Reloading)
	{
		//if (CurrentWeaponState != EWeaponState::EWS_Firing)
		{
			bCanFire = true;
		}
	}
	return bCanFire;
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
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::ControlFiring"));
	float WorldTime = GetWorld()->GetTimeSeconds();
	
	/* ���� ����϶��� ������ �ð��� �� �߰��Ѵ�. */
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//������� �ٽ� �߻� ���ɽð��� �����ش�.
	if((bIsBurstmode && LastFireTime > 0 && LastFireTime + WeaponStat.FireRatePerSec *4 >WorldTime) ||
		(LastFireTime > 0 && LastFireTime + WeaponStat.FireRatePerSec > WorldTime))
	{
		//�߻� ���ɽð��� ���Ѵ�. 
		float RemainingTime = LastFireTime + WeaponStat.FireRatePerSec - WorldTime;

		if (bIsBurstmode) 
		{
			//���� ���� FireRatePerSec�� 4�� ���� ��.
			RemainingTime = (LastFireTime + WeaponStat.FireRatePerSec * 4) - WorldTime;
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
	bIsFiring = true;
	WeaponFX();
	BulletOut(); //Weapon Instant�� ������.
	
	/* ù �߻�� ControlRotation���� �����Ѵ�. -> ��� ����� �����ϱ� ���� */
	if (FireCount == 0)
	{
		StartFiringRotation = GetInstigatorController()->GetControlRotation();
	}

	FireCount++;
	
	UE_LOG(LogTemp, Warning, TEXT("Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	

	bool bCanReFire = CheckRefire();
	if (bCanReFire)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Firing:: Can Refire"));
		GetWorldTimerManager().SetTimer(FiringTimer,this, &AWeapon::Firing, WeaponStat.FireRatePerSec, false);
	}
	else
	{
		//Out of Ammo
		/* Check Ammo�� ���� �ѹ� �� �Ѵ�. 
		* Burst mode�϶��� ź�� ����ص�, �ݹ� Ƚ���� ���� ������ ���Ҷ��� �����, 
		* Out of Ammo�Լ��� ������ ȣ���ϴ°� ��ġ ����.
		* 
		* Burst mode�϶��� ź�� üũ�� �ѹ� �� �ؾ���.
		*/
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Firing:: Can NOT Refire, Call EndFiring"));
			EndFiring();
			//CurrentWeaponState = EWeaponState::EWS_Idle;
		}
		
	}	

	LastFireTime = GetWorld()->GetTimeSeconds();
	

}

void AWeapon::ReFiring()
{
	//���⼭�� �ݹ� Ƚ�� (FireCount)�� ���� Firingȣ������ ���� ��������.
	Firing();
}

void AWeapon::EndFiring()
{
	/* Firing�� ������ ���� �������� �ʱ�ȭ �����ش�. */
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode�� ����
	PreviousSpread = FVector::ZeroVector;
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
				//UE_LOG(LogTemp, Warning, TEXT("Check confirm, Can NOT Refire"));
				break;
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponStat.BurstRound)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Check confirm, Can Refire"));
					bFlag = true;
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("Check confirm, Can NOT Refire"));
				}
				break;
			case EWeaponFiringMode::EWFM_FullAuto:
				//UE_LOG(LogTemp, Warning, TEXT("Check confirm, Can Refire"));
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
	EndFiringRotation = GetInstigatorController()->GetControlRotation();
	Time = 0.f;
	AlphaTime = 0.f;
	GetWorldTimerManager().SetTimer(AimInitHandle, [=] {
		Time += GetWorld()->GetDeltaSeconds();
		AlphaTime = Time / 1.f; // Time/�ǵ��ƿ��� �ð� (����)
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);
		/*UE_LOG(LogTemp, Warning, TEXT("Time : %f"), Time);
		UE_LOG(LogTemp, Warning, TEXT("AlphaTime : %f"), AlphaTime);
		UE_LOG(LogTemp, Warning, TEXT("StartFiringRotation : %s"), *StartFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("EndFiringRotation : %s"), *EndFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());*/
		},GetWorld()->GetDeltaSeconds(),true);
}