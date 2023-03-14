﻿// Fill out your copyright notice in the Description page of Project Settings.


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
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(Mesh);

	if(SKMesh)
	{
		SKMesh->SetupAttachment(GetRootComponent());
	}
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
	else if (ItemSetting.DataAsset->Mesh)
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
	
	/* 이 Interact가 Itemclass일 경우, Item -> AddToInventory이런 함수를 선언, hide mesh, disable collision작업을 해준다.
	*  그 외에 다른 class, 게임내 문 상호작용인 경우 위치를 수정해주거나 ㅇㅇ 이런식으로 분할.
	* 
	*/
	if (ItemSetting.DataAsset == nullptr) return;
	if(bCanNotInteractable) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());


	switch (ItemSetting.DataAsset->InteractType)
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
		//Weapon으로 cast, equip함수 호출
		AEquipment* Equipment = Cast<AEquipment>(this);
		if (Equipment)
		{
			if(Equipment->Equip(Actor, ERifleSlot::ERS_MAX) == false)
			{	
				//장착에 실패했으면 Pickup을 진행한다.
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
	if(bCanNotInteractable) return;
	Mesh->SetRenderCustomDepth(true);
}


void AInteractable::UnsetOutline()//_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interactable::disable outline"));
	if(bCanNotInteractable) return;
	Mesh->SetRenderCustomDepth(false);
}
