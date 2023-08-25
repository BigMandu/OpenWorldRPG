// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Gizmo/TargetBindVolume.h"
#include "OpenWorldRPG/Gizmo/ShootingTarget.h"
#include "Components/BillboardComponent.h"

// Sets default values
ATargetBindVolume::ATargetBindVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TargetPopupVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("billboard"));

	SetRootComponent(TargetPopupVolume);
	billboard->SetupAttachment(GetRootComponent());

}

void ATargetBindVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TargetPopupVolume->OnComponentBeginOverlap.AddDynamic(this, &ATargetBindVolume::CallPopUp);
}


void ATargetBindVolume::CallPopUp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//한번 발생했다면 popup을 호출 하지 않는다.
	if(bIsAlreadyOccurredOverlap) return;

	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{

		bIsAlreadyOccurredOverlap = true;
		for (auto target : BindShootingTargets)
		{
			if(target->bIsPopUpStatus) continue;
			target->StartPopUp();
		}
	}
}
// Called when the game starts or when spawned
void ATargetBindVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

