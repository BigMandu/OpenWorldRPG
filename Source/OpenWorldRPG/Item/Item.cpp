// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ItemSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	ItemSMesh->SetSimulatePhysics(true);
	ItemSMesh->SetupAttachment(GetRootComponent());
	
	UseActionText = FText::FromString("eat");
	ItemName = FText::FromString("Item");

	InteractText = FString("Press E to pick up");
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::Interact()//_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Item::Interact"));
}

void AItem::Pickup()
{
	UE_LOG(LogTemp, Warning, TEXT("Item::Pickup"));
}

void AItem::Use(class ACharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Item::Use"));
}