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
	
	/* 이 Interact가 Itemclass일 경우, Item -> AddToInventory이런 함수를 선언, hide mesh, disable collision작업을 해준다.
	*  그 외에 다른 class, 게임내 문 상호작용인 경우 위치를 수정해주거나 ㅇㅇ 이런식으로 분할.
	* 
	*/
	
	UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());
	switch (InteractType)
	{
	case EInteractType::EIT_Item:
	{
		AItem* Item = Cast<AItem>(this); //Item클래스로 캐스팅
		if (Item)
		{
			Item->Pickup(Actor); //Pickup진행.
		}
		break;
	}
	case EInteractType::EIT_Equipment:
	{
		//여기서 Weapon을 검사하지 말고, 걍 바로 Equip을 호출해야함.
		AEquipment* Equipment = Cast<AEquipment>(this);
		if (Equipment)
		{
			Equipment->StepEquip(Actor); //Weapon으로 cast, equip함수 호출
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