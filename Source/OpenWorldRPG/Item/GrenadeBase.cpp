// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/GrenadeBase.h"
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
#include "NiagaraCollision.h"
#include "NiagaraEmitterInstance.h"

#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

AGrenadeBase::AGrenadeBase()
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

	Mesh->OnComponentHit.AddDynamic(this, &AGrenadeBase::OnCapsuleComponentHit);
	//CapsuleComp->OnComponentHit.AddDynamic(this,&ABaseGrenade::OnCapsuleComponentHit);
}


void AGrenadeBase::OnConstruction(const FTransform& Transform)
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
}

//QuickSlotWidget -> NewItemObject::UseItem에서 호출함.
void AGrenadeBase::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj, bool bIsNeedToDestory)
{
	Super::AttachToHand(Actor,Obj, bIsNeedToDestory);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetLinearDamping(0.01f);
	Mesh->SetAngularDamping(0.f);
	SetOwningPlayer(Actor);
}

//While LMB down
void AGrenadeBase::ReadyToThrow()
{
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;
	UE_LOG(LogTemp,Warning,TEXT("AGrenadeBase::ReadyTo Throw"));
	
	bReadyToThrow = true;

	
	if(OwningPlayer)
	{
		//다른 AnimMontage가 재생중이면 Stop시켜서
		//AnimBP의 Anim을 재생 할 수 있도록 한다.
		OwningPlayer->StopAnimation();
		OwningPlayer->ChangeWeaponTypeNumber(3);
	}
}


//called when LMB Up
//Detach from hand
void AGrenadeBase::ThrowGrenade(ABaseCharacter* Actor)
{
	Actor->PlayAnimation(this->ItemSetting.DataAsset->TPS_UseAnimMontage, this->ItemSetting.DataAsset->FPS_UseAnimMontage);
	
	//던지고 난 이후엔 Interaction이 불가능하도록 한다.
	bCanNotInteractable = true;

	//detach하기 전에 bool먼저 update	
	bThrow = true;

	//Item을 Inventory에서 지워주고
	RemoveCountAtIventory(Actor,1);
}


void AGrenadeBase::DetectThrow(ABaseCharacter* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: DetectThrow, detect AnimNotify"));
	
	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);

	if(GPDA == nullptr ) return;
	
	//Detach를 한다.
	DetachFromHand(Actor, GPDA->bIsNeedToAttachHandBeforeUse);

	//Hit Event를 발생시키기 위함.
	Mesh->SetNotifyRigidBodyCollision(true);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName("BlockAllDynamic");
	
	Mesh->SetLinearDamping(2.f);
	Mesh->SetAngularDamping(1.f);

	FTransform AimPos = GetAimPosition();
	FVector StartTrace = GetTraceStartLocation(AimPos.Rotator().Vector());
	FVector EndVec = StartTrace + AimPos.GetRotation().Vector() * 100.f;
	

	DrawDebugSphere(GetWorld(), StartTrace, 5.f, 6, FColor::Green, false, 3.f, 0, 2.f);
	DrawDebugSphere(GetWorld(), EndVec, 5.f, 6, FColor::Blue, false, 3.f, 0, 2.f);
	

	//FVector CalcVector = (EndVec - GetActorLocation()) * 15.f; //Actor->GetActorForwardVector() * 1000.f;	
	FVector CalcVector = (EndVec - StartTrace) * 50.f; //Actor->GetActorForwardVector() * 1000.f;	
	DrawDebugSphere(GetWorld(), CalcVector, 5.f, 6, FColor::Red, false, 3.f, 0, 2.f);

	Mesh->SetPhysicsLinearVelocity(CalcVector);

	FTimerDelegate TimerCallback;
	TimerCallback.BindUFunction(this, FName("BeginEffect"));
	
	//GetWorldTimerManager().SetTimer(EffectTriggerTimerHandle, this, &AGrenadeBase::BeginEffect, GPDA->EffectDelayTime, false);
	GetWorldTimerManager().SetTimer(EffectTriggerTimerHandle, TimerCallback, GPDA->EffectDelayTime, false);

	//ProjectileMovementComp->Velocity = GetAimPosition().GetRotation().Vector() * 500.f;//ProjectileMovementComp->InitialSpeed;
	//DrawDebugLine(GetWorld(), GetAimPosition().GetRotation().Vector(), ProjectileMovementComp->Velocity, FColor::Green, false, 3.f, 0, 2.f);



	////던진뒤 PrimaryWeapon을 장착시킨다.
	//Actor->HoldingItem = nullptr;
	//EquipBeforeWeapon(Actor);
}

//timer ready to throw
void AGrenadeBase::BeginEffect()
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

		Ni_ParticleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Ni_ParticleComp->SetGenerateOverlapEvents(true);
		//Ni_ParticleComp->SetCollisionObjectType()		
		
		Ni_ParticleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//Ni_ParticleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		Ni_ParticleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		Ni_ParticleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);		
	}

	
	EffectAudioComp->SetAutoActivate(true);
	Ca_ParticleComp->SetWorldRotation(FRotator(0.f, 0.f, 1.f));
	EffectAudioComp->SetSound(GPDA->GrenadeEffectSound);	
	EffectAudioComp->Activate();

	//AI가 인식할 수 있도록 함.
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);

	/*
	switch (GPDA->GrenadeType)
	{
	case EGrenadeType::EGT_Fragment:
		FragmentEffect(GPDA);
	break;
	case EGrenadeType::EGT_Smoke:
		SmokeEffect(GPDA);
	break;
	}	
	*/

	//던지지 않은 상태에서 터졌을 경우 -> 해당 경우는 삭제함
	/*
	if (bThrow == false)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(GetOwningPlayer());
		DetachFromHand(BChar, true);
		RemoveCountAtIventory(BChar, 1);
		bThrow = true;
		bCanNotInteractable = true;

		
		//BChar->HoldingItem = nullptr;
		//EquipBeforeWeapon(BChar);
	}
	*/
}


void AGrenadeBase::EndEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: EndEffect"));
	if (Ca_ParticleComp->IsActive())
	{
		Ca_ParticleComp->DeactivateSystem(); //Deactivate();
	}

	if (Ni_ParticleComp->IsActive())
	{
		Ni_ParticleComp->Deactivate();
	}

	if ( EffectAudioComp->IsActive())
	{
		EffectAudioComp->Stop();// Deactivate();
	}

	//GetWorldTimerManager().RemoveTimer
	
	GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);

	//EndEffect이후 바로 destory를 하지 않고 일정 초 이후에 삭제하도록 한다.
	GetWorldTimerManager().SetTimer(DestoryGrenadeTimerHandle, this, &AGrenadeBase::StartDestory, 10.f, false);
}

UFUNCTION()
void AGrenadeBase::StartDestory()
{
	OnGrenadeDestroy.Broadcast(this);
	//던지고 나서 Effect이후 Destroy를 해준다.
	OwningPlayer = nullptr;
	Destroy();
}


// Grenade를 throw하고 bounce될때 sound를 출력하기 위함.
void AGrenadeBase::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		if (UGrenadePDA* GPDA = Cast< UGrenadePDA>(this->ItemSetting.DataAsset))
		{
			if (GPDA->GrenadeBounceSound)
			{
				BounceAudioComp->SetSound(GPDA->GrenadeBounceSound);
			}

		}

		if (BounceAudioComp)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: playsound"));

			BounceAudioComp->Activate();

			//AI가 인식할 수 있도록 함.
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this);
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////
/*                              안쓰는 함수                                       */
////////////////////////////////////////////////////////////////////////////////////

//아래 함수는 sub class로 분할 해서 더이상 안씀, 주석때문에 남겨둠

/*
void AGrenadeBase::FragmentEffect(UGrenadePDA* gPDA)
{
	if (gPDA == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: FragmentEffect"));

	FRadialDamageEvent RadialDmgEvt;
	const TArray<AActor*> IgnoreActarr;
	TSubclassOf<UDamageType> const DmgType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	UGameplayStatics::ApplyRadialDamage(GetWorld(), gPDA->Damage, GetActorLocation(), gPDA->EffectRadius, DmgType, IgnoreActarr, this, GetOwningPlayer()->GetInstigatorController(), true);


	//안먹음
	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),GPDA->Damage, GPDA->Damage - GPDA->Damage*0.1f,GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius*0.2f,GPDA->EffectRadius, 1.f,
	//	DmgType, IgnoreActarr, OwningPlayer,GetInstigatorController());

	DrawDebugSphere(GetWorld(), GetActorLocation(), gPDA->EffectRadius, 12, FColor::Cyan, false, 5.f, 0, 2.f);
	//DrawDebugSphere(GetWorld(),GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius * 0.2f,12,FColor::Red,false,5.f,0,2.f);


	TArray<FHitResult> Hits;
	FVector Location = GetActorLocation();
	FCollisionShape Shape = FCollisionShape::MakeSphere(gPDA->EffectRadius);
	//ECollisionChannel Collisionch = 

	// RadialForce와 TakeDamage를 합침.
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, Location, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), Shape);
	}

	for (int32 ind = 0; ind < Overlaps.Num(); ++ind)
	{
		const FOverlapResult& Overlap = Overlaps[ ind ];
		AActor* OverlapAct = Overlap.GetActor();

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
			//if (OverlapAct->CanBeDamaged())
			//{
			//	FHitResult OutHit;
			//	FVector EndLocation = OverlapAct->GetActorLocation();
			//	if (GetWorld()->LineTraceSingleByChannel(OutHit, Location, EndLocation, ECollisionChannel::ECC_Visibility))
			//	{
			//		TSubclassOf<UDamageType> const DmgType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			//		FDamageEvent DmgEvt = FDamageEvent(DmgType);
			//		OverlapAct->TakeDamage(gPDA->Damage, DmgEvt, GetOwningPlayer()->GetInstigatorController(), this);
			//	}
			//}
		}
	}

	//GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &AGrenadeBase::EndEffect, 2.f, false);

}
*/


/*
void AGrenadeBase::SmokeEffect(UGrenadePDA* gPDA)
{
	if (gPDA == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: SmokeEffect"));

	if (SmokeEffectVisibleBlockSphere)
	{
		if (gPDA->Ni_GrenadeEffect)
		{
			//UNiagaraFunctionLibrary::SpawnSystemAtLocation
			// 

		}
		else if (gPDA->Ca_GrenadeEffect)
		{

		}
		//SmokeEffectVisibleBlockSphere->SetSphereRadius(gPDA->)
	}

	//GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &AGrenadeBase::EndEffect, gPDA->SmokeDuration, false);
}
*/