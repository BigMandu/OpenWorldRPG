// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/Item/GrenadePDA.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"

ABaseGrenade::ABaseGrenade()
{
	//ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovComp"));
	//ProjectileMovementComp->ProjectileGravityScale = 1.1f;
	//ProjectileMovementComp->InitialSpeed = 0.f;
	//
	//ProjectileMovementComp->bShouldBounce = true;
	//ProjectileMovementComp->Bounciness = 0.5f;

	Mesh->SetSimulatePhysics(true);
}


void ABaseGrenade::OnConstruction(const FTransform& Transform)
{	
	Super::OnConstruction(Transform);
	if (Mesh)
	{
		//FVector Sizebox = this->GetComponentsBoundingBox().GetSize();
		//CapsuleComp->SetCapsuleSize(Sizebox.Y, Sizebox.Z);
		CapsuleComp->SetCapsuleSize(5.f, 5.f);
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Mesh->SetSimulatePhysics(true);
		//선형제동, 선형이동에 가해지는 'Drag(제동력)'이다.
		Mesh->SetLinearDamping(0.1f);//100.f);

		//회전제동, 회전 이동을 줄이기 위해 더해진'Drag(제동력)'이다.
		Mesh->SetAngularDamping(0.f);//100.f);
	
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	
	}
	/*if (SKMesh)
	{
		SKMesh->SetSimulatePhysics(false);
	}*/
}

//QuickSlotWidget -> NewItemObject::UseItem에서 호출함.
void ABaseGrenade::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj)
{

	if (Actor->EquippedWeapon)
	{
		Actor->EquippedWeapon->GunAttachToSubSocket(Actor);

		//재장착을 위해 임시저장한다.
		BeforeEquipppedWeapon = Actor->EquippedWeapon;
		Actor->EquippedWeapon = nullptr;
	}
	else if (Actor->EquippedGrenade.IsValid())
	{
		if (Actor->EquippedGrenade == this)
		{
			this->Destroy();
			return;
		}
		else if (Actor->EquippedGrenade != this)
		{
			Actor->EquippedGrenade->Destroy();
		}
	}	
	
	if (Obj)
	{
		GObj = Obj;
	}

	//손에 든 순간부터 Interaction이 되지 않도록 한다.
	bCanNotInteractable = true;

	Mesh->SetSimulatePhysics(false);
	Mesh->SetLinearDamping(0.01f);
	Mesh->SetAngularDamping(0.f);

	//WeaponClass의 GunAttachToMesh func를 호출해 WeaponGripSocket에 부착한다.
	GunAttachToMesh(Actor);
	SetOwningPlayer(Actor);

	
	

	//MainChar의 LMBDown, Up을 위해 EquippedGrenade변수를 선언, 대입한다.
	Actor->EquippedGrenade = this;
}

//While LMB down
void ABaseGrenade::ReadyToThrow()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	UE_LOG(LogTemp,Warning,TEXT("ABaseGrenade::ReadyTo Throw"));

	
	//GetWorldTimerManager().SetTimer(UpdateAimTimerHandle,this, &AWeapon::, GetWorld()->GetDeltaSeconds(), true);

	GetWorldTimerManager().SetTimer(EffectTimerHandle, this, &ABaseGrenade::BeginEffect, GPDA->EffectDelayTime, false);

}


//called when LMB Up
//Detach from hand
void ABaseGrenade::ThrowGrenade(ABaseCharacter* Actor)
{
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: Throw Grenade"));
	
	//Item을 Inventory에서 지워주고
	if(GObj.IsValid())
	{
		Actor->BaseInventoryComp->RemoveItemCount(GObj.Get(), 1);
		GObj = nullptr;
	}
	//Detach를 한다.
	FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,false);
	this->DetachFromActor(Rules);

	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
	}

	Mesh->SetLinearDamping(2.f);
	Mesh->SetAngularDamping(1.f);
	

	/*여기서 던져야하는데
	* AddImpulse || SetPhysics LinearVelocity
	* 
	* predict projectile path by tracechannel
	*/
	
	FTransform AimPos = GetAimPosition();
	FVector StartTrace = GetTraceStartLocation(AimPos.Rotator().Vector());
	FVector EndVec = StartTrace + AimPos.GetRotation().Vector()*30.f;

	DrawDebugSphere(GetWorld(), StartTrace, 5.f, 6, FColor::Green, false, 3.f, 0, 2.f);
	DrawDebugSphere(GetWorld(), EndVec, 5.f, 6, FColor::Blue, false, 3.f, 0, 2.f);

	
	
	
	FVector CalcVector = (GetActorLocation() - EndVec) * 10.f; //Actor->GetActorForwardVector() * 1000.f;	
	DrawDebugSphere(GetWorld(), CalcVector, 5.f, 6, FColor::Red, false, 3.f, 0, 2.f);

	Mesh->SetPhysicsLinearVelocity(CalcVector);

	//ProjectileMovementComp->Velocity = GetAimPosition().GetRotation().Vector() * 500.f;//ProjectileMovementComp->InitialSpeed;
	//DrawDebugLine(GetWorld(), GetAimPosition().GetRotation().Vector(), ProjectileMovementComp->Velocity, FColor::Green, false, 3.f, 0, 2.f);

	
	
	//던진뒤 PrimaryWeapon을 장착시킨다.
	Actor->EquippedGrenade = nullptr;
	EquipBeforeWeapon(Actor);
}



void ABaseGrenade::BeginEffect()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;

	switch (GPDA->GrenadeType)
	{
	case EGrenadeType::EGT_Fragment:
		FragmentEffect();
	break;
	case EGrenadeType::EGT_Smoke:
		SmokeEffect();
	break;
	}

	OwningPlayer = nullptr;
	//Destroy();
}



void ABaseGrenade::EquipBeforeWeapon(ABaseCharacter* Actor)
{
	if (BeforeEquipppedWeapon.IsValid())
	{
		if (Actor->PrimaryWeapon == BeforeEquipppedWeapon)
		{
			Actor->ChangePrimaryWeapon();
		}
		else if (Actor->SubWeapon == BeforeEquipppedWeapon)
		{
			Actor->ChangeSubWeapon();
		}
		else if (Actor->PistolWeapon == BeforeEquipppedWeapon)
		{
			Actor->ChangePistolWeapon();
		}
	}
}


void ABaseGrenade::FragmentEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: FragmentEffect"));
}

void ABaseGrenade::SmokeEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: SmokeEffect"));
}