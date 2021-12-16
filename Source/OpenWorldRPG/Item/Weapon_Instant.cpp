// Fill out your copyright notice in the Description page of Project Settings.
#pragma warning(disable : 4800)

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
	
	NextSpread = SpreadPoint;
	CalcRecoilNApply(&PreviousSpread, &NextSpread);
	PreviousSpread = NextSpread;

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
	float RecoilValue = 0.f;
	/* 초탄은 무조건 원하는 지점으로 가게 한다. */
	if (Main)
	{
		if (FireCount > 0)
		{
			float randX = 0.0f;
			float randY = 0.0f;
			float NewZ = 0.0f;
			if (FireCount < 5)
			{
				if (bIsAiming)
				{
					NewZ = FireCount * BulletStat.AimBulletSpread;
					LastZpos = NewZ;
				}
				else
				{
					NewZ = FireCount * BulletStat.HipBulletSpread/3;
					LastZpos = NewZ;
				}
				//RecoilValue = -1 * (float)FireCount / 10;
			}
			else
			{
				if (bIsAiming)
				{
					randX = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);
					randY = FMath::RandRange(-1 * BulletStat.AimBulletSpread, BulletStat.AimBulletSpread);

					//NewZ = FireCount * (FireCount / 8.5) * (BulletStat.AimBulletSpread / 10);
					
					NewZ = FireCount * (BulletStat.AimBulletSpread / 30) + LastZpos;
					LastZpos = NewZ;
				}
				else
				{
					randX = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);
					randY = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);

					//NewZ = FireCount * (FireCount / 8.5) * (BulletStat.HipBulletSpread / 10);
					
					NewZ = FireCount * (BulletStat.HipBulletSpread / 30) + LastZpos;
					LastZpos = NewZ;
				}
				//RecoilValue = -1 * (float)FireCount / 20;
			}
			//UE_LOG(LogTemp, Warning, TEXT("Test Value : %.4f"), (float)FireCount / 20);
			//UE_LOG(LogTemp, Warning, TEXT("NewZ : %f"), NewZ);
			TempVector = FVector(Vec.X + randX, Vec.Y + randY, Vec.Z + NewZ);
			//OwningPlayer->GetInstigatorController()->SetControlRotation(RotVector.Rotation());
			//Main->AddControllerPitchInput(RecoilValue);
			
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

void AWeapon_Instant::CalcRecoilNApply(FVector *PreSpread, FVector *NexSpread)
{
	if (FireCount < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("FireCnt is under 1, no calc recoil."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PreSpread : %s"), *(PreSpread->ToString()));
	UE_LOG(LogTemp, Warning, TEXT("NexSpread : %s"), *(NexSpread->ToString()));

	/*float X = (PreSpread->X - NexSpread->X);
	float Y = (PreSpread->Y - NexSpread->Y);
	float Z = (PreSpread->Z - NexSpread->Z);*/

	//UE_LOG(LogTemp, Warning, TEXT("X : %f"), X);
	//UE_LOG(LogTemp, Warning, TEXT("Y : %f"), Y);
	//UE_LOG(LogTemp, Warning, TEXT("Y : %f"), Z);

	/* Left & Right 계산*/
	FVector CrossVec = FVector::CrossProduct(*PreSpread - GetInstigator()->GetActorLocation(), *NexSpread- GetInstigator()->GetActorLocation());
	//FVector CrossVec = FVector::CrossProduct(*PreSpread, *NexSpread);
	DrawDebugLine(GetWorld(), GetActorLocation(), CrossVec, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);
	float YawJudge = FVector::DotProduct(CrossVec.GetSafeNormal(), GetInstigator()->GetActorUpVector());
	
	/* Up & Down 계산*/
	float TESTPITCH = CrossVec.Z;//FVector::DotProduct(CrossVec, GetInstigator()->GetActorUpVector()); //test용

	float PitchJudge = FVector::DotProduct(*PreSpread, *NexSpread);
	float SizeValue = PreSpread->Size() * NexSpread->Size();
	float PitchAngle = FMath::Acos(PitchJudge / SizeValue);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("Judge : %f"), JudgeValue);
		if (YawJudge >= -0.05f && YawJudge <= 0.05f)
		{
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("Nothing"));
			UE_LOG(LogTemp, Warning, TEXT("Nothing"));
		}
		else
		{
			if (YawJudge > 0.f)
			{
				GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("right"));
				UE_LOG(LogTemp, Warning, TEXT("Right"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("left"));
				UE_LOG(LogTemp, Warning, TEXT("LEFT"));
			}
		}

		if (CrossVec.Z > 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Up"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Down"));
		}

		
	}
	UE_LOG(LogTemp, Warning, TEXT("Yaw Judge : %f"), YawJudge);
	UE_LOG(LogTemp, Warning, TEXT("Pitch Judge : %f"), PitchAngle);
	UE_LOG(LogTemp, Warning, TEXT("CrossVec : %s"), *CrossVec.ToString());
	UE_LOG(LogTemp, Warning, TEXT("TESTPITCH : %f"), TESTPITCH);
	
	//AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
	
}

float AWeapon_Instant::PitchRecoilValue(float Zvalue)
{
	return 0.0f;
}

float AWeapon_Instant::YawRecoilValue(FVector Vec)
{
	return 0.0f;
}