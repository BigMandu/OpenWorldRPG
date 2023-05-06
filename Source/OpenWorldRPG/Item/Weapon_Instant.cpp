// Fill out your copyright notice in the Description page of Project Settings.
#pragma warning(disable : 4800)

#include "Weapon_Instant.h"

#include <string>

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h" //디버깅용


AWeapon_Instant::AWeapon_Instant() : Super()
{

}



/* New_BulletOut함수는 ApplyRecoil함수에서 Weapon에 추가한
 * CurveFloat을 이용해 그려진 Spread를 Time을 기준으로 가져오는 방식이다.
 * 카운터 스트라이크의 Spread시스템과 유사하다.
 */
void AWeapon_Instant::New_BulletOut()
{
	FTransform AimPos = GetAimPosition();
	FVector StartTrace = GetTraceStartLocation(AimPos.Rotator().Vector());
	FVector EndTrace = GetTraceEndLocation(StartTrace, AimPos.Rotator().Vector());
	 
	 	
	//그냥 WorldAimPos를 EndPoint로 지정할경우.
	//가끔 딱 맞아떨어질때, Hit이 안먹히는 경우가 생겨 거리를 늘려준다.
	//FHitResult Hit = BulletTrace(StartTrace, WorldAimPosition + AimPos.Rotator().Vector() * 30.f);
	FHitResult Hit = BulletTrace(StartTrace, EndTrace);

	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);

	if (Hit.bBlockingHit)
	{
		DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Blue, false, 5.f);
		CheckHit(Hit, AimPos.Rotator().Vector());
	}
	GetWorldTimerManager().ClearTimer(RecoilHandle);

	//여기서, Burst모드일 때 따로 Recoil을 적용하기 위해 다른 함수를 호출하자.
	//Burst모드는 Random으로 적용하자.
	ApplyRecoil();
}


//Old_BulletOut을 가져다썼다.
void AWeapon_Instant::AIBulletOut()
{
	if(GetInstigatorController())
	{
		int32 Seed = FMath::Rand();
		FRandomStream BulletRandomStream(Seed);
		float LastSpread = 10.f + CurFiringSpread; //함수로 빼서 AimBulletSpread추가.
		float ConeHalfAngle = FMath::DegreesToRadians(LastSpread *0.5);


		FVector Dir = SKMesh->GetSocketRotation(MuzzleFlashSocketName).Vector();  //GetInstigatorController()->GetControlRotation().Vector();// = GetAimRotation();
		FVector StartTrace = SKMesh->GetSocketLocation(MuzzleFlashSocketName); // GetTraceStartLocation(Dir);

		// 이어서 새로추가. (RandomStream을 이용한 Weapon Spread)  

		FVector ShootDir = BulletRandomStream.VRandCone(Dir, ConeHalfAngle*2, ConeHalfAngle);
		DrawDebugCone(GetWorld(), StartTrace, Dir, 2000.f, ConeHalfAngle*2, ConeHalfAngle, 6, FColor::Blue, false, 2.f, (uint8)nullptr, 1.f);	//2000.f = WeaponStat.WeaponRange
		FVector EndTrace = StartTrace + ShootDir * 2000.f;///WeaponStat.WeaponRange;

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Magenta, false, 2.f, uint8(0), 4.f);

		FHitResult Hit = BulletTrace(StartTrace, EndTrace);
		
		if(Hit.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Green, false, 3.f);
			CheckHit(Hit,Dir);
		}
		CurFiringSpread = (10.f < CurFiringSpread + 1.f) ? 10.f : CurFiringSpread + 1.f; //RandomStream을 이용한 Spread.
	}
}




/* Recoil_X와 Recoil_Y는 Curve float 에디터에 생성해놓음.
 * rpm이 950면, rps는 15.8, 0.06초당 1발
 */
void AWeapon_Instant::ApplyRecoil()
{

	if (WeaponDataAsset->WeaponStat.Recoil_X && WeaponDataAsset->WeaponStat.Recoil_Y)
	{
		/* Recoil Time은 AWeapon::EndFiring에서 0으로 초기화 된다.
		 * Recoil Time을 기준으로 Curve Float값을 가져와 지난번과, 다음에 쏠 값을 가져와
		 * 상승값을 구해 적용한다.
		 */
		float LastRecoilTime = RecoilTime - WeaponDataAsset->WeaponStat.SecondPerBullet;
		float LastRecoilValue_X = WeaponDataAsset->WeaponStat.Recoil_X->GetFloatValue(LastRecoilTime);
		float NextRecoilValue_X = WeaponDataAsset->WeaponStat.Recoil_X->GetFloatValue(RecoilTime);

		float LastRecoilValue_Y = WeaponDataAsset->WeaponStat.Recoil_Y->GetFloatValue(LastRecoilTime);
		float NextRecoilValue_Y = WeaponDataAsset->WeaponStat.Recoil_Y->GetFloatValue(RecoilTime);

		PitchValue = (NextRecoilValue_X - LastRecoilValue_X);
		YawValue = (NextRecoilValue_Y - LastRecoilValue_Y);

		/* 조준 유무에 따라 반동을 준다. */
		if (bIsAiming)
		{
			PitchValue *= WeaponDataAsset->WeaponStat.AimBulletSpread;
			YawValue *= WeaponDataAsset->WeaponStat.AimBulletSpread;
		}
		else
		{
			PitchValue *= WeaponDataAsset->WeaponStat.HipBulletSpread;
			YawValue *= WeaponDataAsset->WeaponStat.HipBulletSpread;
		}

		//초반 반동을 약하게 줬기때문에 (Semiauto, Fullauto시 한발씩 끊어쏠때 보정을 위함)
		//Burst모드일때 너무 사기가 되버려 강제로 PitchValue를 더 준다.
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
			PitchValue *= 1.2f;
		}

		//UE_LOG(LogTemp, Warning, TEXT("Pitch : %f, Yaw : %f"), PitchValue, YawValue);

		WorldTime = 0.f;
		RecoilAlphaTime = 0.f;

		GetWorldTimerManager().SetTimer(RecoilHandle, [=] {
			WorldTime += GetWorld()->GetDeltaSeconds();
			RecoilAlphaTime = WorldTime / (WeaponDataAsset->WeaponStat.SecondPerBullet * 1.5);

			float LerpRecoilX = UKismetMathLibrary::Lerp(0, PitchValue, RecoilAlphaTime);
			float LerpRecoilY = UKismetMathLibrary::Lerp(0, YawValue, RecoilAlphaTime);

			//UE_LOG(LogTemp, Warning, TEXT("Lerp X : %f, Lerp Y: %f"), LerpRecoilX, LerpRecoilY);
			GetInstigator()->AddControllerPitchInput(LerpRecoilX);
			GetInstigator()->AddControllerYawInput(LerpRecoilY);

			}, GetWorld()->GetDeltaSeconds(), true);

		/*GetInstigator()->AddControllerPitchInput(PitchValue);
		GetInstigator()->AddControllerYawInput(YawValue);*/

		//리코일 타임은 1발을 쏠때의 타임만큼씩 증가해야한다.
		RecoilTime = RecoilTime + WeaponDataAsset->WeaponStat.SecondPerBullet;
		


		//UE_LOG(LogTemp, Warning, TEXT("RecoilTime : %f"), RecoilTime);
		if (RecoilTime > WeaponDataAsset->WeaponStat.SecondPerBullet * 30) //30을 나중에 탄창 최대개수로 바꾸면됨.
		{
			//UE_LOG(LogTemp, Warning, TEXT("RecoilTime max, set 0.5"));
			RecoilTime = 0.3f;
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("there is no Recoil_X, Y file"));
	}
}


void AWeapon_Instant::CheckHit(const FHitResult& Hit, const FVector Dir)
{
	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor name : %s"), *Hit.GetActor()->GetFName().ToString());

			ApplyDamage(Hit, Dir);
		}

		if (WeaponDataAsset->BulletHitEffect)
		{
			
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponDataAsset->BulletHitEffect, Hit.Location, Hit.ImpactNormal.Rotation());
		}

		if (WeaponDataAsset->WeaponSound.BulletHitSound )
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponDataAsset->WeaponSound.BulletHitSound, Hit.Location);
		}
	}

}

void AWeapon_Instant::ApplyDamage(const FHitResult& Hit, const FVector Dir)
{
	FPointDamageEvent PointDmgEvt;
	PointDmgEvt.HitInfo = Hit;
	PointDmgEvt.ShotDirection = Dir.GetSafeNormal();
	PointDmgEvt.Damage = 90.f; //임시 원래는 WeaponDataAsset-> 이거 써야됨
		
	Hit.GetActor()->TakeDamage(PointDmgEvt.Damage, PointDmgEvt, OwningPlayer->GetController(), this);
	//TakeDamage(10.f, PointDmgEvent, OwningPlayer->GetInstigatorController(), this);
}


/*******************************************************************************************/
//************************        이하는 사용하지 않는 함수들     ****************************/
/*******************************************************************************************/



// Old_BulletOut 
/* 이 함수는 더 이상 사용하지 않는다. New_BulletOut 함수를 사용할것.
 *
 * Random Stream을 이용한 방법과, Rand값을 이용한 방법을 사용했던 함수다.
 * Random Stream은 Spread를 따로 외우거나 익힐 필요 없는 단순한 반동이며
 * Rand값을 이용한 Spread는 개발자가 의도한 대로 Spread를 뿌려 줄 수 있지만, 한계가 있다.
 */

/*
void AWeapon_Instant::Old_BulletOut()
{
	//UE_LOG(LogTemp, Warning, TEXT("Weap_Instant::BulletOut"));
	
	// Rondom Stream 을 이용한 Weapon Spread 
	
	////int32 Seed = FMath::Rand();
	////FRandomStream BulletRandomStream(Seed);
	////float LastSpread = BulletStat.HipBulletSpread + CurFiringSpread; //함수로 빼서 AimBulletSpread추가.
	////float ConeHalfAngle = FMath::DegreesToRadians(LastSpread *0.5);
	

	FVector Dir;// = GetAimRotation();
	FVector StartTrace = GetTraceStartLocation(Dir);

	// 이어서 새로추가. (RandomStream을 이용한 Weapon Spread)  
	
	////FVector ShootDir = BulletRandomStream.VRandCone(Dir, ConeHalfAngle*2, ConeHalfAngle);
	////DrawDebugCone(GetWorld(), StartTrace, Dir, WeaponStat.WeaponRange, ConeHalfAngle*2, ConeHalfAngle, 6, FColor::Green, false, 2.f, (uint8)nullptr, 4.f);	
	////FVector EndTrace = StartTrace + ShootDir * WeaponStat.WeaponRange;
	
	
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
*/


// BulletSpread 함수도 사용하지 않는다. Old_BulletOut에서 사용.
/*
FVector AWeapon_Instant::BulletSpread(FVector Vec)
{
	// Random을 이용한 Weapon Spread 
	FVector TempVector = Vec;
	AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
	
	// 초탄은 무조건 원하는 지점으로 가게 한다.
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
					NewZ = FireCount * WeaponStat.AimBulletSpread;
					LastZpos = NewZ;
				}
				else
				{
					NewZ = FireCount * WeaponStat.HipBulletSpread/3;
					LastZpos = NewZ;
				}
				//RecoilValue = -1 * (float)FireCount / 10;
			}
			else
			{
				if (bIsAiming)
				{
					randX = FMath::RandRange(-1 * WeaponStat.AimBulletSpread, WeaponStat.AimBulletSpread);
					randY = FMath::RandRange(-1 * WeaponStat.AimBulletSpread, WeaponStat.AimBulletSpread);

					//NewZ = FireCount * (FireCount / 8.5) * (BulletStat.AimBulletSpread / 10);
					NewZ = 0.f;
					//NewZ = BulletStat.AimBulletSpread / 100; //FireCount* (BulletStat.AimBulletSpread / 100) +LastZpos;
					//LastZpos = NewZ;
				}
				else
				{
					randX = FMath::RandRange(-1 * WeaponStat.HipBulletSpread, WeaponStat.HipBulletSpread);
					randY = FMath::RandRange(-1 * WeaponStat.HipBulletSpread, WeaponStat.HipBulletSpread);

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
*/


// CalcRecoilNApply함수도 사용하지 않는다. Old Bullet Out에서 사용.
/*
void AWeapon_Instant::CalcRecoilNApply(FVector *PreSpread, FVector *NexSpread)
{
	if (FireCount < 1)// || PreviousSpread != FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("FireCnt is under 1, no calc recoil."));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("PreSpread : %s"), *(PreSpread->ToString()));
	//UE_LOG(LogTemp, Warning, TEXT("NexSpread : %s"), *(NexSpread->ToString()));

	// Left & Right 계산
	FVector CrossVec = FVector::CrossProduct(*PreSpread - GetInstigator()->GetActorLocation(), *NexSpread - GetInstigator()->GetActorLocation());
	float YawJudge = FVector::DotProduct(CrossVec.GetSafeNormal(), GetInstigator()->GetActorUpVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), CrossVec, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);


	// Up & Down 계산
	FVector PitchVector = FVector::CrossProduct(CrossVec - GetInstigator()->GetActorLocation(), GetInstigator()->GetActorUpVector());
	float PitchJudge = FVector::DotProduct(PitchVector.GetSafeNormal(), GetInstigator()->GetActorForwardVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), PitchVector, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);
	

	// 각도 계산 
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
				//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("right"));
				//UE_LOG(LogTemp, Warning, TEXT("Right"));
				
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, TEXT("left"));
				//UE_LOG(LogTemp, Warning, TEXT("LEFT"));
				
			}
			GetInstigator()->AddControllerYawInput(YawJudge);
		}

		if (PitchJudge < 0.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Up"));
			UE_LOG(LogTemp, Warning, TEXT("Pitch Angle : %f"), PitchAngle);
			//GetInstigator()->AddControllerPitchInput(PitchAngle *-20);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Down"));
			//UE_LOG(LogTemp, Warning, TEXT("Pitch Angle : %f"), PitchAngle * -1);
			//GetInstigator()->AddControllerPitchInput(PitchAngle);
		}

		
	}
	//UE_LOG(LogTemp, Warning, TEXT("Yaw Judge : %f"), YawJudge);
	//UE_LOG(LogTemp, Warning, TEXT("Pitch Judge : %f"), PitchJudge);
	//UE_LOG(LogTemp, Warning, TEXT("Pitch Judge : %f"), PitchJudge);
	//UE_LOG(LogTemp, Warning, TEXT("CrossVec : %s"), *CrossVec.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("PitchVector : %s"), *PitchVector.ToString());
	
	
	//AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
	
}
*/


//
//float AWeapon_Instant::PitchRecoilValue(float Zvalue)
//{
//	return 0.0f;
//}
//
//float AWeapon_Instant::YawRecoilValue(FVector Vec)
//{
//	return 0.0f;
//}
