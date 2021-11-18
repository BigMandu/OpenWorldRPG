// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Instant.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h" //디버깅용


AWeapon_Instant::AWeapon_Instant() : Super()
{
	
}


void AWeapon_Instant::BulletOut()
{
	//UE_LOG(LogTemp, Warning, TEXT("Weap_Instant::BulletOut"));
	
	/* Rondom Stream 을 이용한 Weapon Spread */
	/*
	int32 Seed = FMath::Rand();
	FRandomStream BulletRandomStream(Seed);
	float LastSpread = BulletStat.HipBulletSpread + CurFiringSpread; //함수로 빼서 AimBulletSpread추가.
	float ConeHalfAngle = FMath::DegreesToRadians(LastSpread *0.5);
	*/

	FVector Dir = GetAimRotation();
	FVector StartTrace = GetTraceStartLocation(Dir);

	/* 이어서 새로추가. (RandomStream을 이용한 Weapon Spread) */ 
	/*
	FVector ShootDir = BulletRandomStream.VRandCone(Dir, ConeHalfAngle*2, ConeHalfAngle);
	DrawDebugCone(GetWorld(), StartTrace, Dir, WeaponStat.WeaponRange, ConeHalfAngle*2, ConeHalfAngle, 6, FColor::Green, false, 2.f, (uint8)nullptr, 4.f);	
	FVector EndTrace = StartTrace + ShootDir * WeaponStat.WeaponRange;
	*/
	
	FVector EndTrace = StartTrace + Dir * WeaponStat.WeaponRange;
	FVector SpreadPoint = BulletSpread(EndTrace);

	FHitResult Hit = BulletTrace(StartTrace, SpreadPoint);

	DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Blue, false, 2.f);
	CheckHit(Hit);

	//CurFiringSpread = (10.f < CurFiringSpread + 1.f) ? 10.f : CurFiringSpread + 1.f; //RandomStream을 이용한 Spread.
}

FVector AWeapon_Instant::BulletSpread(FVector Vec)
{
	/* Random을 이용한 Weapon Spread */
	FVector TempVector = Vec;
	AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);

	/* 초탄은 무조건 원하는 지점으로 가게 한다. */
	if (Main)
	{
		if (FireCount > 0)
		{
			float randX = 0.0f;
			float randY = 0.0f;
			float NewZ = 0.0f;
			if (FireCount < 3)
			{
				if (bIsAiming)
				{
					//randX = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);
					//randY = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);

					NewZ = FireCount * BulletStat.AimBulletSpread;
					LastZpos = NewZ;
				}
				else
				{
					//randX = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);
					//randY = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);

					NewZ = FireCount * BulletStat.HipBulletSpread/3;
					LastZpos = NewZ;
				}
			}
			else
			{
				if (bIsAiming)
				{
					randX = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);
					randY = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);

					NewZ = FireCount * (FireCount / 8.5) * (BulletStat.AimBulletSpread / 10) + LastZpos;
				}
				else
				{
					randX = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);
					randY = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);

					NewZ = FireCount * (FireCount / 8.5) * (BulletStat.HipBulletSpread / 10) + LastZpos;
				}
			}
			
			TempVector = FVector(Vec.X + randX, Vec.Y + randY, Vec.Z + NewZ);
		}
	}
	return TempVector;
}

void AWeapon_Instant::CheckHit(FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor name : %s"), *Hit.GetActor()->GetFName().ToString());
			//TakeDamage
		}

		if (BulletHitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitEffect,Hit.Location,Hit.ImpactNormal.Rotation());
		}

		if (BulletHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, Hit.Location);
		}
	}
	
}

