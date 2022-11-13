// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"

#include "MainController.h"
#include "AI/EnemyAIController.h"
#include "MainAnimInstance.h"

#include "OpenWorldRPG/GameData/StatManagementComponent.h"

#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Widget/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"





// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //회전속도
	GetCharacterMovement()->JumpZVelocity = 440.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; //웅크리기를 할 수 있도록 true로 해준다.
	GetCharacterMovement()->CrouchedHalfHeight = GetDefaultHalfHeight() / 1.4f; //웅크린 크기를 기본HalfHeight의 /1.6으로 지정한다.

	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	bIsWalking = false; //걷기 기본설정은 false

	StatManagementComponent = CreateDefaultSubobject<UStatManagementComponent>(TEXT("StatManageComp"));
	BaseInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("BaseInventoryComp"));
	//PocketInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("PocketInventoryComp"));
	//SecureBoxInventoryComp= CreateDefaultSubobject<UNewInventoryComponent>(TEXT("SecureBoxInventoryComp"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	LootWidgetComp = CreateDefaultSubobject<ULootWidgetComponent>(TEXT("LootWidgetComp"));
	//AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	//AudioComp->SetupAttachment(GetRootComponent());

	//decal을 무시한다.
	GetMesh()->SetReceivesDecals(false);

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StatManagementComponent)
	{
		StatManagementComponent->OnHPZero.AddDynamic(this,&ABaseCharacter::Die);
	}
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector VecLength = FVector(0.f);
	if (GetVelocity().Size() > VecLength.Size())
	{
		bIsAccelerating = true;
	}
	else
	{
		bIsAccelerating = false;
	}

}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	if (TPAnimInstance)
	{
		/* 사운드는 TP Animation을 기준으로 출력한다. */ //AnimInstance의 StepSound_Notify에서 호출.
		TPAnimInstance->StepSound.AddUObject(this, &ABaseCharacter::StepSound);
		TPAnimInstance->WeaponTypeNumber = 0;
	}

	SetTeamType(TeamType);
	SettingStorage();
	if (bHasSpawnItems)
	{
		SpawnItems();
	}

	

}

void ABaseCharacter::SetTeamType(ETeamType Team)
{
	TeamType = Team;
}

void ABaseCharacter::SettingStorage()
{
	PocketStorage = NewObject<UItemStorageObject>();
	SecureBoxStorage = NewObject<UItemStorageObject>();

	

	if (PocketStorage && SecureBoxStorage)
	{
		PocketStorage->InitializeStorage(PocketSizeX, PocketSizeY, PocketTileSize);
		SecureBoxStorage->InitializeStorage(SecureSizeX, SecureSizeY, SecureTileSize);
	}
	

}

void ABaseCharacter::SetCharacterStatus(ECharacterStatus Type)
{
	ChracterStatus = Type;
	switch (ChracterStatus)
	{
	case ECharacterStatus::EPS_Normal:
		//StatManagementComponent->
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		bIsWalking = false;
		bIsSprinting = false;
		break;
	case ECharacterStatus::EPS_Walk:
		GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
		bIsWalking = true;
		bIsSprinting = false;
		break;
	case ECharacterStatus::EPS_Sprint:
		GetCharacterMovement()->MaxWalkSpeed = 1300.f;
		bIsSprinting = true;
		break;
	default:
		break;
	}
}

void ABaseCharacter::SpawnItems()
{
	for (auto& AddItem : SpawnItemList)
	{
		AItem* Item = GetWorld()->SpawnActor<AItem>(AddItem);
		if (Item)
		{
			//if (PocketInventoryComp->TryAddItem(Item->GetDefaultItemObj()))
			//if(PocketStorage->TryAddItem()
			
			if(BaseInventoryComp->TryAddItem(PocketStorage,Item->ItemSetting))
			{
				Item->SetItemState(EItemState::EIS_Pickup);
				//Item->ItemObj->bIsDestoryed = true;
				Item->Destroy();
			}
		}
	}
}

void ABaseCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		EquippedWeapon = Weapon;
	}
}


void ABaseCharacter::UseItem(class AActor* Item)
{
	if (Item)
	{
		//Item->Use(this);
	}
}

// AIController::ItemFarming함수에서 사용됨
UNewInventoryComponent* ABaseCharacter::GetAllInvComp(int32 index)
{
	switch (index)
	{
	case 0:

		/*
		AEquipment* Backpack = Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Backpack);
		if (Backpack && Backpack->bHasStorage)
		{
			//return Backpack->EquipInventoryComp;
		}
		*/

		break;
	case 1:

		/*
		AEquipment* Vest = Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Vest);
		if (Vest && Vest->bHasStorage)
		{
			//return Vest->EquipInventoryComp;
		}
		*/
		break;


	case 2:
		//return this->PocketInventoryComp;
		break;
	case 3:
		//return this->SecureBoxInventoryComp;
		break;
	}

	return nullptr;
}

void ABaseCharacter::ReloadWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Reload();
		EquippedWeapon->ReloadEnd();
	}
}

int32 ABaseCharacter::GetNumberofCanReload()
{
	UItemStorageObject* Storage = Cast<UItemStorageObject>(Equipment->GetEquipStorage(EEquipmentType::EET_Vest));
	if(EquippedWeapon)
	{
		int32 APM = EquippedWeapon->WeaponDataAsset->WeaponStat.AmmoPerMag;
		int32 CurAmmoInMag = EquippedWeapon->GetCurrentAmmoInMag();		

		//Ammo Per Mag - Current Ammo In Mag = Number of Ammo To Fill Mag (NeedToFillAmmoCnt)
		int32 NeedToFillAmmoCount = FMath::Clamp<int32>(APM-CurAmmoInMag,0,APM);
		int32 TempNTFAC = NeedToFillAmmoCount;

		if (Storage)
		{
			NeedToFillAmmoCount = CollectRemainAmmo(Storage,NeedToFillAmmoCount);
		}

		NeedToFillAmmoCount = CollectRemainAmmo(PocketStorage, NeedToFillAmmoCount);

		//AmmoPerMag에서 남은 잔탄을 뺀, 장전 가능한 탄약 개수를 리턴한다.
		return TempNTFAC - NeedToFillAmmoCount;
	}
	return 0;
}

//Storage에서 ammo를 찾아 장전시 필요한 탄약 개수를 구하고, 모든 탄약을 찾지 못한 경우 남은 탄약의 개수를 리턴한다.
int32 ABaseCharacter::CollectRemainAmmo(UItemStorageObject* Storage, int32 NumberofAmmoReq)
{
	int32 NumberofRemainingAmmoReq = NumberofAmmoReq;
	for (auto item : Storage->Inventory)
	{
		if (item && item->ItemInfo.DataAsset->ItemType == EItemType::EIT_Ammo)
		{
			if (EquippedWeapon->WeaponDataAsset->WeaponStat.AmmoType == item->ItemInfo.DataAsset->AmmoType)
			{
				if (NumberofRemainingAmmoReq <= 0)
				{
					return 0;
				}
				//item의 개수가 필요개수 이상이면 필요개수만큼 지우고 리턴한다.
				if (item->ItemInfo.Count >= NumberofRemainingAmmoReq)
				{
					BaseInventoryComp->RemoveItemCount(item, NumberofRemainingAmmoReq);
					return 0;
				}
				else
				{
					//그게 아니라면, 필요개수에서 item개수를 뺀 값을 저장하고, for문을 돌면서 계속 갱신한다.
					NumberofRemainingAmmoReq = FMath::Clamp<int32>(NumberofRemainingAmmoReq - item->ItemInfo.Count,0, NumberofRemainingAmmoReq - item->ItemInfo.Count);
					BaseInventoryComp->RemoveItemCount(item, item->ItemInfo.Count);
				}
			}
		}
	}
	return NumberofRemainingAmmoReq;
}

int32 ABaseCharacter::GetTotalNumberofSameTypeAmmo()
{
	int32 Ammocnt = 0;
	if(EquippedWeapon)
	{
		
		UItemStorageObject* Storage = Cast<UItemStorageObject>(Equipment->GetEquipStorage(EEquipmentType::EET_Vest));
		if (Storage)
		{
			CheckAmmoStep(Storage, Ammocnt);
		}
		CheckAmmoStep(PocketStorage, Ammocnt);
		
	}
	return Ammocnt;
}

/*
	EquippedWeapon의 AmmoType과 맞는 Type의 Ammo가 있다면 true를 리턴한다.
*/
bool ABaseCharacter::CheckAmmo()
{
	bool bHasAmmo = false;
	int32 Dummyval = 0;
	if (EquippedWeapon)
	{
		UItemStorageObject* Storage = Cast<UItemStorageObject>(Equipment->GetEquipStorage(EEquipmentType::EET_Vest));
		if (Storage)
		{
			bHasAmmo = CheckAmmoStep(Storage, Dummyval,true);
		}

		if (bHasAmmo == false)
		{
			bHasAmmo = CheckAmmoStep(PocketStorage, Dummyval,true);
		}
	}
	return bHasAmmo;
}


/* @Storage : Ammo를 검색할 스토리지
*  @bIsCheckAmmo : True면, 오로지 Ammo의 존재 여부만 체크하기 위해 함수가 동작된다.
*  @AmmoCnt : bIsCheckAmmo가 False여야 이 파라미터가 사용되며, 동일한 Ammo Type의 총 개수를 구하기 위함이디ㅏ.
*/
bool ABaseCharacter::CheckAmmoStep(UItemStorageObject* Storage, int32& AmmoCnt, bool bIsCheckAmmo)
{	
	for (auto item : Storage->Inventory)
	{
		if (item && item->ItemInfo.DataAsset->ItemType == EItemType::EIT_Ammo)
		{
			if (EquippedWeapon->WeaponDataAsset->WeaponStat.AmmoType == item->ItemInfo.DataAsset->AmmoType)
			{	
				if(bIsCheckAmmo)
				{ 
					return true;
				}
				AmmoCnt += item->ItemInfo.Count;
			}
		}
	}
	return false;
}

//RifleSlot이 MAX인 경우 Pistol로 지정한다.
void ABaseCharacter::SetWeaponAssign(AWeapon* Weapon, ERifleSlot RifleSlot)
{
	if(Weapon == nullptr) return;

	if (Weapon->WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (RifleSlot)
		{
		case ERifleSlot::ERS_Primary:
			PrimaryWeapon = Weapon;
		break;
		case ERifleSlot::ERS_Sub:
			SubWeapon = Weapon;
		break;
		}
	}
	else if (Weapon->WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol)
	{
		PistolWeapon = Weapon;
	}

	Weapon->RifleAssign = RifleSlot;
	if (Weapon->ItemObj)
	{
		Weapon->ItemObj->RifleAssign = RifleSlot;
	}
}

void ABaseCharacter::ChangeSafetyLever()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ChangeSafetyLever();

		OnChangeMode.Broadcast(EquippedWeapon);
	}
}

void ABaseCharacter::ChangeWeapon(int32 index)
{
	if (TPAnimInstance)// && FPAnimInstance)
	{
		switch (index)
		{
		case 0:
			TPAnimInstance->WeaponTypeNumber = 0;
			//FPAnimInstance->WeaponTypeNumber = 0;
			break;
		case 1:
			// 현재 장착하고 있는 무기가 Primary와 다를경우에만 변경. 일치하면 똑같은걸 장착할 필요가 없음.
			if (PrimaryWeapon && (PrimaryWeapon != EquippedWeapon))
			{
				if(EquippedWeapon)
					EquippedWeapon->GunAttachToSubSocket(this);

				PrimaryWeapon->GunAttachToMesh(this);
				TPAnimInstance->WeaponTypeNumber = 1;
				//FPAnimInstance->WeaponTypeNumber = 1;
				EquippedWeapon = PrimaryWeapon;
				
			}
			break;
		case 2:
			if (SubWeapon && (SubWeapon != EquippedWeapon))
			{
				if (EquippedWeapon)
					EquippedWeapon->GunAttachToSubSocket(this);

				SubWeapon->GunAttachToMesh(this);
				TPAnimInstance->WeaponTypeNumber = 1;
				//FPAnimInstance->WeaponTypeNumber = 1;
				EquippedWeapon = SubWeapon;
			}
			break;
		case 3:
			if (PistolWeapon && (PistolWeapon != EquippedWeapon))
			{
				if (EquippedWeapon) 
					EquippedWeapon->GunAttachToSubSocket(this);

				PistolWeapon->GunAttachToMesh(this);
				TPAnimInstance->WeaponTypeNumber = 2;
				//FPAnimInstance->WeaponTypeNumber = 2;
				EquippedWeapon = PistolWeapon;			
			}
			break;
		}
		
	}
}

void ABaseCharacter::ChangePrimaryWeapon()
{
	ChangeWeapon(1);
}
void ABaseCharacter::ChangeSubWeapon()
{
	ChangeWeapon(2);
}
void ABaseCharacter::ChangePistolWeapon()
{
	ChangeWeapon(3);
}




FTransform ABaseCharacter::LeftHandik()
{
	FTransform Transform;
	if (EquippedWeapon)
	{
		Transform = EquippedWeapon->SKMesh->GetSocketTransform(WeaponLeftHandSocketName, ERelativeTransformSpace::RTS_World);
		return Transform;
	}

	return Transform;
}


void ABaseCharacter::SpeakSound(USoundCue* Sound)
{
	if (Sound)
	{
		AudioComp->Stop(); //-> Play함수 내부에서 Stop함수를 호출 하기때문에 무의미.
		AudioComp->SetSound(Sound);
		AudioComp->Play();
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(),Sound,GetMesh()->GetSocketLocation(HeadSocketName));
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
	}
}

void ABaseCharacter::StepSound()
{
	if (StepSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSoundCue, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,	AActor* DamageCauser)
{
	float ApplyDmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(ApplyDmg > 0.f)
	{
		StatManagementComponent->DamageApply(ApplyDmg);
	}
	return ApplyDmg;
}

void ABaseCharacter::Die()
{
	AEnemyAIController* AICon = Cast<AEnemyAIController>(GetController());
	if(AICon)
	{
		//AICon->UnPossess();
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
		
		/* set Ragdoll */
		USkeletalMeshComponent* TPMesh = GetMesh();
		if(TPMesh)
		{
			TPMesh->SetCollisionProfileName(FName("RagDoll"));

			TPMesh->SetSimulatePhysics(true);
			TPMesh->WakeAllRigidBodies();
			TPMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			
			bIsDie = true;
		}
	}
}

/* Perception */

bool ABaseCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
                                   int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible,
                                   int32* UserData) const
{
	FHitResult HitResult;
	FVector PlayerLocation;
	bool bResult = false;

	const USkeletalMeshSocket* Head = GetMesh()->GetSocketByName(HeadSocketName);
	if (Head) //Head socket이 있으면 이 socket의 위치를, 
	{
		PlayerLocation = GetMesh()->GetSocketLocation(HeadSocketName);
	}
	else PlayerLocation = GetActorLocation(); //없으면 Actor의 위치를(정가운데)


	// FCollisionObjectQueryParams에는 	FCollisionObjectQueryParams(int32 InObjectTypesToQuery)를 사용할거다.
	// 이를 사용하기 위해서는 To do this, use ECC_TO_BITFIELD to convert to bit field 이렇게 하라고 한다. 

	//WorldDynamic, WorldStatic, IgnoreActor를 (관측자의 위치에서 Player의 위치의 범위) LineTrace로 감지. 
	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, PlayerLocation
		, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_WorldStatic))
		, FCollisionQueryParams(FName(TEXT("SightSense")), true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	//위에서 감지된게 아니거나, Actor가 Player라면
	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = PlayerLocation;
		OutSightStrength = 1;
		bResult = true;
		//UE_LOG(LogTemp, Warning, TEXT("Player:: Catch"));
		//UE_LOG(LogTemp, Warning, TEXT("OutSeenLocation : %s, OutSightStrength : %f"), *OutSeenLocation.ToString(), OutSightStrength);
	}
	else
	{
		OutSightStrength = 0;
		//UE_LOG(LogTemp, Warning, TEXT("Player:: Hiding"));
	}
	return bResult;
}




/**********************************************************/
/**************        Interaction        *****************/
/**********************************************************/
void ABaseCharacter::Interaction(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (Pawn && bIsDie)
	{

		//UE_LOG(LogTemp, Warning, TEXT("Actor name : %s"), *Actor->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Character Interaction"));

		AMainController* MainCon = Cast<AMainController>(Pawn->GetController());
		AEnemyAIController* AICon = Cast<AEnemyAIController>(Pawn->GetController());

		if (MainCon)
		{
			MainCon->bIsInteractCharacterLoot = true;
			LootWidgetComp->CreateInteractionWidget(MainCon, this); //새로추가
		}

		if (AICon)
		{
			AICon->ItemFarming(this);
		}

	}
}

void ABaseCharacter::SetOutline()
{
	if (bIsDie)
	{
		GetMesh()->SetRenderCustomDepth(true);
	}
}

void ABaseCharacter::UnsetOutline()
{
	GetMesh()->SetRenderCustomDepth(false);
}