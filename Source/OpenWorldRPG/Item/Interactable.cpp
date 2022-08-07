// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
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

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = Mesh;
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
		SetMesh(ItemSetting.DataAsset);// , MeshComponent);
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

void AInteractable::SetMesh(UCustomPDA* PDA)//, UMeshComponent*& MeshComp)
{
	if (PDA->SKMesh)
	{
		//auto SKMeshComp = NewObject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComp"));
		//if (SKMeshComp)
		{
			SKMesh->SetSkeletalMesh(PDA->SKMesh);

			SKMesh->SetHiddenInGame(true);
			SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Overlap);

			if (!PDA->ReSizeScale.IsZero())
			{
				SKMesh->SetWorldScale3D(PDA->ReSizeScale);
			}
			//MeshComp = SKMeshComp;
		}
	}
	
	if (PDA->Mesh)
	{
		//auto SMeshComp = NewObject<UStaticMeshComponent>(this, TEXT("StaticMeshComp"));
		//if (SMeshComp)
		{
			Mesh->SetStaticMesh(PDA->Mesh);
			if (!PDA->ReSizeScale.IsZero())
			{
				Mesh->SetWorldScale3D(PDA->ReSizeScale);
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
	check(ItemSetting.DataAsset);
	UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());
	//switch (InteractType)

	//�̰� �� �س����� �𸣰ڴ�.
	/*AMainCharacter* MainChar = Cast<AMainCharacter>(Actor);
	if (MainChar && MainChar->BaseInventoryComp)
	{

	}*/

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
		//���⼭ Weapon�� �˻����� ����, �� �ٷ� Equip�� ȣ���ؾ���.
		AEquipment* Equipment = Cast<AEquipment>(this);
		if (Equipment)
		{
			Equipment->Equip(Actor); //Weapon���� cast, equip�Լ� ȣ��
		}
		break;
	}
	case EInteractType::EIT_LootBox:
	{
		AContainer* Box = Cast<AContainer>(this);
		if (Box)
		{
			UE_LOG(LogTemp, Warning, TEXT("Call Open Box"));
			Box->OpenContainer(Actor);

		}
	}
	default:
		break;
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