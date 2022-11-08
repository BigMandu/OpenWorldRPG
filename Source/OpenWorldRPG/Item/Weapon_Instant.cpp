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
#include "DrawDebugHelpers.h" //������


AWeapon_Instant::AWeapon_Instant() : Super()
{

}



/* New_BulletOut�Լ��� ApplyRecoil�Լ����� Weapon�� �߰���
 * CurveFloat�� �̿��� �׷��� Spread�� Time�� �������� �������� ����̴�.
 * ī���� ��Ʈ����ũ�� Spread�ý��۰� �����ϴ�.
 */
void AWeapon_Instant::New_BulletOut()
{
	FTransform AimPos = GetAimPosition();
	FVector StartTrace = GetTraceStartLocation(AimPos.Rotator().Vector());
	FVector EndTrace = GetTraceEndLocation(StartTrace, AimPos.Rotator().Vector());
	 

	
	//�׳� WorldAimPos�� EndPoint�� �����Ұ��.
	//���� �� �¾ƶ�������, Hit�� �ȸ����� ��찡 ���� �Ÿ��� �÷��ش�.
	//FHitResult Hit = BulletTrace(StartTrace, WorldAimPosition + AimPos.Rotator().Vector() * 30.f);
	FHitResult Hit = BulletTrace(StartTrace, EndTrace);

	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
	DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Blue, false, 5.f);

	CheckHit(Hit, AimPos.Rotator().Vector());

	GetWorldTimerManager().ClearTimer(RecoilHandle);

	//���⼭, Burst����� �� ���� Recoil�� �����ϱ� ���� �ٸ� �Լ��� ȣ������.
	//Burst���� Random���� ��������.
	ApplyRecoil();
}

/* Recoil_X�� Recoil_Y�� Curve float �����Ϳ� �����س���.
 * rpm�� 950��, rps�� 15.8, 0.06�ʴ� 1��
 */
void AWeapon_Instant::ApplyRecoil()
{

	if (WeaponDataAsset->WeaponStat.Recoil_X && WeaponDataAsset->WeaponStat.Recoil_Y)
	{
		/* Recoil Time�� AWeapon::EndFiring���� 0���� �ʱ�ȭ �ȴ�.
		 * Recoil Time�� �������� Curve Float���� ������ ��������, ������ �� ���� ������
		 * ��°��� ���� �����Ѵ�.
		 */
		float LastRecoilTime = RecoilTime - WeaponDataAsset->WeaponStat.SecondPerBullet;
		float LastRecoilValue_X = WeaponDataAsset->WeaponStat.Recoil_X->GetFloatValue(LastRecoilTime);
		float NextRecoilValue_X = WeaponDataAsset->WeaponStat.Recoil_X->GetFloatValue(RecoilTime);

		float LastRecoilValue_Y = WeaponDataAsset->WeaponStat.Recoil_Y->GetFloatValue(LastRecoilTime);
		float NextRecoilValue_Y = WeaponDataAsset->WeaponStat.Recoil_Y->GetFloatValue(RecoilTime);

		PitchValue = (NextRecoilValue_X - LastRecoilValue_X);
		YawValue = (NextRecoilValue_Y - LastRecoilValue_Y);

		/* ���� ������ ���� �ݵ��� �ش�. */
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

		//�ʹ� �ݵ��� ���ϰ� ��⶧���� (Semiauto, Fullauto�� �ѹ߾� ����� ������ ����)
		//Burst����϶� �ʹ� ��Ⱑ �ǹ��� ������ PitchValue�� �� �ش�.
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

		//������ Ÿ���� 1���� ���� Ÿ�Ӹ�ŭ�� �����ؾ��Ѵ�.
		RecoilTime = RecoilTime + WeaponDataAsset->WeaponStat.SecondPerBullet;
		


		//UE_LOG(LogTemp, Warning, TEXT("RecoilTime : %f"), RecoilTime);
		if (RecoilTime > WeaponDataAsset->WeaponStat.SecondPerBullet * 30) //30�� ���߿� źâ �ִ밳���� �ٲٸ��.
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

		if (WeaponDataAsset->BulletHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponDataAsset->BulletHitSound, Hit.Location);
		}
	}

}

void AWeapon_Instant::ApplyDamage(const FHitResult& Hit, const FVector Dir)
{
	FPointDamageEvent PointDmgEvt;
	PointDmgEvt.HitInfo = Hit;
	PointDmgEvt.ShotDirection = Dir.GetSafeNormal();
	PointDmgEvt.Damage = 90.f; //�ӽ� ������ WeaponDataAsset-> �̰� ��ߵ�
		
	Hit.GetActor()->TakeDamage(PointDmgEvt.Damage, PointDmgEvt, OwningPlayer->GetController(), this);
	//TakeDamage(10.f, PointDmgEvent, OwningPlayer->GetInstigatorController(), this);
}


/*******************************************************************************************/
//************************        ���ϴ� ������� �ʴ� �Լ���     ****************************/
/*******************************************************************************************/



// Old_BulletOut 
/* �� �Լ��� �� �̻� ������� �ʴ´�. New_BulletOut �Լ��� ����Ұ�.
 *
 * Random Stream�� �̿��� �����, Rand���� �̿��� ����� ����ߴ� �Լ���.
 * Random Stream�� Spread�� ���� �ܿ�ų� ���� �ʿ� ���� �ܼ��� �ݵ��̸�
 * Rand���� �̿��� Spread�� �����ڰ� �ǵ��� ��� Spread�� �ѷ� �� �� ������, �Ѱ谡 �ִ�.
 */

/*
void AWeapon_Instant::Old_BulletOut()
{
	//UE_LOG(LogTemp, Warning, TEXT("Weap_Instant::BulletOut"));
	
	// Rondom Stream �� �̿��� Weapon Spread 
	
	////int32 Seed = FMath::Rand();
	////FRandomStream BulletRandomStream(Seed);
	////float LastSpread = BulletStat.HipBulletSpread + CurFiringSpread; //�Լ��� ���� AimBulletSpread�߰�.
	////float ConeHalfAngle = FMath::DegreesToRadians(LastSpread *0.5);
	

	FVector Dir;// = GetAimRotation();
	FVector StartTrace = GetTraceStartLocation(Dir);

	// �̾ �����߰�. (RandomStream�� �̿��� Weapon Spread)  
	
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

	//CurFiringSpread = (10.f < CurFiringSpread + 1.f) ? 10.f : CurFiringSpread + 1.f; //RandomStream�� �̿��� Spread.
}
*/


// BulletSpread �Լ��� ������� �ʴ´�. Old_BulletOut���� ���.
/*
FVector AWeapon_Instant::BulletSpread(FVector Vec)
{
	// Random�� �̿��� Weapon Spread 
	FVector TempVector = Vec;
	AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
	
	// ��ź�� ������ ���ϴ� �������� ���� �Ѵ�.
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


// CalcRecoilNApply�Լ��� ������� �ʴ´�. Old Bullet Out���� ���.
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

	// Left & Right ���
	FVector CrossVec = FVector::CrossProduct(*PreSpread - GetInstigator()->GetActorLocation(), *NexSpread - GetInstigator()->GetActorLocation());
	float YawJudge = FVector::DotProduct(CrossVec.GetSafeNormal(), GetInstigator()->GetActorUpVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), CrossVec, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);


	// Up & Down ���
	FVector PitchVector = FVector::CrossProduct(CrossVec - GetInstigator()->GetActorLocation(), GetInstigator()->GetActorUpVector());
	float PitchJudge = FVector::DotProduct(PitchVector.GetSafeNormal(), GetInstigator()->GetActorForwardVector());
	//DrawDebugLine(GetWorld(), GetActorLocation(), PitchVector, FColor::Green, false, 1.f, (uint8)nullptr, 2.f);
	

	// ���� ��� 
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
