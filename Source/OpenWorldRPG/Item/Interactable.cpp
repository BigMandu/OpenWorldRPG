// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Item.h"
#include "Weapon.h"
#include "OpenWorldRPG/MainCharacter.h"
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
	case EInteractType::EIT_Weapon:
	{
		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
		if (Main)
		{
			//���⼭ Weapon�� �˻����� ����, �� �ٷ� Equip�� ȣ���ؾ���.
			AWeapon* Weapon = Cast<AWeapon>(this);
			if (Weapon)
			{
				Weapon->Equip(Actor); //Weapon���� cast, equip�Լ� ȣ��
			}
		}

		break;
	}
	default:
		break;
	}
}

void AInteractable::SetOutline_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::enable outline"));
	
	Mesh->SetRenderCustomDepth(true);
}


void AInteractable::UnsetOutline_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::disable outline"));
	Mesh->SetRenderCustomDepth(false);
}
