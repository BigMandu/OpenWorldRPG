// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Item.h"
#include "Equipment.h"
#include "LootBox.h"
#include "Perception/AISense_Sight.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable Mesh"));
	RootComponent = Mesh;
	InteractType = EInteractType::EIT_Item;
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();	
}


void AInteractable::Interaction(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactive_Interface , Interaction function"));
	
	/* �� Interact�� Itemclass�� ���, Item -> AddToInventory�̷� �Լ��� ����, hide mesh, disable collision�۾��� ���ش�.
	*  �� �ܿ� �ٸ� class, ���ӳ� �� ��ȣ�ۿ��� ��� ��ġ�� �������ְų� ���� �̷������� ����.
	* 
	*/
	
	UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());
	switch (InteractType)
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
			Equipment->StepEquip(Actor); //Weapon���� cast, equip�Լ� ȣ��
		}
		break;
	}
	case EInteractType::EIT_LootBox:
	{
		ALootBox* Box = Cast<ALootBox>(this);
		if (Box)
		{
			UE_LOG(LogTemp, Warning, TEXT("Call Open Box"));
			Box->OpenBox(Actor);
		}
	}
	default:
		break;
	}
}

void AInteractable::SetOutline()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::enable outline"));
	
	Mesh->SetRenderCustomDepth(true);
}


void AInteractable::UnsetOutline()//_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::disable outline"));
	Mesh->SetRenderCustomDepth(false);
}