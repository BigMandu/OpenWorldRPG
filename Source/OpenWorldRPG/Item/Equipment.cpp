// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AEquipment::AEquipment() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	EquipInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	SKMesh->SetHiddenInGame(true);

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	if (bHasStorage)
	{
		AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (WEquipGridWidget && MainCon)
		{
			UNewInventoryGrid* Grid = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			//EquipGridWidget = CreateWidget<UNewInventoryGrid>(this, WEquipGridWidget);
			//EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
		}
	}
}

void AEquipment::SetOwningPlayer(AActor* Actor)
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

void AEquipment::Equip(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::Equip func called"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	bool bFlag = false;


	if (Main)
	{
		if (CheckSendToInventory(Main)) //�κ��丮�� ���´��� Ȯ��
		{
			bFlag = true;
			return;
		}

		switch (EquipmentType)
		{
		case EEquipmentType::EET_Rifle:
		case EEquipmentType::EET_Pistol:
		{
			
			//if (!bFlag) //Inventory�� ������ �ʾ����� ���� ��Ų��.
			{
				AWeapon* Weapon = Cast<AWeapon>(this);
				/* 1,2,3�� �������� Quick Swap�ϱ� ���� */
				if (EquipmentType == EEquipmentType::EET_Rifle && Weapon) //�������̰�
				{
					if (Main->PrimaryWeapon) //�̹� �ֹ��Ⱑ ������
					{
						Main->SubWeapon = Weapon;// (AWeapon*)this; //�ι���� ����
					}
					else //�ֹ��Ⱑ ������
					{
						Main->PrimaryWeapon = Weapon;// (AWeapon*)this; //�ֹ����
					}
				}
				else //�ǽ���
				{
					Main->PistolWeapon = Weapon; // (AWeapon*)this;
				}

				//��� �ִ� ���Ⱑ ���� ��� ���� Weapon�� �鵵�� �Ѵ�.
				if (Main->EquippedWeapon == nullptr)
				{
					if (EquipmentType == EEquipmentType::EET_Rifle)
					{
						Main->ChangeWeapon(1);
					}
					else
					{
						Main->ChangeWeapon(3);
					}

					//GunAttachToMesh(Main);
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
		}
			break;
		case EEquipmentType::EET_Helmet:
			//if (!bFlag)
			{
				
				//SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
			}
			break;
		case EEquipmentType::EET_Vest:
			//if (!bFlag)
			{
				//����
				const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("VestSocket");
				
				Socket->AttachActor(this, Main->GetMesh());

				SKMesh->SetHiddenInGame(false); //�ӽ÷� �صа���.
			}
			break;
		case EEquipmentType::EET_Backpack:
			//if (!bFlag)
			{
				//SKMesh->SetHiddenInGame(true); //�ӽ�
			}
		}



		//if (!bFlag) //Inventory�� ������ �ʾ�����
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

bool AEquipment::CheckSendToInventory(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
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
		else //Pickup���¸� (Inventory�� �ִ� ������) -> �������ش� //��� ���� ���� �Լ��� �����ؾ��ҵ��ϴ�.
		{
			AEquipment* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
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

void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}