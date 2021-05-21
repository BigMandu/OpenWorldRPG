// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Components/StaticMeshComponent.h"


/*

https://www.youtube.com/watch?v=QgvY8PuZIDo&list=PLnHeglBaPYu9FFgnYww2UC1g3F7fYWkaT&index=20


*/

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}


AActor* AInteractable::Interaction(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Interactive_Interface , Interaction function"));
	if (Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor is : %s"), *GetName());
		return this;
	}
	
	return nullptr;
}
