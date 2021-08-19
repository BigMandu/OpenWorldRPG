// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Instant.h"
#include "DrawDebugHelpers.h" //디버깅용


AWeapon_Instant::AWeapon_Instant() : Super()
{
	
}


void AWeapon_Instant::BulletOut()
{
	//UE_LOG(LogTemp, Warning, TEXT("Weap_Instant::BulletOut"));
	
	FVector Dir = GetAimRotation();

	FVector StartTrace = GetTraceStartLocation(Dir);
	FVector EndTrace = StartTrace + Dir * WeaponStat.WeaponRange;

	/*float Dotp = (StartTrace | GetActorForwardVector());
	UE_LOG(LogTemp, Warning, TEXT("dot : %f"), Dotp);*/

	FHitResult Hit = BulletTrace(StartTrace, EndTrace); 
	CheckHit(Hit);
}

FHitResult AWeapon_Instant::BulletTrace(FVector& StartTrace, FVector& EndTrace)
{
	FHitResult Hit;
	
	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator를 IgnoreActor로 하면된다.
	
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST,params);

	/* debug */
	/*if (GetInstigator())
	{
		FString str = GetInstigator()->GetFName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Isnt : GetInstigator: %s"), *str);
	}*/
	//
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);

	return Hit;
}

void AWeapon_Instant::CheckHit(FHitResult& Hit)
{
	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor name : %s"), *Hit.GetActor()->GetFName().ToString());
	}
}