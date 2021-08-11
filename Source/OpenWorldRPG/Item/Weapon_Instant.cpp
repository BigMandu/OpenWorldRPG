// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Instant.h"
#include "DrawDebugHelpers.h" //µð¹ö±ë¿ë


AWeapon_Instant::AWeapon_Instant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}


void AWeapon_Instant::BulletOut()
{
	UE_LOG(LogTemp, Warning, TEXT("Weap_Instant::BulletOut"));
	
	FTransform form = GetCamLocRot();
	FVector StartTrace = form.GetLocation();
	FVector Direction = form.GetRotation().Vector();
	FVector EndTrace = StartTrace + Direction * WeaponStat.WeaponRange;

	FHitResult Hit = BulletTrace(StartTrace, EndTrace);
	CheckHit(Hit);
}

FHitResult AWeapon_Instant::BulletTrace(FVector& StartTrace, FVector& EndTrace)
{
	FHitResult Hit;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_Pawn,params);

	/* debug */
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green);

	return Hit;
}

void AWeapon_Instant::CheckHit(FHitResult& Hit)
{
	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor name : %s"), *Hit.GetActor()->GetFName().ToString());
	}
}