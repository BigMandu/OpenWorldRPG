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
			SetInstigator(Main); //Instigator 설정.
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
		if (CheckSendToInventory(Main)) //인벤토리로 보냈는지 확인
		{
			bFlag = true;
			return;
		}

		switch (EquipmentType)
		{
		case EEquipmentType::EET_Rifle:
		case EEquipmentType::EET_Pistol:
		{
			
			//if (!bFlag) //Inventory로 보내지 않았으면 장착 시킨다.
			{
				AWeapon* Weapon = Cast<AWeapon>(this);
				/* 1,2,3을 눌렀을때 Quick Swap하기 위해 */
				if (EquipmentType == EEquipmentType::EET_Rifle && Weapon) //라이플이고
				{
					if (Main->PrimaryWeapon) //이미 주무기가 있으면
					{
						Main->SubWeapon = Weapon;// (AWeapon*)this; //부무기로 지정
					}
					else //주무기가 없으면
					{
						Main->PrimaryWeapon = Weapon;// (AWeapon*)this; //주무기로
					}
				}
				else //피스톨
				{
					Main->PistolWeapon = Weapon; // (AWeapon*)this;
				}

				//들고 있는 무기가 없을 경우 지금 Weapon을 들도록 한다.
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
				//아래 조건으로 하는것 보다 위의 조건으로 하는게 더 나은듯.
				if (GetItemState() == EItemState::EIS_Spawn) //땅에 있는 Item이면,
				{
						같은타입 무기가 이미 장착 -> 땅에있는 Item을 Inventory로
						같은 타입 무기가 없음 -> 땅에있는 Item을 장착.

						여기서 Main의 EquipCompo넌트에서 검색해야함. 결과는 true or false리턴.

				}
				else //이미 Inventory에 있는 Item이면
				{
					 같은 타입 무기가 이미 장착 -> 기존 무기를 Inventory로, 땅Item을 장착
					 같은 타입 무기가 없음 -> 땅에 있는 Item을 장착


				}
				//카메라 모드에 따라 1인칭, 3인칭 Mesh에 부착시킨다.
				GunAttachToMesh(Main); //아래코드를 함수로 대체.
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
				
				//SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
			}
			break;
		case EEquipmentType::EET_Vest:
			//if (!bFlag)
			{
				//장착
				const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("VestSocket");
				
				Socket->AttachActor(this, Main->GetMesh());

				SKMesh->SetHiddenInGame(false); //임시로 해둔것임.
			}
			break;
		case EEquipmentType::EET_Backpack:
			//if (!bFlag)
			{
				//SKMesh->SetHiddenInGame(true); //임시
			}
		}



		//if (!bFlag) //Inventory로 보내지 않았으면
		{
			UE_LOG(LogTemp, Warning, TEXT("AWeapon::Equip Success"));

			//Main에 있는 Equipment에 Add해준다.
			Main->Equipment->AddEquipment(this);
			SetOwningPlayer(Main);

			Mesh->SetSimulatePhysics(false);
			Mesh->SetEnableGravity(false);

			Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
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

	//이 무기와 같은 타입의 무기가 이미 장착되어있고 (Rifle인 경우 2개 장착가능)
	if (Main->Equipment->IsWeaponExist(this))
	{
		if (GetItemState() == EItemState::EIS_Spawn) //월드에 있는 무기면
		{
			//OwningEquipment를 null로 설정해준다.
			OwningEquipment = nullptr;

			//Inventory로 이동해야함.
			Pickup(Main);
			return true;
		}
		else //Pickup상태면 (Inventory에 있는 무기임) -> 스왑해준다 //얘는 따로 빼서 함수를 구현해야할듯하다.
		{
			AEquipment* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
			if (Beforeweapon != nullptr)
			{
				Beforeweapon->OwningEquipment = nullptr;
				Beforeweapon->Pickup(Main); //기존 무기를 Inventory로 보낸다. 
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