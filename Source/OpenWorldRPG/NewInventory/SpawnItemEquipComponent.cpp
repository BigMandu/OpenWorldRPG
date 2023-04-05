// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/SpawnItemEquipComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"


#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/BasePDA.h"

#include "OpenWorldRPG/BaseCharacter.h"


USpawnItemEquipComponent::USpawnItemEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//if this true, bhasSpawnItems And bHasSpawnEquipments must be false;
	bRandomSpawn = true;
	bMustSpawnBackPpack = false;
	bMustSpawnHelmet = false;
	bMustSpawnVest = false;

	//if this true, brandomspawn must be false;
	bHasSpawnItems = false;
	bHasSpawnEquipments = false;

	//DataTable'/Game/GameData/RifleDataTable.RifleDataTable'
	if(bRandomSpawn)
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> RifleDataTableObj(TEXT("/Game/GameData/Equip_RifleDataTable.Equip_RifleDataTable"));
		static ConstructorHelpers::FObjectFinder<UDataTable> HelmetDataTableObj(TEXT("/Game/GameData/Equip_HelmetDataTable.Equip_HelmetDataTable"));
		static ConstructorHelpers::FObjectFinder<UDataTable> BackPackDataTableObj(TEXT("/Game/GameData/Equip_BackPackDataTable.Equip_BackPackDataTable"));
		static ConstructorHelpers::FObjectFinder<UDataTable> VestDataTableObj(TEXT("/Game/GameData/Equip_VestDataTable.Equip_VestDataTable"));

		if (RifleDataTableObj.Succeeded() && HelmetDataTableObj.Succeeded() && BackPackDataTableObj.Succeeded() && VestDataTableObj.Succeeded())
		{
			RifleDT = RifleDataTableObj.Object;
			HelmetDT = HelmetDataTableObj.Object;
			VestDT = VestDataTableObj.Object;
			BackPackDT = BackPackDataTableObj.Object;
		}
	}

}


void USpawnItemEquipComponent::BeginPlay()
{
	Super::BeginPlay();


	if (bRandomSpawn)
	{
		SpawnRandomItems();
	}
	else
	{
		if (bHasSpawnItems)
		{
			SpawnItems();
		}

		if (bHasSpawnEquipments)
		{
			SpawnEquipments();
		}
	}
}


void USpawnItemEquipComponent::SpawnItems()
{
	ABaseCharacter* OwnerBChar = Cast<ABaseCharacter>(GetOwner());
	if(OwnerBChar == nullptr) return;
	if(SpawnItemList.Num() == 0 ) return;

	for (auto ItemType : SpawnItemList)
	{		
		OwnerBChar->BaseInventoryComp->TryAddItem(OwnerBChar->PocketStorage, ItemType);		
	}
}

void USpawnItemEquipComponent::SpawnEquipments()
{
	ABaseCharacter* OwnerBChar = Cast<ABaseCharacter>(GetOwner());
	if (OwnerBChar == nullptr) return;

	for (auto& AddEquipment : SpawnEquipmentList)
	{
		AEquipment* WantToEquip = GetWorld()->SpawnActor<AEquipment>(AddEquipment);
		if (WantToEquip)
		{
			WantToEquip->SKMesh->SetHiddenInGame(false);
			WantToEquip->SKMesh->SetSimulatePhysics(false);
			WantToEquip->SKMesh->SetEnableGravity(false);

			WantToEquip->Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
			WantToEquip->Mesh->SetSimulatePhysics(false);
			WantToEquip->Mesh->SetEnableGravity(false);
			WantToEquip->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WantToEquip->Equip(OwnerBChar);
		}
	}
}


void USpawnItemEquipComponent::SpawnRandomItems()
{
	ABaseCharacter* OwnerBChar = Cast<ABaseCharacter>(GetOwner());
	if (OwnerBChar == nullptr) return;

	int32 RifleRowind = RifleDT->GetRowMap().Num();
	int32 HelmetRowind = HelmetDT->GetRowMap().Num();
	int32 VestRowind = VestDT->GetRowMap().Num();
	int32 BackPackRowind = BackPackDT->GetRowMap().Num();

	
	for (EEquipmentType EquipTp : TEnumRange<EEquipmentType>())
	{
		UCustomPDA* EquipPDA = nullptr;
		int32 RandIndex = 0;
		switch (EquipTp)
		{
		case EEquipmentType::EET_Rifle:
		{
			RandIndex = FMath::RandRange(1, RifleRowind);
			FRifleTable* RifleDTRow = RifleDT->FindRow<FRifleTable>(*FString::FromInt(RandIndex), RifleDT->GetName());
			if (RifleDTRow == nullptr) break;
			EquipPDA = RifleDTRow->WeaponDataAsset;
		}
		break;
		case EEquipmentType::EET_Helmet:
		{
			RandIndex = FMath::RandRange(1, HelmetRowind);

			RandIndex = SpawnORNot(bMustSpawnHelmet, RandIndex);
			FHelmetTable* HelmetDTRow = HelmetDT->FindRow<FHelmetTable>(*FString::FromInt(RandIndex), HelmetDT->GetName());
			if(HelmetDTRow == nullptr) break;
			EquipPDA = HelmetDTRow->HelmetDataAsset;
		}
		break;
		case EEquipmentType::EET_Vest:
		{
			RandIndex = FMath::RandRange(1, VestRowind);
			RandIndex = SpawnORNot(bMustSpawnVest, RandIndex);
			FVestTable* VestDTRow = VestDT->FindRow<FVestTable>(*FString::FromInt(RandIndex), VestDT->GetName());
			if (VestDTRow == nullptr) break;
			EquipPDA = VestDTRow->VestDataAsset;
		}
		break;
		case EEquipmentType::EET_Backpack:
		{
			RandIndex = FMath::RandRange(1, BackPackRowind);
			RandIndex = SpawnORNot(bMustSpawnBackPpack, RandIndex);
			FBackPackTable* BackPackDTRow = BackPackDT->FindRow<FBackPackTable>(*FString::FromInt(RandIndex), BackPackDT->GetName());
			if (BackPackDTRow == nullptr) break;
			EquipPDA = BackPackDTRow->BackPackDataAsset;
		}
		break;
		}


		if (EquipPDA == nullptr) continue;
		
		TryEquip(EquipPDA, OwnerBChar);
	}
}


int32 USpawnItemEquipComponent::SpawnORNot(bool bIsMustSpawn, int32 returnIndex)
{
	//bIsMustSpawn이 true면 바로 리턴하고 끝내고
	//아니면 그냥 랜덤을 돌려서 나온 값의 평균값 이상이면 Spawn을 못하도록 0을 리턴하고,
	// 평균값 아래면 spawn을 하도록 한다.
	if (bIsMustSpawn == false)
	{
		float SRandd = FMath::FRandRange(0.1f,RAND_MAX);
		float Rand = FMath::FRandRange(0.01f, SRandd);

		float MiddleRand = (0.01f + SRandd) / 2.f;

		if (Rand >= MiddleRand)
		{
			return 0;
		}
		else
		{
			return returnIndex;
		}
	}
	return returnIndex;
}

void USpawnItemEquipComponent::TryEquip(UCustomPDA* CPDA, ABaseCharacter* Char)
{
	if(CPDA == nullptr || Char == nullptr) return;

	AEquipment* Equipment = nullptr;
	Equipment = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), CPDA);
	if (Equipment == nullptr) return;

	Equipment->SKMesh->SetHiddenInGame(false);
	Equipment->SKMesh->SetSimulatePhysics(false);
	Equipment->SKMesh->SetEnableGravity(false);

	Equipment->Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
	Equipment->Mesh->SetSimulatePhysics(false);
	Equipment->Mesh->SetEnableGravity(false);
	Equipment->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Equipment->Equip(Char);
}