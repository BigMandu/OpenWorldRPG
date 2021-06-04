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

/*
SkeletalMesh�� Collision response�� ignore�Ǿ��ִ��� Ȯ���ؾ��Ѵ�.
			��, Camera�� Collision�� ignore�ϵ��� �����ҰŴ�.
			(Weapon�� Character�� Camera���̿� ������ Camera�� Character�� Ȯ������ �ʵ��� �ϱ� �����̴�.)

		//�̷��� �ϸ� Camera�� Ȯ����� �ʴ´�. �����ϰԵ�.
SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
//��, Pawn�� ���õ� �� �ֵ��� �Ѵ�.
SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//�̷��� �ϸ� Weapon�� ���� Camera�� pawn�� Camera�� �����ϰ� �ȴ�.

* �׸��� Simulating physics�� �ϰ��ִٸ�, Player�� Attach�� �ؾ� �ϱ� ������ Simulating physics�� ������Ѵ�.

SkeletalMesh->SetSimulatePhysics(false);
//�̷��� �ؼ�, Collision�� Physics������ �ذ��ߴ�. 

//���������� Attach�� ��������. SkeletalMeshSocket.h�� include�������.
const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
* ���� ���� ����� �����̶�� �� RightHandSocket������ Character skeleton�� �ִ� RightHandSocket�� ���� ������ �ɰŴ�.
	�׷��� �� ����� ��ȿ���� �ѹ��� Ȯ�� �ؾ��Ѵ�.



*/