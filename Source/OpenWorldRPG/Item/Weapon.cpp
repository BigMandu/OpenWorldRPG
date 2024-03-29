// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/MainAnimInstance.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"

#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"
#include "OpenWorldRPG/Item/WeaponParts.h"


#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Curves/CurveFloat.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h" //디버깅용
//#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include <OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h>


#define WeaponDEBUG 0

AWeapon::AWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetupAttachment(RootComponent);

	OptionalStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OptionalMesh"));
	OptionalStaticMesh->SetupAttachment(GetRootComponent());

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;
	RifleAssign = ERifleSlot::ERS_MAX;

	bIsFiring = false;
	bLMBDown = false;
	bDetectLookInput = false;

	bIsTacticalDevEmit = false;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* New Aim System, */
	if(!OwningPlayer || !WeaponDataAsset) return;


	if(OwningPlayer->CharacterStatus != ECharacterStatus::EPS_Dead)
	{
		if(AMainCharacter* Player = Cast<AMainCharacter>(OwningPlayer))
		{
			if (Player->EquippedWeapon == this)
			{
				UpdateAim();
				WeaponClipping();
			}
		}
		else
		{
			WeaponClipping();
		}
	}
	//if ( MainCon != nullptr ) //if (OwningPlayer != nullptr)
	//{
	//	if ( MainCon->Main->EquippedWeapon == this )
	//	{
	//		UpdateAim();
	//		WeaponClipping();
	//		//DrawDebugLine(GetWorld(), GetActorForwardVector(), GetActorLocation() + GetActorRotation().Vector() * 100.f, FColor::Cyan, false, 1.f, 0, 2.f);
	//	}
	//}
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CapsuleComp->SetHiddenInGame(false); //for debug
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponDataAsset = Cast<UWeaponPDA>(ItemSetting.DataAsset);

	if (WeaponDataAsset)
	{
		SettingWeaponPartsManager();
		UpdateWeaponParts();
	}
	
}

void AWeapon::SetMesh()
{
	Super::SetMesh();

	if (ItemSetting.DataAsset->Mesh_Optional)
	{
		OptionalStaticMesh->SetStaticMesh(ItemSetting.DataAsset->Mesh_Optional);
		OptionalStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

bool AWeapon::StepEquip(AActor* Char, ERifleSlot RifleSlot)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Char);
	check(BChar)

	//지정한 값을 쉽게 사용하기 위해 Cast해둔다.
	WeaponDataAsset = Cast<UWeaponPDA>(ItemSetting.DataAsset);
	check(WeaponDataAsset)

	CapsuleAdjust();


	if (RifleAssign == ERifleSlot::ERS_MAX)
	{
		if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
		{
			/* 1,2,3을 눌렀을때 Quick Swap하기 위해 Rifle을 지정한다 */
			if (RifleSlot == ERifleSlot::ERS_MAX) //지정된 Slot이 없을때
			{
				if (BChar->PrimaryWeapon) //이미 주무기가 있으면
				{
					BChar->SetWeaponAssign(this, ERifleSlot::ERS_Sub);
					//SettingRifleAssign(BChar, ERifleSlot::ERS_Sub);
				}
				else //주무기가 없으면
				{
					BChar->SetWeaponAssign(this, ERifleSlot::ERS_Primary);
					//SettingRifleAssign(BChar, ERifleSlot::ERS_Primary);
				}
			}
			//Drag&Drop으로 Equip을 진행했을땐, 아래 분기로 빠진다.
			else
			{
				//RifleAssign = RifleSlot;
				//SettingRifleAssign(BChar, RifleSlot);
				BChar->SetWeaponAssign(this, RifleSlot);

			}
		}
		else if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol)
		{
			BChar->SetWeaponAssign(this, ERifleSlot::ERS_Pistol);
		}
	}

	//들고 있는 무기가 없을 경우 지금 Weapon을 들도록 한다.
	if (BChar->EquippedWeapon == nullptr)
	{
		GunAttachToMesh(BChar);
		CurrentWeaponState = EWeaponState::EWS_Idle;

		if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
		{
			if (BChar->PrimaryWeapon)
			{
				BChar->ChangeWeapon(1);
			}
			else if ( BChar->SubWeapon )
			{
				BChar->ChangeWeapon(2);
			}

			BChar->ChangeWeaponTypeNumber(1);
		}
		else
		{
			BChar->ChangeWeapon(3);
			BChar->ChangeWeaponTypeNumber(2);
		}

		//바로 장착하는 경우, Anim재생이 없으므로 바로 Attach한다.
		
	}
	//들고있는 무기가 있는 경우, 지금 Weapon을 SubSocket으로 옮긴다.
	else
	{
		GunAttachToSubSocket(BChar);
	}

	//Fire시 계산을 편히 하기 위해 값을 미리 세팅 한다.
	WeaponDataAsset->WeaponStat.FireRatePerSec = WeaponDataAsset->WeaponStat.FireRatePerMin / 60;
	WeaponDataAsset->WeaponStat.SecondPerBullet = 1 / WeaponDataAsset->WeaponStat.FireRatePerSec; //0.06


	//모든 세팅이 끝나고 AddEquipment를 호출하기 위해 나중에 호출했다.
	Super::StepEquip(BChar);

	/* 아래는 AddEquipment 이후에 진행 해야할 함수들을 호출..
	* 아래함수들은 ItemObj가 필요함.
	*/
	
	UpdateWeaponParts();

	/**장착 직후 Ammo Widget을 update하기 위해
	 * CheckAmmo로 obj의 AmmoInMag 변수를 불러오고 broadcast를 한다.
	 */
	if (OwningPlayer)
	{
		CheckAmmo();
		OwningPlayer->OnGetAmmo.Broadcast(this);
	}


	return true;
}


/*
*  Weapon을 WeaponGrip Socket에 장착한다.
*/
void AWeapon::GunAttachToMesh(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	AMainCharacter* Main = Cast<AMainCharacter>(BChar);

	const USkeletalMeshSocket* AttachSocket = nullptr;
	FTransform AttachTransform;


	// 1, 3인칭 변경시 weapon attach를 변경한다. (Main에만 적용)
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
					if (WeaponDataAsset)
					{
						SetActorRelativeTransform(WeaponDataAsset->FPMeshAttachTransform);
					}
				}
				break;
			case ECameraMode::ECM_TPS:
				if (TPSocket->AttachActor(this, Main->GetMesh()))
				{
					if (WeaponDataAsset)
					{
						SetActorRelativeTransform(WeaponDataAsset->MeshAttachTransform);
					}
				}
				break;
			}

			if (SKMesh)
			{
				SKMesh->SetHiddenInGame(false);
			}
			//Main->SetEquippedWeapon(this);
		}
	}

	//BChar만 null이 아닌경우 -> AI에만 해당
	if (BChar && Main == nullptr)
	{
		const USkeletalMeshSocket* WeaponSocket = BChar->GetMesh()->GetSocketByName("WeaponGrip");
		if (WeaponSocket)
		{
			if (WeaponSocket->AttachActor(this, BChar->GetMesh()))
			{
				if (WeaponDataAsset)
				{
					SetActorRelativeTransform(WeaponDataAsset->MeshAttachTransform);
				}

				//SKMesh->SetHiddenInGame(false);
				//BChar->SetEquippedWeapon(this);
			}
		}

	}
}


/*
* WeaponGrip이 아닌 AttachSocket에 해당 Weapon을 부착 시킨다.
*/
void AWeapon::GunAttachToSubSocket(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	if (BChar == nullptr) return;

	const USkeletalMeshSocket* AttachSocket = nullptr;
	FTransform AttachTransform;

	if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (RifleAssign)
		{
		case ERifleSlot::ERS_Primary:
			AttachSocket = BChar->GetMesh()->GetSocketByName("PrimaryWeaponAttachSocket");
			AttachTransform = WeaponDataAsset->PrimaryWeaponAttachTransform;
			break;
		case ERifleSlot::ERS_Sub:
			AttachSocket = BChar->GetMesh()->GetSocketByName("SubWeaponAttachSocket");
			AttachTransform = WeaponDataAsset->SubWeaponAttachTransform;
			break;
		}
	}
	else if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol)
	{
		AttachSocket = BChar->GetMesh()->GetSocketByName("PistolAttachSocket");
		AttachTransform = WeaponDataAsset->PistolAttachTransform;
	}


	if (AttachSocket)
	{
		AttachSocket->AttachActor(this, BChar->GetMesh());
		SetActorRelativeTransform(AttachTransform);

		if (WeaponDataAsset->EquippedSound)
		{
			UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->EquippedSound, OwningPlayer->GetRootComponent());
		}
	}


}

FTransform AWeapon::GetSightSocketTransform()
{
	//if have any sight device. return that location.

	//Socket의 위치를 얻기 전에, Weapon의 위치를 강제한다.
	//Animation에 의해 Attach된 Weapon의 위치가 바뀌므로 강제하여 Socket의 일관성 있는 위치를 리턴하기 위함.
	//SetActorRelativeTransform(WeaponDataAsset->FPMeshAttachTransform);

	FTransform ReturnTransform;
	bool bHasSight = false;

	if (ItemObj && ItemObj->WeaponPartsManager)
	{
		if (AEquipment* A_Scope = ItemObj->WeaponPartsManager->GetWeaponParts(EWeaponPartsType::EWPT_Scope))
		{
			ReturnTransform = A_Scope->SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
			bHasSight = true;
		}
	}
	else if(WeaponPartsManager)
	{
		if (AEquipment* A_Scope = WeaponPartsManager->GetWeaponParts(EWeaponPartsType::EWPT_Scope))
		{
			ReturnTransform = A_Scope->SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
			bHasSight = true;
		}

	}
	//need to debug why WPM is gone and when
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / WPM is null "));
	}

	//Sight가 있다면 아래 기본 IronSight는 진행하지 않는다.
	if(bHasSight == false)
	{
		ReturnTransform = SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
	}

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / Sight Socket World : %s"), *ReturnTransform.GetLocation().ToString())
		//FTransform TestTF = SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_ParentBoneSpace);
		/*FVector DebugLocation = TestTF.GetLocation() + GetActorLocation();
		DrawDebugSphere(GetWorld(), DebugLocation, 6.f,6,FColor::Blue,false,10.f,0,3.f);
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / Sight Socket World : %s"), *ReturnTransform.GetLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / TEST TF : %s"), *DebugLocation.ToString());
		*/

		return ReturnTransform;
}

//무조건 생성한다.
void AWeapon::SettingWeaponPartsManager()
{
	WeaponPartsManager = NewObject<UWeaponPartsManagerObject>();
	WeaponPartsManager->SetOwnerWeapon(this);
	WeaponPartsManager->OnChangeParts.AddDynamic(this, &AWeapon::UpdateWeaponParts);

	if (bHasSpawnParts)
	{
		TArray<UCustomPDA*> List1 = UCustomInventoryLibrary::RandomSpawnUsingTableButCount(1,SpawnMuzzlePartsList);
		AddSpawnParts(List1);
		TArray<UCustomPDA*> List2 = UCustomInventoryLibrary::RandomSpawnUsingTableButCount(1, SpawnScopePartsList);
		AddSpawnParts(List2);
		TArray<UCustomPDA*> List3 = UCustomInventoryLibrary::RandomSpawnUsingTableButCount(1, SpawnTacticalPartsList);
		AddSpawnParts(List3);
	}
	

	/*if ( ItemObj && ItemObj->WeaponPartsManager.IsValid() )
	{
		UCustomInventoryLibrary::SetWeaponPartsManager(WeaponPartsManager.Get(), ItemObj->WeaponPartsManager.Get());
		ItemObj->WeaponPartsManager->SetOwnerWeapon(WeaponPartsManager->GetOwnerWeapon());
		ItemObj->WeaponPartsManager->OnChangeParts.AddDynamic(this, &AWeapon::UpdateWeaponParts);
	}*/
}

void AWeapon::AddSpawnParts(TArray<UCustomPDA*>SpawnParts)
{
	for (UCustomPDA* PartsPDA : SpawnParts)
	{
		bool bIsCreated = false;
		//auto SpawnParts = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Cast<UCustomPDA>(PartsPDA));
		UNewItemObject* PartsObj = UCustomInventoryLibrary::CreateObject(FItemSetting(PartsPDA,1,0),bIsCreated); //SpawnParts->ItemSetting, bIsCreated);
		//SpawnParts->Destroy();
		if (bIsCreated)
		{
			WeaponPartsManager->AddParts(PartsObj);
		}
	}
}

void AWeapon::UpdateWeaponParts()
{
	
	if(WeaponDataAsset && (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle ||
	WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (ItemObj && ItemObj->WeaponPartsManager)
		{
			ItemObj->WeaponPartsManager->UpdateParts(GetWorld(), this);
		}
		else if(WeaponPartsManager)
		{
			WeaponPartsManager->UpdateParts(GetWorld(), this);
		}
	}
}


void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();
	if (OwningPlayer == nullptr) return;

	RifleAssign = ERifleSlot::ERS_MAX;
	if (ItemObj)
	{
		ItemObj->RifleAssign = ERifleSlot::ERS_MAX;
	}

	if (OwningPlayer->EquippedWeapon == this)
	{
		OwningPlayer->ChangeWeapon(0);
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove , Remove RifleAssign"));
	}


	if (OwningPlayer->PrimaryWeapon == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Primary..."));
		OwningPlayer->PrimaryWeapon = nullptr;
	}
	else if (OwningPlayer->SubWeapon == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Sub..."));
		OwningPlayer->SubWeapon = nullptr;
	}
	else if (OwningPlayer->PistolWeapon == this)
	{
		OwningPlayer->PistolWeapon = nullptr;
	}

	OwningPlayer = nullptr;
	if (WeaponPartsManager)
	{
		WeaponPartsManager->DestroyAllAttachParts(this);
	}

	//Destory 함수로 Attach된게 Destory안되면 호출 해야됨.

	Destroy();

}


void AWeapon::CapsuleAdjust()
{
	CapsuleComp->SetRelativeTransform(WeaponDataAsset->CapsuleComponentTF);
	CapsuleComp->SetCapsuleHalfHeight(WeaponDataAsset->CapsuleHalfHeight);
	CapsuleComp->SetCapsuleRadius(WeaponDataAsset->CapsuleRadius);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
void AWeapon::ToggleTacticalEquip()
{
	if (WeaponDataAsset)
	{
		if (CurrentWeaponState == EWeaponState::EWS_Idle)
		{
			AWeaponParts* TacticalParts = nullptr;
			if (ItemObj && ItemObj->WeaponPartsManager)
			{
				if ( AEquipment* A_Tactical = ItemObj->WeaponPartsManager->GetWeaponParts(EWeaponPartsType::EWPT_Tactical) )
				{
					//ReturnTransform = A_Scope->SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
					//bHasSight = true;
					TacticalParts = Cast<AWeaponParts>(A_Tactical);
				}
			}
			else if (WeaponPartsManager)
			{
				if ( AEquipment* A_Tactical = WeaponPartsManager->GetWeaponParts(EWeaponPartsType::EWPT_Tactical) )
				{
					//ReturnTransform = A_Scope->SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
					//bHasSight = true;
					TacticalParts = Cast<AWeaponParts>(A_Tactical);
				}

			}

			if(TacticalParts == nullptr) return;

			if (bIsTacticalDevEmit)
			{
				//off
				bIsTacticalDevEmit = false;
				TacticalParts->SetEmitValue(false);
				
				
			}
			else
			{
				//on
				bIsTacticalDevEmit = true;
				TacticalParts->SetEmitValue(true);
			}


		}
	}
}

void AWeapon::ChangeSafetyLever()
{
	//UCustomPDA* CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
	if (WeaponDataAsset && WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_Safe:
			WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
			break;
		case EWeaponFiringMode::EWFM_SemiAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Burst;
			break;
		case EWeaponFiringMode::EWFM_Burst:
			WeaponFiringMode = EWeaponFiringMode::EWFM_FullAuto;
			break;
		case EWeaponFiringMode::EWFM_FullAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Safe;
			break;
		}
	}
}

void AWeapon::ChangeSafetyLeverForAI(EWeaponFiringMode FiringMode)
{
	if (WeaponDataAsset && WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		WeaponFiringMode = FiringMode;
	}
}


void AWeapon::TempNewWeaponState()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::TempNewWeaponState"));
	EWeaponState State = EWeaponState::EWS_Idle;

	if (bLMBDown)
	{
		if (CanFire())//발사를 할 수 있다면, Firing으로 상태를 변경한다.
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("TempState -> Firing"));
#endif
			State = EWeaponState::EWS_Firing;
		}
	}

	//이 임시저장한 State를 Setting하기 위해 SetWeaponState함수를 호출한다.
	SetWeaponState(State);
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::SetWeaponState"));
	//기존에 발사중이었고, 더이상 LMB를 누르지 않는다면
	if (CurrentWeaponState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		// 점사면 끝내도 지정된 몇발 이상 안쐈으면 사격되도록 함.
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		//사격이 중단되어야 할때 EndFiring호출 
		 //점사일때는 냅둔다-> Check Firing함수에서 Firing을 호출하도록 함.
		if (bCanEndFire)
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Call EndFiring func"));
#endif			
			CurrentWeaponState = NewState;
			EndFiring();

		}

	}
	//기존에 발사하지 않았고, LMB를 눌렀다면
	else if (CurrentWeaponState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{

#if WeaponDEBUG
		UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Start Firing Call ControlFiring func"));
#endif			
		//사격을 시작한다.
		CurrentWeaponState = NewState;
		ControlFiring();
	}
}


/*
* 쏠 수 있는 조건
* 1. 조정간 안전이 아니어야한다.
* 2. 장전중, 장착중이 아니어야 한다.
* 3. 탄약이 한발이상 있어야 한다.
* 
* 5. 스프린트 중이 아니어야 한다.
*/
bool AWeapon::CanFire()
{
	bool bCanFire = false;

	//if (ItemObj->bIsDestoryed == false)
	{
		if (WeaponFiringMode != EWeaponFiringMode::EWFM_Safe && CurrentWeaponState != EWeaponState::EWS_Reloading
		&& CurrentWeaponState != EWeaponState::EWS_Equipping && OwningPlayer->bIsSprinting == false)
		{
			//SemiAuto에 한발 이상 사격한게 아닐때만 CheckAmmo && 사격 가능
			// 왜 이렇게 했는지 모르겠다... 필요없어 보여서 뺐는데도 잘 된다.
			//if ( !( WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto && FireCount > 0 ) )
			{
				if (CheckAmmo())
				{
					bCanFire = true;
				}
				else
				{
					//틱틱 하는 Sound 추가하기.
				}
			}
		}
	}
	return bCanFire;
}

int32 AWeapon::GetCurrentAmmoInMag()
{
	return AmmoLeftInMag;
}

bool AWeapon::CheckAmmo()
{
	if (ItemObj)
	{
		AmmoLeftInMag = ItemObj->GetAmmoLeftInMag();
	}

	if (AmmoLeftInMag <= 0)
	{
		// -1이하로 내려갈일은 없지만 방지용으로.
		AmmoLeftInMag = 0;
		return false;
	}
	return true;
}

void AWeapon::UseAmmo()
{
	--AmmoLeftInMag;
	if (ItemObj)
	{
		ItemObj->SetAmmoLeftInMag(AmmoLeftInMag);
	}

	OwningPlayer->OnGetAmmo.Broadcast(this);
}

void AWeapon::Equipping()
{
	if(!WeaponDataAsset || !OwningPlayer) return;

	CurrentWeaponState = EWeaponState::EWS_Equipping;

	if(WeaponDataAsset->WeaponAnimaton.TPS_Actor_EquipAnim && WeaponDataAsset->WeaponAnimaton.FPS_Actor_EquipAnim )
	{
		OwningPlayer->PlayAnimation(WeaponDataAsset->WeaponAnimaton.TPS_Actor_EquipAnim, WeaponDataAsset->WeaponAnimaton.FPS_Actor_EquipAnim);
	}

	if ( WeaponDataAsset->WeaponSound.EquipSound)
	{
		UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->WeaponSound.EquipSound, OwningPlayer->GetRootComponent());
	}

}

void AWeapon::EndEquipping()
{
	CurrentWeaponState = EWeaponState::EWS_Idle;

}

bool AWeapon::CheckCanReload()
{
	//Idle상태가 아닌 상태인 경우 장전 불가.
	if (CurrentWeaponState != EWeaponState::EWS_Idle)
	{
		return false;
	}

	return true;
}

//Reload는 단순히 Animation을 진행하도록 한다.
void AWeapon::Reload()
{
	if (CheckCanReload() == false)
	{
		//UE_LOG(LogTemp,Warning,TEXT("AWeapon::Reload// Can't Reload."
		//"Already reloading or Weapon is busy."));
		return;
	}

	
	bool bHasAmmo = OwningPlayer->CheckAmmo();
	CntAmmoSameType = 0;

	//Player가 Ammo를 갖고 있고 && Mag에 남은 Ammo가 최대 APM보다 적은 경우에만 장전 가능
	if (bHasAmmo && AmmoLeftInMag < WeaponDataAsset->WeaponStat.AmmoPerMag)
	{
		CurrentWeaponState = EWeaponState::EWS_Reloading;

		OwningPlayer->SetLeftHandIK(0.f);

		//Animation 재생
		if ( WeaponDataAsset->WeaponAnimaton.TPS_Actor_ReloadAnim && WeaponDataAsset->WeaponAnimaton.FPS_Actor_ReloadAnim )
		{
			if(OwningPlayer->TPAnimInstance->LeftHandAlpha != 0.f ) return;

			OwningPlayer->PlayAnimation(WeaponDataAsset->WeaponAnimaton.TPS_Actor_ReloadAnim, WeaponDataAsset->WeaponAnimaton.FPS_Actor_ReloadAnim);
		}

		//Sound 재생 -> Notify로 옮김.
		/*if ( WeaponDataAsset->WeaponSound.ReloadSound)
		{
			UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->WeaponSound.ReloadSound, OwningPlayer->GetRootComponent());
		}*/
	}
	else
	{
		//UE_LOG(LogTemp,Warning,TEXT("AWeapon::Reload// Reload Failed! : Ammo is not Vest or Pocket / full ammo in mag."));
	}	
}

//AnimNotify를 통해 이 함수를 호출하면
//탄약을 갱신한다.
void AWeapon::ReloadEnd()
{
	//Current 잔탄에 장전 가능한 탄약을 더한다.
	AmmoLeftInMag += OwningPlayer->GetNumberofCanReload();

	//잔여 ammo를 업데이트 한다.
	CntAmmoSameType = OwningPlayer->GetTotalNumberofSameTypeAmmo();
	if ( ItemObj )
	{
		ItemObj->SetAmmoLeftInMag(AmmoLeftInMag);
	}

	OwningPlayer->SetLeftHandIK(1.f);
	CurrentWeaponState = EWeaponState::EWS_Idle;
	

	OwningPlayer->OnGetAmmo.Broadcast(this);
}

void AWeapon::PlayReloadSound(EPlayReloadSound SoundType)
{
	if (WeaponDataAsset)
	{
		FVector WeaponLo = GetActorLocation();
		USoundCue* Sound = nullptr;
		UWorld* World = GetWorld();
		if(!World) return;

		switch (SoundType)
		{
			case EPlayReloadSound::EPRS_EjectSound:
				Sound = WeaponDataAsset->WeaponSound.Reload_EjectSound;
			break;
			case EPlayReloadSound::EPRS_InsertSound:
				Sound = WeaponDataAsset->WeaponSound.Reload_InsertSound;
			break;
			case EPlayReloadSound::EPRS_BoltReleaseSound:
				Sound = WeaponDataAsset->WeaponSound.Reload_BoltReleaseSound;
			break;

		}
		//UE_LOG(LogTemp,Warning,TEXT("AWeapon::PlayReloadSound"));
		UGameplayStatics::PlaySoundAtLocation(World, Sound, WeaponLo);
		//UGameplayStatics::SpawnSoundAttached(Sound, OwningPlayer->GetRootComponent());
		UAISense_Hearing::ReportNoiseEvent(World, WeaponLo, 0.7f, this);
	}
}

bool AWeapon::CanEndFire()
{
	return false;
}


/* FiringMode에 따라 Delay를 줘야함.
 * semiauto인 경우 toggle방식으로.
 * burst인 경우 3발 사격
 * Fullauto인 경우 연속사격
 *
 * 여기서 timer체크를 한번 해주고 이후에 호출되는 함수를 또 두개로 나눠서 함.
 */
void AWeapon::StartFire()
{
	if (!bLMBDown)
	{
		bLMBDown = true;
		/*
		샷을 때렸을때 기존에 Shot을때리고 있었는지, idling상태였는지 체크해야함.
		샷을 때리고 있는 중이었으면 계속해서 공격 함수 호출(이거말고 다른 함수)
		아니면, 공격을 멈춘 함수호출해야함.(밑 함수 말고 다른거)
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
	/* SetWeaponState에서 호출된다.
	*
	* Timer를 통해 Firing함수를 호출한다.
	* Timer의 시간은
	* (마지막 발사 시간은 발사할때의 월드 타임)
	* 마지막 발사시간 + 발사간격 타임 = 발사 가능 시간 이 된다.
	* 그럼, 이 발사 가능시간이 월드 타임보다 크면 발사를 할 수 없다.
	* 다음 발사 시간은
	* 이 발사가능시간에 월드타임을 빼면 구해진다.
	*
	* 예를 들어, 1초에 쏘고 2초의 간격을 가진다면
	* 3초의 월드타임에 쏠 수 있다.
	* 3초때 쏠 쑤 있는데 현재의 월드타임이 2초라면 쏠 수 없다.
	*
	* 다음 발사시간도예를 들면
	* 3초때 쏠 쑤있는데, 현재 월드타임인 2초를 빼면
	* 1초가 남는데. 1초후에 3초가 되어 쏠 쑤 있다.
	* 이를 식으로 나타내면 된다.
	*/

#if WeaponDEBUG
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::ControlFiring"));
#endif			

	float WorldTime = GetWorld()->GetTimeSeconds();

	// 점사 모드일때는 강제로 시간을 더 추가한다. 
	// Enum을 써도 되지만 if문이 너무 길어져서 boolean변수를 하나 추가한거다.
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//마지막 발사시간과 총의 SPB (발사당 필요한 초)를 더한 값이 worldTime보다 크다면
	// 아직 발사 할 수 없으므로 발사가능 시간을 구한다.f

	//점사모드면 발사 가능 시간을 늦춘 조건으로 검사한다.
	if ((bIsBurstmode && LastFireTime > 0 && LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet * 4 > WorldTime) ||
		(LastFireTime > 0 && LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet > WorldTime))
	{
		//발사 가능시간을 구한다. 
		float RemainingTime = 0.f;// = LastFireTime + WeaponStat.SecondPerBullet - WorldTime;

		if (bIsBurstmode)
		{
			//점사 모드면 SecondPerBullet에 4를 곱한 값.
			RemainingTime = (LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet * 4) - WorldTime;
		}
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::Firing, RemainingTime, false);

	}
	else
	{
		//발사가 가능하다면 바로 호출한다.
		Firing();
	}

}

//Actual Fire
void AWeapon::Firing()
{
	/* ControlFiring, ReFiring에서 호출됨. */
	/*
		ammo 체크, FireCount 증가, 함수 끝에 Timer기록 (마지막 발사 시간 기록용)
		계속해서 사격중이라면 ReFiring함수 호출
	*/

#if WeaponDEBUG
	UE_LOG(LogTemp, Warning, TEXT("Firing function"));
#endif			
	bIsFiring = true;
	AMainCharacter* Main = Cast<AMainCharacter>(GetInstigator());

	if (bDetectLookInput == false)
	{
		if (Main)
		{
			if (Main->bIsLookInput)
			{
				bDetectLookInput = true;
			}
		}
	}

	/* 첫 발사면 ControlRotation값을 저장한다. -> 사격 종료시 원복하기 위함 */
	if ( FireCount == 0 )
	{
		StartFiringRotation = GetInstigatorController()->GetControlRotation();
		//UE_LOG(LogTemp, Warning, TEXT("Save Start Firing Rotation"));
		//UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}


	if ( Main )
	{
		//for player
		New_BulletOut(); //Weapon Instant에 구현함
	}
	else
	{
		//for ai
		AIBulletOut(); //Weapon Instant에 구현함
	}


	WeaponFX();
	UseAmmo();
	FireCount++;

	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::Firing/  Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);

	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponDataAsset->WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();
}

//ReFiring을 할 수 있는지 확인하는 함수다.
void AWeapon::ReFiring()
{
	//실제로 체크하는 함수를 호출한다.
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

// Firing이 끝나면 각종 변수들을 초기화 시켜준다.
void AWeapon::EndFiring()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);

	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode를 위함
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;


	// 사격을 끝냈을때 첫 사격 에임으로 되돌아 오는 기능
	if (AMainCharacter* Player = Cast<AMainCharacter>(OwningPlayer))
	{
		if (Player->EquippedWeapon == this)
		{
			GetWorldTimerManager().ClearTimer(AimInitHandle);
			AimInitialize();
		}
	}

}

//Refire가 가능한지 체크한다.
bool AWeapon::CheckRefire()
{
	bool bFlag = false;
	//격발이 가능한지 우선 체크하고
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			//Burst모드일때는 BurstRound이하로 쐈을때 refire가 가능하다.
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponDataAsset->WeaponStat.BurstRound)
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

// 플레이어의 시점각도 (뷰포트가 바라보고 있는 회전각)를 Vector값으로 구한다.
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

/* 위 함수에서 구한 각과 스타트 로케이션을 내적 벡터 해줘서 최종 탄 시작 위치를 구한다.*/
FVector AWeapon::GetTraceStartLocation(FVector Dir)
{
	check(OwningPlayer)
	check(MainCon)

	FVector ReturnLocation;
	FRotator Rot;

	//ClippingWall 함수가 실행되면, 탄 시작 위치를 Muzzle의 위치로 바꾼다.
	//함수가 실행되지 않으면,  탄시작 위치는 Actor와(Actor-CamLo = Actor의 옆위치가 된다) Dir의 사이각이 된다.

	//For debug
	if (SKMesh->GetSocketByName(MuzzleFlashSocketName) == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetTraceStartLocation / Can not find MuzzleFlashSocket"));
	}

	if (bIsHighReady)
	{
		FVector WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		ReturnLocation = WeaponMuzzleLocation; //GetActorLocation();
	}
	else if (MainCon)
	{
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir);
	}

	return ReturnLocation;
}

FVector AWeapon::GetTraceEndLocation(FVector StartVector, FVector Dir)
{
	FVector ReturnVec;

	//ClippingWall함수가 실행되면 탄의 종료 위치는 Muzzle의 방향*거리가 되며
	//함수가 실행되지 않으면, WorldAimPosition이 된다.
	if (bIsHighReady)
	{
		//Muzzle Flash Rotation값으로 하면 처음은 잘되는데  시간 지나다 보면 Rotation값이 이상하게 되는듯? 자꾸 변한다. -> ActorQuat 절대 축으로 변환함.
		//FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);
		//ReturnVec = StartVector +  MuzzleRotation.Vector() * WeaponDataAsset->WeaponStat.WeaponRange;

		//이 Actor Mesh의 전방 축을 가져온다.
		ReturnVec = StartVector + this->GetActorQuat().GetAxisY() * WeaponDataAsset->WeaponStat.WeaponRange;

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

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator를 IgnoreActor로 하면된다.
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(OwningPlayer);
	params.AddIgnoredComponent(CapsuleComp);

	if (AMainCharacter* Player = Cast<AMainCharacter>(OwningPlayer))
	{
		params.AddIgnoredActor(Player);
	}
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);

	return Hit;
}

/* 사운드와 총구 이펙트 */
void AWeapon::WeaponFX()
{
	//소음기가 있을때
	
	if (WeaponPartsManager && WeaponPartsManager->MuzzleParts)
	{
		//사운드만 재생
		if (WeaponDataAsset->WeaponSound.MuzzleParts_FireSound)
		{
			UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->WeaponSound.FireSound, this->SKMesh);
		}
	}
	//소음기가 없을때
	else
	{
		//사운드
		if (WeaponDataAsset->WeaponSound.FireSound)
		{
			UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->WeaponSound.FireSound, this->SKMesh);
		}

		//총구 이펙트
		if (WeaponDataAsset->FireMuzzleEffect)
		{
			const USkeletalMeshSocket* MuzzleSocket = SKMesh->GetSocketByName(MuzzleFlashSocketName);

			if (MuzzleSocket)
			{
				FVector MuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
				FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponDataAsset->FireMuzzleEffect, MuzzleLocation, MuzzleRotation);
			}
		}
	}

	if (OwningPlayer->AimMode == EAimMode::EAM_Aim)
	{
		OwningPlayer->StopAnimMontage(nullptr);
		OwningPlayer->PlayAnimation(WeaponDataAsset->WeaponAnimaton.TPS_ADS_Actor_FireAnim, WeaponDataAsset->WeaponAnimaton.FPS_ADS_Actor_FireAnim);
	}
	else
	{
		OwningPlayer->StopAnimMontage(nullptr);
		OwningPlayer->PlayAnimation(WeaponDataAsset->WeaponAnimaton.TPS_Actor_FireAnim,WeaponDataAsset->WeaponAnimaton.FPS_Actor_FireAnim);
	}
	//Animatoin 재생.
	//PlayWeaponAnimAndCamShake(FireAnimaton);
}

/*
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
*/

void AWeapon::AimInitialize()
{
	if(GetInstigatorController() == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::AimInitialize"));
	//UE_LOG(LogTemp, Warning, TEXT("Save End Firing Location And Init"));
	EndFiringRotation = GetInstigatorController()->GetControlRotation();

	//fire중 마우스 입력이 감지되면 AimInit을 단순히 아래로 내리게 한다.
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
		AlphaTime = Time / OwningPlayer->StatManagementComponent->GetAimInitRate();//0.8f; // : Time/되돌아오는 시간  (스텟)

	//FRotator LerpAimRotation = FMath::RInterpTo(EndFiringRotation, StartFiringRotation, GetWorld()->GetDeltaSeconds(), 20.f);
	FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
	GetInstigatorController()->SetControlRotation(LerpAimRotation);

	//UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());
		}, GetWorld()->GetDeltaSeconds(), true);
}

void AWeapon::UpdateAim()
{
	FTransform AimPos = GetAimPosition();
	FVector EndVec = AimPos.GetLocation() + AimPos.GetRotation().Vector() * 3000.f;
	FHitResult Hit = BulletTrace(AimPos.GetLocation(), EndVec);
	if(!MainCon || !MainCon->PlayerChar->CameraFPS) return;

	if (Hit.bBlockingHit)
	{
		WorldAimPosition = Hit.Location;

		//FPS시점에서 Trace를 같은곳에 한번 더 쏴서 맞으면 맞은 위치로 AimPos를 옮긴다.
		FHitResult WeaponHit = BulletTrace(MainCon->PlayerChar->CameraFPS->GetComponentLocation(), Hit.Location);
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

	//DrawDebugPoint(GetWorld(), WorldAimPosition, 10.f, FColor::Green, false, -1.f);

}

void AWeapon::WeaponClipping()
{
	FTransform CurrentMeshAttachTransform;
	FVector CurrentMeshRightHand;
	FVector RotVec;
	//Player인 경우 FPS/TPS시점에 따라 HandIK를 다르게 적용한다.

	if(!OwningPlayer || !WeaponDataAsset) return;
	if (!OwningPlayer->GetMesh()) return;

	if (AMainCharacter* Player = Cast<AMainCharacter>(OwningPlayer))
	{
		if(Player->FPMesh == nullptr || Player->GetMesh() == nullptr) return;
		
		//아래 EndLo Vec을 계산할때 사용할 방향벡터
		RotVec = GetAimPosition().GetRotation().GetForwardVector();

		switch (Player->CameraMode)
		{
			case ECameraMode::ECM_FPS:
				CurrentMeshAttachTransform = WeaponDataAsset->FPMeshAttachTransform;
				CurrentMeshRightHand = Player->FPMesh->GetSocketLocation("Hand_R");
			break;
			case ECameraMode::ECM_TPS:
				CurrentMeshAttachTransform = WeaponDataAsset->MeshAttachTransform;
				CurrentMeshRightHand = Player->GetRightHandLocation();// GetMesh()->GetSocketLocation("Hand_R");
			break;
			
		}
	}
	else
	{
		RotVec = OwningPlayer->GetActorForwardVector();

		CurrentMeshAttachTransform = WeaponDataAsset->MeshAttachTransform;
		CurrentMeshRightHand = OwningPlayer->GetRightHandLocation();
	}
	

	//LienTrace의 길이는 Weapon에 부착된 CapsuleComp길이
	float Length = CapsuleComp->GetScaledCapsuleHalfHeight() * 4;

	//방향벡터는 AimPosition으로 한다.
	//FVector EndLo = CurrentMeshRightHand + GetAimPosition().GetRotation().GetForwardVector() * Length;
	FVector EndLo = CurrentMeshRightHand + RotVec * Length;
	TArray<AActor*> IgnoreActorList;
	IgnoreActorList.Add(GetInstigator());
	FHitResult Hit;

	ETraceTypeQuery TTQ = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	//오른쪽 손에서 Aim이 향하는 방향으로 Weapon의 Capsule길이만큼 LineTrace를 쏜다.
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CurrentMeshRightHand, EndLo, 5.f, TTQ, false,
		IgnoreActorList, EDrawDebugTrace::None, Hit, true);

	//이 Line Trace가 Hit되면 앞에 무언가 장애물이 있다는 뜻이고,
	//Weapon의 Roll을 수정해 위쪽으로 회전시킨다. 'HighReady형태로'
	if ( Hit.bBlockingHit )
	{
		//Aim상태 였다면 Aim을 풀어준다.
		if (OwningPlayer->AimMode == EAimMode::EAM_Aim)
		{
			OwningPlayer->SetAimMode(EAimMode::EAM_NotAim);
		}
		FVector CurVec = CurrentMeshAttachTransform.GetTranslation();
		//FVector CalcVec = CurrentMeshAttachTransform.GetTranslation() - (NewY * 50.f);

		FRotator CurRot = CurrentMeshAttachTransform.GetRotation().Rotator();

		//Why Roll?? -> Skeleton에서 무기를 넣고 위로 돌리니 Roll값만 바껴서 Roll로 한거임.
		// Hit.Distance / Length하게되면 0부터 1이 나오는데.
		// Hit.Distance == Length -> 1이됨. 즉, 1일때는 0이되고 0일때는 90도가 되어야함.
		// 때문에 90을 곱했고, 90을 걍 곱해버리면 1일때 90이 되버림. 그리고 어차피 각도가 -90이 되야 위로 올라가니
		// 0*90 = 0, 0.5*90 = 45, 1*90 = 90이라 걍 -90을 빼버렸음,
		// 최종값은 0일때 -90, 0.5일때 -45, 1일때 0이 나옴.
		FRotator CalcRot = FRotator(CurRot.Pitch, CurRot.Yaw, ( CurRot.Roll + ( Hit.Distance / Length ) * 90.f ) - 90.f);
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

	//UE_LOG(LogTemp,Warning,TEXT("high Ready is %s"),bIsHighReady ? "true" : "false");
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
			//Weaponclass에 있는 bIsHighReady는 AnimInstance에서 완전히 내려왔을때 false를 시켜주도록 하자.
			bIsHighReady = true;
			//Main->TPAnimInstance->bBeginHighReady = true;
			//Main->FPAnimInstance->bBeginHighReady = true;
			// New Fix Clipping wall
			//Tick에서 Sphere Line Trace를 쏘는데, distance와 radius는 설정 가능함.(총마다 다를수 있으니..)
			//hit이 있다면, Original Transform에서 Location값  - (0.f,0.f, Curve.evaluate(hit.distance / distance)
			//이렇게 해서 나온값을 NewTransform.location으로 지정해주면 된다.
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