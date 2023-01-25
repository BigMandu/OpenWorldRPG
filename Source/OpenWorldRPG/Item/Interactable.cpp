// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/MainCharacter.h"

#include "Item.h"
#include "Equipment.h"
#include "Container.h"
#include "Perception/AISense_Sight.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//DummyComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyComponent"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	//SetRootComponent(DummyComp);
	SetRootComponent(Mesh);
	//Mesh->SetupAttachment(GetRootComponent());
	SKMesh->SetupAttachment(GetRootComponent());

	//InteractType = EInteractType::EIT_Item;
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();	
}

void AInteractable::OnConstruction(const FTransform& Transform)
{
	if (ItemSetting.DataAsset)
	{
		SetMesh();// ItemSetting.DataAsset);// , MeshComponent);
	}
	Super::OnConstruction(Transform);
}

void AInteractable::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	/*switch (InteractType)
	{
	case EInteractType::EIT_Equipment:
	case EInteractType::EIT_Item:
		bIsPreOccupied = false;
		break;
	}*/

}

void AInteractable::SetMesh()// UCustomPDA* PDA)//, UMeshComponent*& MeshComp)
{
	if (ItemSetting.DataAsset == nullptr) return;

	if (ItemSetting.DataAsset->SKMesh)
	{
		//auto SKMeshComp = NewObject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComp"));
		//if (SKMeshComp)
		{
			
			SKMesh->SetSkeletalMesh(ItemSetting.DataAsset->SKMesh);

			SKMesh->SetHiddenInGame(true);
			//SKMesh->SetHiddenInGame(false);  //for test

			SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Overlap);
		
			if (!ItemSetting.DataAsset->ReSizeScale.IsZero())
			{
				//SKMesh->SetWorldScale3D(ItemSetting.DataAsset->ReSizeScale);
			}
			//MeshComp = SKMeshComp;
		}
	}
	
	if (ItemSetting.DataAsset->Mesh)
	{
		//auto SMeshComp = NewObject<UStaticMeshComponent>(this, TEXT("StaticMeshComp"));
		//if (SMeshComp)
		{
			Mesh->SetStaticMesh(ItemSetting.DataAsset->Mesh);
			if (!ItemSetting.DataAsset->ReSizeScale.IsZero())
			{
				Mesh->SetWorldScale3D(ItemSetting.DataAsset->ReSizeScale);
			}
		}
	}

	//MeshComp->SetupAttachment(GetRootComponent());
	//MeshComp->RegisterComponent();
	//RootComponent = MeshComp;

}

void AInteractable::Interaction(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactive_Interface , Interaction function"));
	
	/* �� Interact�� Itemclass�� ���, Item -> AddToInventory�̷� �Լ��� ����, hide mesh, disable collision�۾��� ���ش�.
	*  �� �ܿ� �ٸ� class, ���ӳ� �� ��ȣ�ۿ��� ��� ��ġ�� �������ְų� ���� �̷������� ����.
	* 
	*/
	if (ItemSetting.DataAsset == nullptr) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());


	switch (ItemSetting.DataAsset->InteractType)
	{
	case EInteractType::EIT_Item:
	{
		AItem* Item = Cast<AItem>(this); //ItemŬ������ ĳ����
		if (Item)
		{
			Item->Pickup(Actor); //Pickup����.
		}
		break;
	}
	case EInteractType::EIT_Equipment:
	{
		//Weapon���� cast, equip�Լ� ȣ��
		AEquipment* Equipment = Cast<AEquipment>(this);
		if (Equipment)
		{
			if(Equipment->Equip(Actor, ERifleSlot::ERS_MAX) == false)
			{	
				//������ ���������� Pickup�� �����Ѵ�.
				Equipment->Pickup(Actor);
			}
		}
		break;
	}
	}
}

void AInteractable::SetOutline()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::enable outline"));
	
	//Mesh->SetRenderCustomDepth(true);
	Mesh->SetRenderCustomDepth(true);
}


void AInteractable::UnsetOutline()//_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::disable outline"));
	Mesh->SetRenderCustomDepth(false);
}