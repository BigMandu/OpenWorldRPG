// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = WeaponMesh;
	Mesh->SetupAttachment(GetRootComponent());

	WeaponMesh->SetHiddenInGame(true);
}

void AWeapon::Equip(AActor* Char)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	if (Main)
	{
		const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("WeaponGrip");
		if (Socket)
		{
			Mesh->SetHiddenInGame(true); //Static Mesh�� �����ش�.
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			WeaponMesh->SetHiddenInGame(false);

			Socket->AttachActor(this, Main->GetMesh());
			Main->EquippedWeapon = this;

			/*if (EquippedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
			}*/
		}
	}
}