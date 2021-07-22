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
	if (Main)
	{
		Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SKMesh->SetHiddenInGame(false);

		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
				CheckSendToInventory(Main);
				
				//������ ���̸� �ȵȴ�. 1,2,3 (�ֹ���, �ι���, ����)Ű�� �������� Attach���Ѿ���.
				if (Main->EquippedWeapon == nullptr)
				{
					AttachToMesh(Main); 
				}
				else
				{
					SKMesh->SetHiddenInGame(true);
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
				AttachToMesh(Main); //�Ʒ��ڵ带 �Լ��� ��ü.
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
				break;

		case EWeaponType::EWT_Pistol:
				CheckSendToInventory(Main);

				//������ ���̸� �ȵȴ�. 1,2,3 (�ֹ���, �ι���, ����)Ű�� �������� Attach���Ѿ���.
				if (Main->EquippedWeapon == nullptr)
				{
					AttachToMesh(Main);
				}
				else
				{
					SKMesh->SetHiddenInGame(true);
				}
				/*if (EquippedSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
				}*/

				//EquippedWeapon�� ���� ��� �ִ� ����� �����Ѵ�. 
				//Main->EquippedWeapon = this;
				break;

		case EWeaponType::EWT_Helmet:
			CheckSendToInventory(Main);
			SKMesh->bHiddenInGame = true; //�ӽ÷� �صа���.
			break;

		case EWeaponType::EWT_Vest:
			CheckSendToInventory(Main);
			SKMesh->bHiddenInGame = true; //�ӽ÷� �صа�.
			break;
		}		

		//Main�� �ִ� Equipment�� Add���ش�.
		Main->Equipment->AddEquipment(this);
	}
}

void AWeapon::CheckSendToInventory(AMainCharacter* Main)
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
		}

	}
}

void AWeapon::AttachToMesh(AMainCharacter* Main)
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
	
			Main->EquippedWeapon = this;
		}

	}
}