// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "EquipmentComponent.h"
#include "DrawDebugHelpers.h" //������
#include "Camera/CameraComponent.h"

AWeapon::AWeapon() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	SKMesh->SetHiddenInGame(true);

	bIsFiring = false;
	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
}

void AWeapon::SetOwningPlayer(AMainCharacter* Main)
{
	if (OwningPlayer != Main)
	{
		SetInstigator(Main); //Instigator ����.
		OwningPlayer = Main;
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


				/*if (EquippedSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
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

void AWeapon::SetWeaponFiringMode()
{
	if (WeaponType == EWeaponType::EWT_Rifle)
	{
		switch (WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_Safe:
			WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
		case EWeaponFiringMode::EWFM_SemiAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Burst;
		case EWeaponFiringMode::EWFM_Burst:
			WeaponFiringMode = EWeaponFiringMode::EWFM_FullAuto;
		case EWeaponFiringMode::EWFM_FullAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Safe;
		}
	}
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
	if (!bIsFiring)
	{
		bIsFiring = true;
		/*
		���� �������� ������ Shot�������� �־�����, idling���¿����� üũ�ؾ���.
		���� ������ �ִ� ���̾����� ����ؼ� ���� �Լ� ȣ��(�̰Ÿ��� �ٸ� �Լ�)
		�ƴϸ�, ������ ���� �Լ�ȣ���ؾ���.(�� �Լ� ���� �ٸ���)
		*/

		Firing();
	}
}

void AWeapon::StopFire()
{
	if (bIsFiring)
	{
		bIsFiring = false;
	}
}


void AWeapon::Firing()
{
	/* Start Fire, ReFiring���� ȣ���. */
	/*
		ammo üũ, FireCount ����, �Լ� ���� Timer��� (������ �߻� �ð� ��Ͽ�)
		����ؼ� ������̶�� ReFiring�Լ� ȣ��
	*/	
	BulletOut(); //Weapon Instant�� ������.
	
}

void AWeapon::ReFiring()
{
	//���⼭�� �ݹ� Ƚ�� (FireCount)�� ���� Firingȣ������ ���� ��������.
	Firing();
}

FVector AWeapon::GetAimRotation()
{
	AMainController* MainCon = OwningPlayer ? Cast<AMainController>(OwningPlayer->MainController) : nullptr;
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
	AMainController* MainCon = OwningPlayer ? Cast<AMainController>(OwningPlayer->MainController) : nullptr;
	FVector ReturnLocation = FVector::ZeroVector;

	if (MainCon)
	{
		FRotator Rot;
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);

		FVector CalcDistance = OwningPlayer->GetActorLocation() - ReturnLocation;
		
		UE_LOG(LogTemp, Warning, TEXT("======================================="));
		UE_LOG(LogTemp, Warning, TEXT("ViewPoint Loc : %s , ViewPoint Rot : %s"), *ReturnLocation.ToString(), *Dir.ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation : %s"), *OwningPlayer->GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation - ViewPoint Loc  = %s"), *CalcDistance.ToString());
		UE_LOG(LogTemp, Warning, TEXT("---------------------------------------"));
		//UE_LOG(LogTemp, Warning, TEXT("Result not - ViewPoint Loc= %s"), *(ReturnLocation + Dir * OwningPlayer->GetActorLocation()).ToString());
		UE_LOG(LogTemp, Warning, TEXT("Result NOT | operation = %s"), *(ReturnLocation + Dir * CalcDistance).ToString());
		
		FVector tempLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation);
		FVector NewTempLocation = ReturnLocation + Dir * 700;
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir );
		
		DrawDebugSphere(GetWorld(), ReturnLocation, 12.f, 6, FColor::Green, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), tempLocation, 12.f, 6, FColor::Red, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), NewTempLocation, 12.f, 6, FColor::Blue, true, 2.f, (uint8)nullptr, 2.f);

		UE_LOG(LogTemp, Warning, TEXT("         Final Result = %s"), *ReturnLocation.ToString());
	}
	
	return ReturnLocation;
}
