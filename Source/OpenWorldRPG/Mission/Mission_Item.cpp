// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission_Item.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "Components/SphereComponent.h"

AMission_Item::AMission_Item(): Super()
{
	//Tick함수 복붙후 아래 코드 주석해제하자 
	//PrimaryActorTick.bCanEverTick = true;

	NotifySphere = CreateDefaultSubobject<USphereComponent>(TEXT("NotifySphere"));
	NotifySphere->SetupAttachment(GetRootComponent());
}

void AMission_Item::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	bCanNotInteractable = false;
	NotifySphere->OnComponentBeginOverlap.AddDynamic(this, &AMission_Item::OverlapMissionObj);
	NotifySphere->OnComponentEndOverlap.AddDynamic(this, &AMission_Item::EndOverlapMissionObj);

	MissionUniqueID_Interface = MissionUniqueID;
}

void AMission_Item::Interaction(AActor* Actor)
{
	Super::Interaction(Actor);

	if (UWorld* World = GetWorld())
	{
		if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(World)))
		{
			Gmode->EvaluateMissionStatus(this);
		}
	}

}


void AMission_Item::OverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		SetOutline();
	}
}

void AMission_Item::EndOverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		UnsetOutline();
	}
}

void AMission_Item::ShowNotifyIcon()
{
	
}
void AMission_Item::HideNotifyIcon()
{
	
}