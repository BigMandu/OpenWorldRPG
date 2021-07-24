// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "EquipmentComponent.h"

AWeapon::AWeapon()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());

	SKMesh->SetHiddenInGame(true);
}

void AWeapon::Equip(AActor* Char)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	bool bFlag = false;
	if (Main)
	{
		Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		

		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
		case EWeaponType::EWT_Pistol:
			if (CheckSendToInventory(Main)) //Inventory�� ��������
			{
				bFlag = true;
			}
			else //inventory�� ������ �ʾ����� ������Ų��.
			{
				//������ ���̸� �ȵȴ�. 1,2,3 (�ֹ���, �ι���, ����)Ű�� �������� Attach���Ѿ���.
				if (Main->EquippedWeapon == nullptr)
				{
					GunAttachToMesh(Main);
				}
				else
				{
					//SKMesh->SetHiddenInGame(true);
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
			if(CheckSendToInventory(Main))
			{
				bFlag = true;
				
			}
			else
			{
				SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
			}
			break;

		case EWeaponType::EWT_Vest:
			if (CheckSendToInventory(Main))
			{
				bFlag = true;
				
			}
			else
			{
				SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
			}
			break;
		}		

		
		if (!bFlag) //Inventory�� ������ �ʾ�����
		{
			//Main�� �ִ� Equipment�� Add���ش�.
			Main->Equipment->AddEquipment(this);
		}
	}
}

bool AWeapon::CheckSendToInventory(AMainCharacter* Main)
{
	check(Main);

	//�� ����� ���� Ÿ���� ���Ⱑ �̹� �����Ǿ��ְ� (Rifle�� ��� 2�� ��������)
	if (Main->Equipment->IsWeaponExist(this))
	{
		if (GetItemState() == EItemState::EIS_Spawn) //���忡 �ִ� �����
		{
			//OwningEquipment�� null�� �������ش�.
			OwningEquipment = nullptr;

			//Inventory�� �̵��ؾ���.
			Pickup(Main);
			return true;
		}
		else //Pickup���¸� (Inventory�� �ִ� ������)
		{
			AWeapon* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
			if (Beforeweapon != nullptr)
			{
				Beforeweapon->OwningEquipment = nullptr;
				Beforeweapon->Pickup(Main); //���� ���⸦ Inventory��
			}
			UE_LOG(LogTemp, Warning, TEXT("Weapon::DecideEquipOrInventory. something wrong."));
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
				FPSocket->AttachActor(this, Main->FPMesh);
				break;
			case ECameraMode::ECM_TPS:
				TPSocket->AttachActor(this, Main->GetMesh());
				break;
			}

			SKMesh->SetHiddenInGame(false);
			Main->EquippedWeapon = this;
		}

	}
}