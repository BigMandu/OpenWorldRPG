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
		Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		

		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
		case EWeaponType::EWT_Pistol:
			if (CheckSendToInventory(Main)) //Inventory로 보냈으면
			{
				bFlag = true;
			}
			else //inventory로 보내지 않았으면 장착시킨다.
			{
				//무작정 붙이면 안된다. 1,2,3 (주무기, 부무기, 권총)키를 눌렀을때 Attach시켜야함.
				if (Main->EquippedWeapon == nullptr)
				{
					GunAttachToMesh(Main);
				}
				else
				{
					//SKMesh->SetHiddenInGame(true);
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
				SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
			}
			break;

		case EWeaponType::EWT_Vest:
			if (CheckSendToInventory(Main))
			{
				bFlag = true;
				
			}
			else
			{
				SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
			}
			break;
		}		

		
		if (!bFlag) //Inventory로 보내지 않았으면
		{
			//Main에 있는 Equipment에 Add해준다.
			Main->Equipment->AddEquipment(this);
		}
	}
}

bool AWeapon::CheckSendToInventory(AMainCharacter* Main)
{
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
		else //Pickup상태면 (Inventory에 있는 무기임)
		{
			AWeapon* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
			if (Beforeweapon != nullptr)
			{
				Beforeweapon->OwningEquipment = nullptr;
				Beforeweapon->Pickup(Main); //기존 무기를 Inventory로
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