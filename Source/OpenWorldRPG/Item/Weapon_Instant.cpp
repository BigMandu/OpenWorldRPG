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

	UE_LOG(LogTemp, Warning, TEXT("Spread Point : %s"), *SpreadPoint.ToString());

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
					NewZ = 0.f;
					//NewZ = BulletStat.AimBulletSpread / 100; //FireCount* (BulletStat.AimBulletSpread / 100) +LastZpos;
					//LastZpos = NewZ;
				}
				else
				{
					randX = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);
					randY = FMath::RandRange(-1 * BulletStat.HipBulletSpread, BulletStat.HipBulletSpread);

					//NewZ = FireCount * (FireCount / 8.5) * (BulletStat.HipBulletSpread / 10);
					NewZ = 0.f;
					//NewZ = BulletStat.HipBulletSpread * 0.05; //FireCount * (BulletStat.HipBulletSpread / 50);// +LastZpos;
					//LastZpos = NewZ;
				}
				//RecoilValue = -1 * (float)FireCount / 20;
			}
			//UE_LOG(LogTemp, Warning, TEXT("Test Value : %.4f"), (float)FireCount / 20);
			//UE_LOG(LogTemp, Warning, TEXT("NewZ : %f"), NewZ);
			GetInstigator()->AddControllerPitchInput(NewZ* -0.005);
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
	if (FireCount < 1)// || PreviousSpread != FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("FireCnt is under 1, no calc recoil."));
		return;
	}

	/*UE_LOG(LogTemp, Warning, TEXT("PreSpread : %s"), *(PreSpread->ToString()));
	UE_LOG(LogTemp, Warning, TEXT("NexSpread : %s"), *(NexSpread->ToString()));*/

	/* Left & Right 계산*/
	//FVector CrossVec = FVector::CrossProduct(*PreSpread, *NexSpread).GetSafeNormal();
	FVector CrossVec = FVector::CrossProduct(*PreSpread - GetInstigator()->GetActorLocation(), *NexSpread - GetInstigator()->GetActorLocation());
	float YawJudge = FVector::DotProduct(CrossVec.GetSafeNormal(), GetInstigator()->GetActorUpVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), CrossVec, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);


	/* Up & Down 계산*/
	FVector PitchVector = FVector::CrossProduct(CrossVec - GetInstigator()->GetActorLocation(), GetInstigator()->GetActorUpVector());
	float PitchJudge = FVector::DotProduct(PitchVector.GetSafeNormal(), GetInstigator()->GetActorForwardVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), PitchVector, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);
	

	/* 각도 계산 */
	float VectorAngle = FVector::DotProduct(*PreSpread, *NexSpread);
	float SizeValue = PreSpread->Size() * NexSpread->Size();
	float PitchAngle = FMath::Acos(VectorAngle / SizeValue);

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
				/*GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("right"));
				UE_LOG(LogTemp, Warning, TEXT("Right"));*/
				
			}
			else
			{
				/*GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("left"));
				UE_LOG(LogTemp, Warning, TEXT("LEFT"));*/
				
			}
			GetInstigator()->AddControllerYawInput(YawJudge/5.f);
		}

		if (PitchJudge < 0.f)
		{
			/*UE_LOG(LogTemp, Warning, TEXT("Up"));
			UE_LOG(LogTemp, Warning, TEXT("Pitch Angle : %f"), PitchAngle);*/
			//GetInstigator()->AddControllerPitchInput(PitchAngle *-20);
		}
		else
		{
			/*UE_LOG(LogTemp, Warning, TEXT("Down"));
			UE_LOG(LogTemp, Warning, TEXT("Pitch Angle : %f"), PitchAngle * -1);*/
			//GetInstigator()->AddControllerPitchInput(PitchAngle);
		}

		
	}
	/*UE_LOG(LogTemp, Warning, TEXT("Yaw Judge : %f"), YawJudge);
	UE_LOG(LogTemp, Warning, TEXT("Pitch Judge : %f"), PitchJudge);*/
	/*UE_LOG(LogTemp, Warning, TEXT("Pitch Judge : %f"), PitchJudge);
	UE_LOG(LogTemp, Warning, TEXT("CrossVec : %s"), *CrossVec.ToString());
	UE_LOG(LogTemp, Warning, TEXT("PitchVector : %s"), *PitchVector.ToString());*/
	
	
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