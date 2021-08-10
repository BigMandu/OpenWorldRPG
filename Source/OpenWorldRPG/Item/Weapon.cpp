// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "EquipmentComponent.h"

AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer) : Super()
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
		OwningPlayer = Main;
	}
}

void AWeapon::Equip(AActor* Char)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	bool bFlag = false;
	if (Main)
	{
		if (CheckSendToInventory(Main)) //인벤토리로 보냈으면
		{
			bFlag = true;
		}

		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
		case EWeaponType::EWT_Pistol:
			if (!bFlag) //Inventory로 보내지 않았으면 장착 시킨다.
			{				
				/* 1,2,3을 눌렀을때 Quick Swap하기 위해 */
				if (WeaponType == EWeaponType::EWT_Rifle) //라이플이고
				{
					if (Main->PrimaryWeapon) //이미 주무기가 있으면
					{
						Main->SubWeapon = this; //부무기로 지정
					}
					else //주무기가 없으면
					{
						Main->PrimaryWeapon = this; //주무기로
					}
				}
				else //피스톨
				{
					Main->PistolWeapon = this;
				}

				//아무 무기가 없을 경우 지금 Weapon을 장착시킨다.
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
			if(!bFlag)
			{
				//장착
				SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
			}
			break;
		case EWeaponType::EWT_Vest:
			if (!bFlag)
			{
				//장착
				SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
			}
			break;
		}		

		
		if (!bFlag) //Inventory로 보내지 않았으면
		{
			UE_LOG(LogTemp, Warning, TEXT("AWeapon::Equip Success"));

			//Main에 있는 Equipment에 Add해준다.
			Main->Equipment->AddEquipment(this);
			SetOwningPlayer(Main);

			Mesh->SetSimulatePhysics(false);
			Mesh->SetEnableGravity(false);

			Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

bool AWeapon::CheckSendToInventory(AMainCharacter* Main)
{
	check(Main);

	//이 무기와 같은 타입의 무기가 이미 장착되어있고 (Rifle인 경우 2개 장착가능)
	if (Main->Equipment->IsWeaponExist(this))
	{
		if (GetItemState() == EItemState::EIS_Spawn || GetItemState() == EItemState::EIS_Drop) //월드에 있는 무기면
		{
			//OwningEquipment를 null로 설정해준다.
			OwningEquipment = nullptr;

			//Inventory로 이동해야함.
			Pickup(Main);
			return true;
		}
		else //Pickup상태면 (Inventory에 있는 무기임) -> 얘는 따로 빼서 함수를 구현해야할듯하다.
		{
			AWeapon* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
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

			if (Main->EquippedWeapon)//이미 들고있는 무기가 있을경우
			{
				//해당 무기의 SkMesh를 안보이게 한다.
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

	/* Weapon Rifle이 드랍될때
	*  몇개씩 땅 밑으로 꺼진다... 디버깅 해봐도 collision은 제대로 세팅되는데도
	*  꺼진다. - 원인파악 불가.
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
	/* 여기서 FiringMode에 따라 Delay를 줘야함.
	*	semiauto인 경우 toggle방식으로.
	*	burst인 경우 3발 사격
	*  Fullauto인 경우 연속사격
	* 
	* 여기서 timer체크를 한번 해주고 이후에 호출되는 함수를 또 두개로 나눠서 함.
	*/
	if (!bIsFiring)
	{
		bIsFiring = true;
		/*
		샷을 때렸을때 기존에 Shot을때리고 있었는지, idling상태였는지 체크해야함.
		샷을 때리고 있는 중이었으면 계속해서 공격 함수 호출(이거말고 다른 함수)
		아니면, 공격을 멈춘 함수호출해야함.(밑 함수 말고 다른거)
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
	/* Start Fire, ReFiring에서 호출됨. */
	/*
		ammo 체크, FireCount 증가, 함수 끝에 Timer기록 (마지막 발사 시간 기록용)
		계속해서 사격중이라면 ReFiring함수 호출
	*/

	BulletOut(); //Weapon Instant에 구현함.
}

void AWeapon::ReFiring()
{
	//여기서는 격발 횟수 (FireCount)에 따라 Firing호출할지 말지 결정하자.
	Firing();
}


void AWeapon::BulletOut()
{
	UE_LOG(LogTemp, Warning, TEXT("Weap::BulletOut"));
}

FTransform AWeapon::GetCamLocRot()
{
	/*
	AMainController* MainCon;
	if (OwningPlayer)
	{
		MainCon = Cast<AMainController>(OwningPlayer->MainController);
	}
	*/
	AMainController* MainCon = OwningPlayer ? Cast<AMainController>(OwningPlayer->MainController) : nullptr;
	FTransform form;
	if (MainCon)
	{
		FVector Loc = FVector::ZeroVector;
		FRotator Rot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(Loc, Rot);
		form.SetLocation(Loc);
		form.SetRotation(Rot.Quaternion());

	}
	return form;
}