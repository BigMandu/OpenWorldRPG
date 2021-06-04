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

			Mesh->SetHiddenInGame(true); //Static Mesh를 지워준다.
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
SkeletalMesh의 Collision response가 ignore되어있는지 확인해야한다.
			또, Camera의 Collision을 ignore하도록 설정할거다.
			(Weapon이 Character와 Camera사이에 있으면 Camera가 Character를 확대하지 않도록 하기 위함이다.)

		//이렇게 하면 Camera가 확대되지 않는다. 무시하게됨.
SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
//또, Pawn이 무시될 수 있도록 한다.
SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//이렇게 하면 Weapon에 대한 Camera와 pawn의 Camera를 무시하게 된다.

* 그리고 Simulating physics를 하고있다면, Player와 Attach를 해야 하기 때문에 Simulating physics를 멈춰야한다.

SkeletalMesh->SetSimulatePhysics(false);
//이렇게 해서, Collision과 Physics문제를 해결했다. 

//본격적으로 Attach를 시작하자. SkeletalMeshSocket.h를 include해줘야함.
const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
* 만약 위의 결과가 성공이라면 이 RightHandSocket변수는 Character skeleton에 있는 RightHandSocket에 대한 참조가 될거다.
	그래서 이 결과가 유효한지 한번더 확인 해야한다.



*/