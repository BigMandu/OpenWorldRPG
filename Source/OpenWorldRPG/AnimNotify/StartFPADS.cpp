// Fill out your copyright notice in the Description page of Project Settings.


#include "StartFPADS.h"
#include "OpenWorldRPG/MainCharacter.h"

void UStartFPADS::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("UStartFPADS::Notify called"));
	AMainCharacter* Player = Cast<AMainCharacter>(MeshComp->GetOwner());
	if(Player == nullptr) return;

	Player->FPSADS();
}
