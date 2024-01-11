// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/GrenadeFragment.h"

void AGrenadeFragment::BeginEffect()
{
	Super::BeginEffect();

	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;

	FRadialDamageEvent RadialDmgEvt;
	const TArray<AActor*> IgnoreActarr;
	TSubclassOf<UDamageType> const DmgType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	UGameplayStatics::ApplyRadialDamage(GetWorld(), GPDA->Damage, GetActorLocation(), GPDA->EffectRadius, DmgType, IgnoreActarr, this, GetOwningPlayer()->GetInstigatorController(), true);


	//안먹음
	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),GPDA->Damage, GPDA->Damage - GPDA->Damage*0.1f,GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius*0.2f,GPDA->EffectRadius, 1.f,
	//	DmgType, IgnoreActarr, OwningPlayer,GetInstigatorController());

	DrawDebugSphere(GetWorld(), GetActorLocation(), GPDA->EffectRadius, 12, FColor::Cyan, false, 5.f, 0, 2.f);
	//DrawDebugSphere(GetWorld(),GetActorLocation(), GPDA->EffectRadius - GPDA->EffectRadius * 0.2f,12,FColor::Red,false,5.f,0,2.f);


	TArray<FHitResult> Hits;
	FVector Location = GetActorLocation();
	FCollisionShape Shape = FCollisionShape::MakeSphere(GPDA->EffectRadius);
	//ECollisionChannel Collisionch = 

	/* RadialForce와 TakeDamage를 합침. */
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
				meshcomp->AddRadialForce(Location, GPDA->EffectRadius, 20000.f, ERadialImpulseFalloff::RIF_Constant, true);
			}
			else if (Char)
			{
				Char->GetMesh()->AddRadialForce(Location, GPDA->EffectRadius, 20000.f, ERadialImpulseFalloff::RIF_Constant, true);
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

	GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &AGrenadeBase::EndEffect, 1.33f, false);

}