// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/Item/GrenadePDA.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainCharacter.h"
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

#include "Perception/AISense_Hearing.h"

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

	EffectAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("EffectAudioComp"));
	EffectAudioComp->SetupAttachment(GetRootComponent());

	BounceAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BounceAudioComp"));
	BounceAudioComp->SetupAttachment(GetRootComponent());
	
	Mesh->OnComponentHit.AddDynamic(this, &ABaseGrenade::OnCapsuleComponentHit);
	//CapsuleComp->OnComponentHit.AddDynamic(this,&ABaseGrenade::OnCapsuleComponentHit);
	

}


void ABaseGrenade::OnConstruction(const FTransform& Transform)
{	
	Super::OnConstruction(Transform);
	if (Mesh)
	{
		//FVector Sizebox = this->GetComponentsBoundingBox().GetSize();
		//CapsuleComp->SetCapsuleSize(Sizebox.Y, Sizebox.Z);
		CapsuleComp->SetCapsuleSize(2.f, 2.f);
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Mesh->SetSimulatePhysics(true);
		//선형제동, 선형이동에 가해지는 'Drag(제동력)'이다.
		Mesh->SetLinearDamping(0.2f);//100.f);

		//회전제동, 회전 이동을 줄이기 위해 더해진'Drag(제동력)'이다.
		Mesh->SetAngularDamping(0.2f);//100.f);

		
			
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);


		if ( UGrenadePDA* GPDA = Cast< UGrenadePDA>(this->ItemSetting.DataAsset) )
		{
			if ( GPDA->GrenadeBounceSound )
			{
				BounceAudioComp->SetSound(GPDA->GrenadeBounceSound);
			}
			
		}
		
	}
	/*if (SKMesh)
	{
		SKMesh->SetSimulatePhysics(false);
	}*/
}

//QuickSlotWidget -> NewItemObject::UseItem에서 호출함.
void ABaseGrenade::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj, bool bIsNeedToDestory)
{
	Super::AttachToHand(Actor,Obj, bIsNeedToDestory);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetLinearDamping(0.01f);
	Mesh->SetAngularDamping(0.f);

	//WeaponClass의 GunAttachToMesh func를 호출해 WeaponGripSocket에 부착한다.
	//GunAttachToMesh(Actor);
	SetOwningPlayer(Actor);
}

//While LMB down
void ABaseGrenade::ReadyToThrow()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	UE_LOG(LogTemp,Warning,TEXT("ABaseGrenade::ReadyTo Throw"));
	
	bReadyToThrow = true;

	
	if(OwningPlayer)
	{
		//if ( AMainCharacter* Player = Cast<AMainCharacter>(OwningPlayer) )
		//{
		//	//FPMesh도 끌수 있도록 커스텀 함수를 호출한다..
		//	//여기서 좀 헤맴.
		//	Player->StooopAnimMontage();
		//	//Player->StopAnimMontage(nullptr);
		//	Player->ChangeWeaponTypeNumber(3);
		//}
		//else
		{
			//다른 AnimMontage가 재생중이면 Stop시켜서
			//AnimBP의 Anim을 재생 할 수 있도록 한다.
			OwningPlayer->StopAnimation();
			OwningPlayer->ChangeWeaponTypeNumber(3);
		}
	}


 	GetWorldTimerManager().SetTimer(EffectTriggerTimerHandle, this, &ABaseGrenade::BeginEffect, GPDA->EffectDelayTime, false);

}


//called when LMB Up
//Detach from hand
void ABaseGrenade::ThrowGrenade(ABaseCharacter* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: Throw Grenade, playAnim"));
	// 
	//UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	//Actor->PlayAnimMontage(GPDA->ThrowingAnimMontage);


	Actor->PlayAnimation(this->ItemSetting.DataAsset->TPS_UseAnimMontage, this->ItemSetting.DataAsset->FPS_UseAnimMontage);
	
	
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	//Mesh->BodyInstance.SetCollisionProfileName("Projectile");

	/*CapsuleComp->SetGenerateOverlapEvents(true);
	CapsuleComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);*/


	
	//던지고 난 이후엔 Interaction이 불가능하도록 한다.
	bCanNotInteractable = true;

	//detach하기 전에 bool먼저 update	
	bThrow = true;

	//Item을 Inventory에서 지워주고
	RemoveCountAtIventory(Actor,1);
}

void ABaseGrenade::DetectThrow(ABaseCharacter* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: DetectThrow, detect AnimNotify"));
	
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);

	if(GPDA == nullptr ) return;
	////Detach를 한다.
	DetachFromHand(Actor, GPDA->bIsNeedToAttachHandBeforeUse);

	//Hit Event를 발생시키기 위함.
	Mesh->SetNotifyRigidBodyCollision(true);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName("BlockAllDynamic");
	
	Mesh->SetLinearDamping(2.f);
	Mesh->SetAngularDamping(1.f);


	/*여기서 던져야하는데
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



	////던진뒤 PrimaryWeapon을 장착시킨다.
	//Actor->HoldingItem = nullptr;
	//EquipBeforeWeapon(Actor);
}

//timer ready to throw
void ABaseGrenade::BeginEffect()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	
	//Cascade(나이아가라 이전 이펙트) Effect를 먼저 체크하고
	if (GPDA->Ca_GrenadeEffect)
	{
		Ca_ParticleComp->SetAutoActivate(true);
		Ca_ParticleComp->SetTemplate(GPDA->Ca_GrenadeEffect);
		Ca_ParticleComp->Activate();
	}
	//다음으로 나이아가라 Effect를 체크한다.
	else if (GPDA->Ni_GrenadeEffect)
	{
		Ni_ParticleComp->SetAutoActivate(true);
		Ni_ParticleComp->SetAsset(GPDA->Ni_GrenadeEffect);
		Ni_ParticleComp->Activate();

	}

	
	EffectAudioComp->SetAutoActivate(true);
	Ca_ParticleComp->SetWorldRotation(FRotator(0.f, 0.f, 1.f));
	EffectAudioComp->SetSound(GPDA->GrenadeEffectSound);
	
	
	EffectAudioComp->Activate();
	//AI가 인식할 수 있도록 함.
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);

	switch (GPDA->GrenadeType)
	{
	case EGrenadeType::EGT_Fragment:
		FragmentEffect(GPDA);
	break;
	case EGrenadeType::EGT_Smoke:
		SmokeEffect(GPDA);
	break;
	}	

	//던지지 않은 상태에서 터졌을 경우
	if (bThrow == false)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(GetOwningPlayer());
		DetachFromHand(BChar, true);
		RemoveCountAtIventory(BChar, 1);
		bThrow = true;
		bCanNotInteractable = true;

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
	

	//안먹음
	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),GPDA->Damage, GPDA->Damage - GPDA->Damage*0.1f,GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius*0.2f,GPDA->EffectRadius, 1.f,
	//	DmgType, IgnoreActarr, OwningPlayer,GetInstigatorController());
	
	DrawDebugSphere(GetWorld(),GetActorLocation(), gPDA->EffectRadius,12,FColor::Cyan,false,5.f,0,2.f);
	//DrawDebugSphere(GetWorld(),GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius * 0.2f,12,FColor::Red,false,5.f,0,2.f);

	
	TArray<FHitResult> Hits;
	FVector Location = GetActorLocation();
	FCollisionShape Shape = FCollisionShape::MakeSphere(gPDA->EffectRadius);
	//ECollisionChannel Collisionch = 

	/* RadialForce와 TakeDamage를 합침. */
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
			//우선 MeshComponent만 있는 StaticMesh인 경우, Character인 경우 /두개로 나뉜다.
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

			//LineTrace를 여러번 쏴야할듯
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


void ABaseGrenade::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if ( Hit.bBlockingHit )
	{
		if ( UGrenadePDA* GPDA = Cast< UGrenadePDA>(this->ItemSetting.DataAsset) )
		{
			if ( GPDA->GrenadeBounceSound )
			{
				BounceAudioComp->SetSound(GPDA->GrenadeBounceSound);
			}

		}

		if ( BounceAudioComp)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ABaseGrenade:: playsound"));
			
			BounceAudioComp->Activate();

			//AI가 인식할 수 있도록 함.
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
		}
	}
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
	if ( EffectAudioComp->IsActive())
	{
		EffectAudioComp->Stop();// Deactivate();
	}

	//GetWorldTimerManager().RemoveTimer
	
	GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);
	OnGrenadeDestroy.Broadcast(this);

	//던지고 나서 Effect이후 Destroy를 해준다.
	OwningPlayer = nullptr;
	Destroy();
}
