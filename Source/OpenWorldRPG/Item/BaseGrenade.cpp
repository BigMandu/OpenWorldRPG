// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/Item/GrenadePDA.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainAnimInstance.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"

#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"

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
	bReadyToThrow = false;
	bThrow = false;

	Ca_ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CascadeParticleComp"));
	Ca_ParticleComp->SetupAttachment(GetRootComponent());
	
	Ni_ParticleComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraParticleComp"));
	Ni_ParticleComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());
	//AudioComp->SetAutoActivate(false);
	//AudioComp->Deactivate();

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
		//��������, �����̵��� �������� 'Drag(������)'�̴�.
		Mesh->SetLinearDamping(0.1f);//100.f);

		//ȸ������, ȸ�� �̵��� ���̱� ���� ������'Drag(������)'�̴�.
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

//QuickSlotWidget -> NewItemObject::UseItem���� ȣ����.
void ABaseGrenade::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj)
{
	Super::AttachToHand(Actor,Obj);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetLinearDamping(0.01f);
	Mesh->SetAngularDamping(0.f);

	//WeaponClass�� GunAttachToMesh func�� ȣ���� WeaponGripSocket�� �����Ѵ�.
	GunAttachToMesh(Actor);
	SetOwningPlayer(Actor);
}

//While LMB down
void ABaseGrenade::ReadyToThrow()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	UE_LOG(LogTemp,Warning,TEXT("ABaseGrenade::ReadyTo Throw"));
	
	bReadyToThrow = true;
	if (this->OwningPlayer->TPAnimInstance)
	{
		this->OwningPlayer->TPAnimInstance->WeaponTypeNumber = 3;
	}

	GetWorldTimerManager().SetTimer(EffectTriggerTimerHandle, this, &ABaseGrenade::BeginEffect, GPDA->EffectDelayTime, false);

}


//called when LMB Up
//Detach from hand
void ABaseGrenade::ThrowGrenade(ABaseCharacter* Actor)
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: Throw Grenade, playAnim"));
	Actor->PlayAnimMontage(GPDA->ThrowingAnimMontage);
	
	
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	
	//������ �� ���Ŀ� Interaction�� �Ұ����ϵ��� �Ѵ�.
	bCanNotInteractable = true;

	//detach�ϱ� ���� bool���� update	
	bThrow = true;

	//Item�� Inventory���� �����ְ�
	RemoveCountAtIventory(Actor,1);
}

void ABaseGrenade::DetectThrow(ABaseCharacter* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: DetectThrow, detect AnimNotify"));
	

	////Detach�� �Ѵ�.
	DetachFromHand(Actor,false);


	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
	}

	Mesh->SetLinearDamping(2.f);
	Mesh->SetAngularDamping(1.f);


	/*���⼭ �������ϴµ�
	* AddImpulse || SetPhysics LinearVelocity
	*
	* predict projectile path by tracechannel
	*/

	FTransform AimPos = GetAimPosition();
	FVector StartTrace = GetTraceStartLocation(AimPos.Rotator().Vector());
	FVector EndVec = StartTrace + AimPos.GetRotation().Vector() * 100.f;
	

	DrawDebugSphere(GetWorld(), StartTrace, 5.f, 6, FColor::Green, false, 3.f, 0, 2.f);
	DrawDebugSphere(GetWorld(), EndVec, 5.f, 6, FColor::Blue, false, 3.f, 0, 2.f);
	

	//FVector CalcVector = (EndVec - GetActorLocation()) * 15.f; //Actor->GetActorForwardVector() * 1000.f;	
	FVector CalcVector = (EndVec - StartTrace) * 50.f; //Actor->GetActorForwardVector() * 1000.f;	
	DrawDebugSphere(GetWorld(), CalcVector, 5.f, 6, FColor::Red, false, 3.f, 0, 2.f);

	Mesh->SetPhysicsLinearVelocity(CalcVector);

	//ProjectileMovementComp->Velocity = GetAimPosition().GetRotation().Vector() * 500.f;//ProjectileMovementComp->InitialSpeed;
	//DrawDebugLine(GetWorld(), GetAimPosition().GetRotation().Vector(), ProjectileMovementComp->Velocity, FColor::Green, false, 3.f, 0, 2.f);



	////������ PrimaryWeapon�� ������Ų��.
	//Actor->HoldingItem = nullptr;
	//EquipBeforeWeapon(Actor);
}

//timer ready to throw
void ABaseGrenade::BeginEffect()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	
	//Cascade(���̾ư��� ���� ����Ʈ) Effect�� ���� üũ�ϰ�
	if (GPDA->Ca_GrenadeEffect)
	{
		Ca_ParticleComp->SetAutoActivate(true);
		Ca_ParticleComp->SetTemplate(GPDA->Ca_GrenadeEffect);
		Ca_ParticleComp->Activate();
	}
	//�������� ���̾ư��� Effect�� üũ�Ѵ�.
	else if (GPDA->Ni_GrenadeEffect)
	{
		Ni_ParticleComp->SetAutoActivate(true);
		Ni_ParticleComp->SetAsset(GPDA->Ni_GrenadeEffect);
		Ni_ParticleComp->Activate();

	}

	
	AudioComp->SetAutoActivate(true);	
	Ca_ParticleComp->SetWorldRotation(FRotator(0.f, 0.f, 1.f));
	AudioComp->SetSound(GPDA->GrenadeEffectSound);
	
	
	AudioComp->Activate();
	

	switch (GPDA->GrenadeType)
	{
	case EGrenadeType::EGT_Fragment:
		FragmentEffect(GPDA);
	break;
	case EGrenadeType::EGT_Smoke:
		SmokeEffect(GPDA);
	break;
	}	

	//������ ���� ���¿��� ������ ���
	if (bThrow == false)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(GetOwningPlayer());
		DetachFromHand(BChar, false);
		
		/*
		BChar->HoldingItem = nullptr;
		EquipBeforeWeapon(BChar);*/
	}

	
}

void ABaseGrenade::FragmentEffect(UGrenadePDA* gPDA)
{
	if(gPDA == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: FragmentEffect"));
	
	FRadialDamageEvent RadialDmgEvt;
	const TArray<AActor*> IgnoreActarr;
	TSubclassOf<UDamageType> const DmgType = TSubclassOf<UDamageType>(UDamageType::StaticClass());			
	UGameplayStatics::ApplyRadialDamage(GetWorld(), gPDA->Damage, GetActorLocation(), gPDA->EffectRadius,DmgType,IgnoreActarr,this,GetOwningPlayer()->GetInstigatorController(),true);
	

	//�ȸ���
	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),GPDA->Damage, GPDA->Damage - GPDA->Damage*0.1f,GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius*0.2f,GPDA->EffectRadius, 1.f,
	//	DmgType, IgnoreActarr, OwningPlayer,GetInstigatorController());
	
	DrawDebugSphere(GetWorld(),GetActorLocation(), gPDA->EffectRadius,12,FColor::Cyan,false,5.f,0,2.f);
	//DrawDebugSphere(GetWorld(),GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius * 0.2f,12,FColor::Red,false,5.f,0,2.f);

	
	TArray<FHitResult> Hits;
	FVector Location = GetActorLocation();
	FCollisionShape Shape = FCollisionShape::MakeSphere(gPDA->EffectRadius);
	//ECollisionChannel Collisionch = 

	/* RadialForce�� TakeDamage�� ��ħ. */
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, Location , FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),Shape);
	}

	for (int32 ind = 0; ind < Overlaps.Num(); ++ind)
	{
		const FOverlapResult& Overlap = Overlaps[ind];
		AActor* OverlapAct =  Overlap.GetActor();
		
		if (OverlapAct && OverlapAct != this && Overlap.Component.IsValid())
		{
			//�켱 MeshComponent�� �ִ� StaticMesh�� ���, Character�� ��� /�ΰ��� ������.
			UStaticMeshComponent* meshcomp = Cast< UStaticMeshComponent>(OverlapAct->GetRootComponent());
			ABaseCharacter* Char = Cast<ABaseCharacter>(OverlapAct);
			if (meshcomp)
			{
				meshcomp->AddRadialForce(Location, gPDA->EffectRadius, 20000.f, ERadialImpulseFalloff::RIF_Constant, true);
			}
			else if (Char)
			{
				Char->GetMesh()->AddRadialForce(Location, gPDA->EffectRadius, 20000.f, ERadialImpulseFalloff::RIF_Constant, true);
			}

			//LineTrace�� ������ �����ҵ�
			/*if (OverlapAct->CanBeDamaged())
			{
				FHitResult OutHit;
				FVector EndLocation = OverlapAct->GetActorLocation();
				if (GetWorld()->LineTraceSingleByChannel(OutHit, Location, EndLocation, ECollisionChannel::ECC_Visibility))
				{
					TSubclassOf<UDamageType> const DmgType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
					FDamageEvent DmgEvt = FDamageEvent(DmgType);
					OverlapAct->TakeDamage(gPDA->Damage, DmgEvt, GetOwningPlayer()->GetInstigatorController(), this);
				}
			}*/
		}
	}

	GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ABaseGrenade::EndEffect, 2.f, false);
	
}

void ABaseGrenade::SmokeEffect(UGrenadePDA* gPDA)
{
	if (gPDA == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: SmokeEffect"));
	
	GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ABaseGrenade::EndEffect, gPDA->SmokeDuration, false);

	
}

void ABaseGrenade::EndEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: EndEffect"));
	if (Ca_ParticleComp->IsActive())
	{
		Ca_ParticleComp->DeactivateSystem(); //Deactivate();
	}
	if (AudioComp->IsActive())
	{
		AudioComp->Stop();// Deactivate();
	}

	//GetWorldTimerManager().RemoveTimer
	
	GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);

	//������ ���� Effect���� Destroy�� ���ش�.
	OwningPlayer = nullptr;
	Destroy();
}

//void ABaseGrenade::EquipBeforeWeapon(ABaseCharacter* Actor)
//{
//	if (Actor->TPAnimInstance)
//	{
//		Actor->TPAnimInstance->WeaponTypeNumber = 0;
//	}
//
//	if (BeforeEquipppedWeapon.IsValid())
//	{
//		if (Actor->PrimaryWeapon == BeforeEquipppedWeapon)
//		{
//			Actor->ChangePrimaryWeapon();
//		}
//		else if (Actor->SubWeapon == BeforeEquipppedWeapon)
//		{
//			Actor->ChangeSubWeapon();
//		}
//		else if (Actor->PistolWeapon == BeforeEquipppedWeapon)
//		{
//			Actor->ChangePistolWeapon();
//		}
//	}
//}
