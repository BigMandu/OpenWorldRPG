// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	RootComponent = OverlapCollision; //오버랩 충돌체를 루트컴포넌트로 지정해야 충돌 이벤트가 발생한다.

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CollisionBeginOverlap);
	OverlapCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CollisionEndOverlap);

	
	OverlapCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	
	OverlapCollision->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	OverlapCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::CollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Overlap Begin"));
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			Player->SetOverlappingActor(this);
		}
	}
}

void AWeapon::CollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Overlap End"));
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			Player->SetOverlappingActor(nullptr);
		}
	}
}

void AWeapon::Equip(ACharacter* Char)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	/*FAttachmentTransformRules ATR;
	ATR.ScaleRule = EAttachmentRule::KeepRelative;
	ATR.LocationRule = EAttachmentRule::KeepRelative;*/
	if (Main)
	{
		const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("WeaponGrip");
		if (Socket)
		{
			//AttachToActor(this, ATR, Socket->GetFName());
			Socket->AttachActor(this, Main->GetMesh());
		}
		
	}
}