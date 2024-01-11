// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/WorldControlVolume/AreaControlVolume.h"
#include "OpenWorldRPG/WorldControlVolume/SpawnVolume.h"
#include "OpenWorldRPG/MainCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"


// Sets default values
AAreaControlVolume::AAreaControlVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaControlSphere"));
	SetRootComponent(CollisionSphere);
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void AAreaControlVolume::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAreaControlVolume::OnBeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AAreaControlVolume::OnEndOverlap);

	TArray<AActor*> OverlappingList;
	CollisionSphere->GetOverlappingActors(OverlappingList);
	bool bIsAlreadyOverlapping = false;

	for (auto OverlapActor : OverlappingList)
	{
		if (OverlapActor)
		{
			if (AMainCharacter* Player = Cast<AMainCharacter>(OverlapActor))
			{
				bIsAlreadyOverlapping = true;
				break;
			}
		}
	}

	if (bIsAlreadyOverlapping)
	{
		NeedToShowActors();
	}
	else
	{
		NeedToHideActors();
	}

}



void AAreaControlVolume::NeedToShowActors()
{
	for (ASpawnVolume* Volume : SpawnVolumeList)
	{
		if (Volume && Volume->bNeedToHideWhenFarAway)
		{
			Volume->SetSpawnedActorVisibility(true);
		}		
	}
}

void AAreaControlVolume::NeedToHideActors()
{
	for (ASpawnVolume* Volume : SpawnVolumeList)
	{
		if (Volume && Volume->bNeedToHideWhenFarAway)
		{
			Volume->SetSpawnedActorVisibility(false);
		}
	}
}

void AAreaControlVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		NeedToShowActors();
	}
}
void AAreaControlVolume::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		NeedToHideActors();
	}
}