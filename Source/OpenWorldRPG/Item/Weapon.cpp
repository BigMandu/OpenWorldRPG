// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "EquipmentComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Curves/CurveFloat.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h" //디버깅용
#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#define DEBUG 0

AWeapon::AWeapon() : Super()
{
	//SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	//RootComponent = SKMesh;
	//Mesh->SetupAttachment(GetRootComponent());
	//SKMesh->SetHiddenInGame(true);

	bIsFiring = false;
	bLMBDown = false;
	bDetectLookInput = false;

	//MuzzleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffectComp"));
	//MuzzleEffectComp->SetupAttachment(GetRootComponent());
	MuzzleFlashSocketName = FName("muzzleflash");

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;

	RifleAssign = ERifleAssign::ERA_MAX;
}

//void AWeapon::SetOwningPlayer(AActor* Actor)
//{
//	if (OwningPlayer != Actor)
//	{
//		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
//		if (Main)
//		{
//			OwningPlayer = Main;
//			SetInstigator(Main); //Instigator 설정.
//		}
//	}
//}

void AWeapon::Equip(AActor* Char)
{
	Super::Equip(Char);
	AMainCharacter* Main = Cast<AMainCharacter>(Char);
	check(Main);

	if (RifleAssign == ERifleAssign::ERA_MAX)
	{
		/* 1,2,3을 눌렀을때 Quick Swap하기 위해 */
		if (EquipmentType == EEquipmentType::EET_Rifle) //라이플이고
		{
			if (Main->PrimaryWeapon) //이미 주무기가 있으면
			{
				RifleAssign = ERifleAssign::ERA_Sub;
				Main->SubWeapon = this;// (AWeapon*)this; //부무기로 지정
			}
			else //주무기가 없으면
			{
				RifleAssign = ERifleAssign::ERA_Primary;
				Main->PrimaryWeapon = this;// (AWeapon*)this; //주무기로
			}
		}
		else //피스톨
		{
			Main->PistolWeapon = this; // (AWeapon*)this;
		}
	}


	//들고 있는 무기가 없을 경우 지금 Weapon을 들도록 한다.
	if (Main->EquippedWeapon == nullptr)
	{
		if (EquipmentType == EEquipmentType::EET_Rifle)
		{
			if (Main->PrimaryWeapon)
			{
				Main->ChangeWeapon(1);
			}
			else if (Main->SubWeapon)
			{
				Main->ChangeWeapon(2);
			}

		}
		else
		{
			Main->ChangeWeapon(3);
		}

		//GunAttachToMesh(Main);
	}



	WeaponStat.FireRatePerSec = WeaponStat.FireRatePerMin / 60;
	WeaponStat.SecondPerBullet = 1 / WeaponStat.FireRatePerSec; //0.06

	//broadcast를 위해 AddEquipment 함수를 호출한다. 이미 추가되어있기 때문에, broadcast말고는 하는게 없다.
	Main->Equipment->AddEquipment(this);
	
	GunAttachToMesh(Char);
	
}

void AWeapon::GunAttachToMesh(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		/* 이미 들고 있는 무기가 있고, 들고있는 무기와 이 무기가 다른경우
		* 이미 들고 있는 무기를 'SubWeapon Attach' 소켓에 부착시킨다.
		*/
		if (Main->EquippedWeapon && (Main->EquippedWeapon != this))// && ItemState == EItemState::EIS_Spawn)
		{
			const USkeletalMeshSocket* SubSocket = Main->GetMesh()->GetSocketByName("SubWeaponAttachSocket");

			SubSocket->AttachActor(Main->EquippedWeapon, Main->GetMesh());
			Main->EquippedWeapon->SetActorRelativeTransform(MeshAttachTransform);

			/*SubSocket->AttachActor(this, Main->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);*/

		}
		
	
		const USkeletalMeshSocket* TPSocket = Main->GetMesh()->GetSocketByName("WeaponGrip");
		const USkeletalMeshSocket* FPSocket = Main->FPMesh->GetSocketByName("WeaponGrip");

		if (TPSocket && FPSocket)
		{
			switch (Main->CameraMode)
			{
			case ECameraMode::ECM_FPS:

				//AttachToActor(Main,)
				if (FPSocket->AttachActor(this, Main->FPMesh))
				{
					Main->BaseWeapTransform = SKMesh->GetRelativeTransform();
				}
				break;
			case ECameraMode::ECM_TPS:
				if (TPSocket->AttachActor(this, Main->GetMesh()))
				{

				}
				break;
			}				

			//SKMesh->SetVisibility(true);
			SKMesh->SetHiddenInGame(false);
			Main->EquippedWeapon = this;
		}		

	}
}


void AWeapon::FPS_AimAttachToMesh(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main && Main->EquippedWeapon)
	{
		if (Main->CameraMode == ECameraMode::ECM_FPS)
		{
			const USkeletalMeshSocket* FP_HipSocket = Main->FPMesh->GetSocketByName("WeaponGrip");
			const USkeletalMeshSocket* FP_AimSocket = Main->FPMesh->GetSocketByName("WeaponAimGrip");
			if (FP_HipSocket && FP_AimSocket)
			{
				switch (Main->AimMode)
				{
				case EAimMode::EAM_Aim:
					FP_AimSocket->AttachActor(this, Main->FPMesh);
					break;
				case EAimMode::EAM_NotAim:
					FP_HipSocket->AttachActor(this, Main->FPMesh);
					break;
				}
			}
		}
	}
}

//void AWeapon::Drop()
//{
//	Super::Drop();
//
//	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
//	SKMesh->SetHiddenInGame(true);
//	OwningEquipment = false;
//	
//	
//}

void AWeapon::ChangeSafetyLever()
{
	if (EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_Safe:
			UE_LOG(LogTemp, Warning, TEXT("Semi Auto"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
			break;
		case EWeaponFiringMode::EWFM_SemiAuto:
			UE_LOG(LogTemp, Warning, TEXT("3-round burst"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_Burst;
			break;
		case EWeaponFiringMode::EWFM_Burst:
			UE_LOG(LogTemp, Warning, TEXT("Full Automatic"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_FullAuto;
			break;
		case EWeaponFiringMode::EWFM_FullAuto:
			UE_LOG(LogTemp, Warning, TEXT("Safe"));
			WeaponFiringMode = EWeaponFiringMode::EWFM_Safe;
			break;
		}
	}
}


void AWeapon::TempNewWeaponState()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::TempNewWeaponState"));
	EWeaponState State = EWeaponState::EWS_Idle;

	if (bLMBDown)
	{
		if (CanFire())//발사를 할 수 있다면, Firing으로 상태를 변경한다.
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("TempState -> Firing"));
#endif
			State = EWeaponState::EWS_Firing;
		}
	}
	
	//이 임시저장한 State를 Setting하기 위해 SetWeaponState함수를 호출한다.
	SetWeaponState(State);
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::SetWeaponState"));
	//기존에 발사중이었고, 더이상 LMB를 누르지 않는다면
	if (CurrentWeaponState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		/* 점사면 끝내도 지정된 몇발 이상 안쐈으면 사격되도록 함. */
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		/* 사격이 중단되어야 할때 EndFiring호출 
		 점사일때는 냅둔다, Check Firing함수에서 Firing을 호출하도록 함.*/
		if (bCanEndFire)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Call EndFiring func"));
#endif			
			CurrentWeaponState = NewState;
			EndFiring(); 
			
		}
		
	}
	//기존에 발사하지 않았고, LMB를 눌렀다면
	else if (CurrentWeaponState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{

#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Start Firing Call ControlFiring func"));
#endif			
		//사격을 시작한다.
		CurrentWeaponState = NewState;
		ControlFiring();	
	}
}

bool AWeapon::CanFire()
{
	bool bCanFire = false;
	
	/*
	* 쏠 수 있는 조건
	* 조정간 안전이 아니어야한다.
	* 장전중, 사격중이 아니어야 한다. 
	* 탄약이 한발이상 있어야 한다.
	* 
	* 장착중이 아니어야 한다. //미구현.
	* 스프린트 중이 아니어야 한다. //미구현
	*/
	if (ItemObj->bIsDestoryed == false)
	{
		if (WeaponFiringMode != EWeaponFiringMode::EWFM_Safe && CurrentWeaponState != EWeaponState::EWS_Reloading)
		{
			if (!(WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto && FireCount > 0))
			{
				bCanFire = true;
			}
		}
	}
	return bCanFire;
}

bool AWeapon::CanEndFire()
{
	return false;
}

void AWeapon::StartFire()
{
	/* 여기서 FiringMode에 따라 Delay를 줘야함.
	*	semiauto인 경우 toggle방식으로.
	*	burst인 경우 3발 사격
	*  Fullauto인 경우 연속사격
	* 
	* 여기서 timer체크를 한번 해주고 이후에 호출되는 함수를 또 두개로 나눠서 함.
	*/
	if (!bLMBDown)
	{
		bLMBDown = true;
		/*
		샷을 때렸을때 기존에 Shot을때리고 있었는지, idling상태였는지 체크해야함.
		샷을 때리고 있는 중이었으면 계속해서 공격 함수 호출(이거말고 다른 함수)
		아니면, 공격을 멈춘 함수호출해야함.(밑 함수 말고 다른거)
		*/

		TempNewWeaponState();
	}
}

void AWeapon::StopFire()
{
	if (bLMBDown)
	{
		bLMBDown = false;
		TempNewWeaponState();
	}
}

void AWeapon::ControlFiring()
{
	/* SetWeaponState에서 호출된다.
	* 
	* Timer를 통해 Firing함수를 호출한다.
	* Timer의 시간은
	* (마지막 발사 시간은 발사할때의 월드 타임)
	* 마지막 발사시간 + 발사간격 타임 = 발사 가능 시간 이 된다.
	* 그럼, 이 발사 가능시간이 월드 타임보다 크면 발사를 할 수 없다.
	* 다음 발사 시간은
	* 이 발사가능시간에 월드타임을 빼면 구해진다.
	* 
	* 예를 들어, 1초에 쏘고 2초의 간격을 가진다면
	* 3초의 월드타임에 쏠 수 있다.
	* 3초때 쏠 쑤 있는데 현재의 월드타임이 2초라면 쏠 수 없다.
	* 
	* 다음 발사시간도예를 들면
	* 3초때 쏠 쑤있는데, 현재 월드타임인 2초를 빼면
	* 1초가 남는데. 1초후에 3초가 되어 쏠 쑤 있다.
	* 이를 식으로 나타내면 된다.	
	*/

#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::ControlFiring"));
#endif			
	
	float WorldTime = GetWorld()->GetTimeSeconds();
	
	/* 점사 모드일때는 강제로 시간을 더 추가한다. */
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//점사모드면 재발사 가능 시간을 늦춰준다.
	if((bIsBurstmode && LastFireTime > 0 && LastFireTime + WeaponStat.SecondPerBullet *4 >WorldTime) ||
		(LastFireTime > 0 && LastFireTime + WeaponStat.SecondPerBullet > WorldTime))
	{
		//발사 가능시간을 구한다. 
		float RemainingTime = LastFireTime + WeaponStat.SecondPerBullet - WorldTime;

		if (bIsBurstmode) 
		{
			//점사 모드면 SecondPerBullet에 4를 곱한 값.
			RemainingTime = (LastFireTime + WeaponStat.SecondPerBullet * 4) - WorldTime;
		}
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::Firing, RemainingTime, false);
		
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No time, Call firing directly"));
		Firing();
	}
	
}

void AWeapon::Firing()
{
	/* ControlFiring, ReFiring에서 호출됨. */
	/*
		ammo 체크, FireCount 증가, 함수 끝에 Timer기록 (마지막 발사 시간 기록용)
		계속해서 사격중이라면 ReFiring함수 호출
	*/

#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Firing function"));
#endif			
	bIsFiring = true;

	if (bDetectLookInput == false)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(GetInstigator());
		if (Main)
		{
			if (Main->bIsLookInput)
			{
				bDetectLookInput = true;
			}
		}
	}

	/* 첫 발사면 ControlRotation값을 저장한다. -> 사격 종료시 원복하기 위함 */
	if (FireCount == 0)
	{
		StartFiringRotation = GetInstigatorController()->GetControlRotation();
		//UE_LOG(LogTemp, Warning, TEXT("Save Start Firing Rotation"));
		UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}

	WeaponFX();
	//BulletOut(); //Weapon Instant에 구현함.
	New_BulletOut(); //Weapon Instant

	FireCount++;
	
	UE_LOG(LogTemp, Warning, TEXT("Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	
	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();

	/*
	bool bCanReFire = CheckRefire();
	if (bCanReFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firing:: Can Refire. Set Timer"));
		//GetWorldTimerManager().SetTimer(FiringTimer,this, &AWeapon::Firing, WeaponStat.SecondPerBullet, false);
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	}
	else
	{
		//Out of Ammo
		// Check Ammo를 따로 한번 더 한다. 
		// Burst mode일때는 탄이 충분해도, 격발 횟수에 따라 재사격을 못할때도 생기니, 
		// Out of Ammo함수를 무조건 호출하는건 옳치 않음.
		// 
		// Burst mode일때는 탄약 체크를 한번 더 해야함.
		//
		//AimInitialize();
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)//EWeaponFiringMode::EWFM_Burst)
		{
			UE_LOG(LogTemp, Warning, TEXT("Firing:: Burst mode, Can NOT Refire, Call EndFiring"));
			EndFiring();
			//TempNewWeaponState(); //EndFiring을 호출하지 않고 이렇게 해봄
			//CurrentWeaponState = EWeaponState::EWS_Idle;
		}
		else if (WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto)
		{
			UE_LOG(LogTemp, Warning, TEXT("Firing:: Semi auto, Can NOT Refire, Call EndFiring"));
			TempNewWeaponState();
		}
		
	}	
	*/	

}

void AWeapon::ReFiring()
{
	//여기서는 격발 횟수 (FireCount)에 따라 Firing호출할지 말지 결정하자.
	bool bCanReFire = CheckRefire();
	if (bCanReFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReFiring:: Can Refire. call Firing"));
		Firing();
	}
	else
	{
		EndFiring();
	}
	
}

void AWeapon::EndFiring()
{
	/* Firing이 끝나면 각종 변수들을 초기화 시켜준다. */
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	//GetWorldTimerManager().ClearTimer(RecoilHandle);
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode를 위함
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;
	/* 사격을 끝냈을때 첫 사격 에임으로 되돌아 오는 기능*/
	AimInitialize();
}

bool AWeapon::CheckRefire()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::CheckRefire"));
	bool bFlag = false;
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Safe:
			case EWeaponFiringMode::EWFM_SemiAuto:
				//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can't Refire"));
				break;
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponStat.BurstRound)
				{
					//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can Refire"));
					bFlag = true;
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can't Refire"));
				}
				break;
			case EWeaponFiringMode::EWFM_FullAuto:
				//UE_LOG(LogTemp, Warning, TEXT("CheckRefire:: Can Refire"));
				bFlag = true;
				break;
			}
		}
	}
	return bFlag;
}

FVector AWeapon::GetAimRotation()
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);

	AMainController* MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	FVector ReturnAim = FVector::ZeroVector;
	
	if (MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		ReturnAim = AimRot.Vector();
	}

	return ReturnAim;
}

FVector AWeapon::GetTraceStartLocation(FVector& Dir)
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);
	
	AMainController* MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	FVector ReturnLocation = FVector::ZeroVector;

	if (MainCon)
	{
		FRotator Rot;
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);

		/*
		* FVector CalcDistance = OwningPlayer->GetActorLocation() - ReturnLocation;
		float DotP = FVector::DotProduct(CalcDistance, Dir);
		UE_LOG(LogTemp, Warning, TEXT("======================================="));
		UE_LOG(LogTemp, Warning, TEXT("ViewPoint Loc : %s , ViewPoint Rot : %s"), *ReturnLocation.ToString(), *Dir.ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation : %s"), *OwningPlayer->GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("ActorLocation - ViewPoint Loc  = %s"), *CalcDistance.ToString());
		UE_LOG(LogTemp, Warning, TEXT("size : %f, OR : %f, Dot : %f"), CalcDistance.Size(), (CalcDistance | Dir), DotP);
		UE_LOG(LogTemp, Warning, TEXT("---------------------------------------"));
		//UE_LOG(LogTemp, Warning, TEXT("Result not - ViewPoint Loc= %s"), *(ReturnLocation + Dir * OwningPlayer->GetActorLocation()).ToString());
		UE_LOG(LogTemp, Warning, TEXT(" Red  Result = %s"), *(ReturnLocation + Dir * CalcDistance).ToString());
		
		FVector tempLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation);
		FVector NewTempLocation = ReturnLocation + Dir * 700;
		FVector SizeTempLocation = ReturnLocation + Dir * CalcDistance.Size();
		
		FVector dotLocation = ReturnLocation + Dir * DotP;
		*/
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir );
		
		/*
		DrawDebugSphere(GetWorld(), ReturnLocation, 12.f, 6, FColor::Green, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), tempLocation, 12.f, 6, FColor::Red, true, 2.f, (uint8)nullptr, 2.f);
		DrawDebugSphere(GetWorld(), SizeTempLocation, 12.f, 6, FColor::Purple, true, 2.f, (uint8)nullptr, 2.f);
		//DrawDebugSphere(GetWorld(), dotLocation, 12.f, 6, FColor::Blue, true, 2.f, (uint8)nullptr, 2.f);

		UE_LOG(LogTemp, Warning, TEXT("Purple Result= %s"), *SizeTempLocation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Blue Result= %s"), *dotLocation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Green Result = %s"), *ReturnLocation.ToString());
		*/
	}
	
	return ReturnLocation;
}


FHitResult AWeapon::BulletTrace(FVector& StartTrace, FVector& EndTrace)
{
	FHitResult Hit;

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator를 IgnoreActor로 하면된다.

	
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);


	/* debug */
	/*if (GetInstigator())
	{
		FString str = GetInstigator()->GetFName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Isnt : GetInstigator: %s"), *str);
	}
	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);
	*/	

	return Hit;
}

/* 사운드와 총구 이펙트 */
void AWeapon::WeaponFX()
{
	//사운드
	if (FireSound)
	{

		//PlaySound2D를 쓰면 Actor의 위치에서 소리가 나는게 아니라 게임플레이어 한테 들린다.
		//UWorld* world = GetWorld();
		//UGameplayStatics::PlaySound2D(world, FireSound);
		UGameplayStatics::SpawnSoundAttached(FireSound, OwningPlayer->GetRootComponent());
	}

	//총구 이펙트
	if (FireMuzzleEffect)
	{
		const USkeletalMeshSocket* MuzzleSocket = SKMesh->GetSocketByName(MuzzleFlashSocketName);

		if (MuzzleSocket)
		{
			SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			FVector MuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireMuzzleEffect, MuzzleLocation, MuzzleRotation);
		}
	}

	//Animatoin 재생.
	PlayWeaponAnimAndCamShake(FireAnimaton);
}

void AWeapon::PlayWeaponAnimAndCamShake(FWeaponAnim& Anim)
{
	check(OwningPlayer)
	AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer); 
	if (MainChar && CamShake)
	{
		if (MainChar->CameraMode == ECameraMode::ECM_FPS)
		{
			MainChar->PlayAnimMontage(Anim.FPSAnim);
			MainChar->MainController->ClientStartCameraShake(CamShake, 0.7f);
			//MainChar->MainController->ClientPlayCameraShake(CamShake, 0.7f);
		}
		else
		{
			MainChar->PlayAnimMontage(Anim.TPSAnim);
			MainChar->MainController->ClientStartCameraShake(CamShake, 1.0f);
			//MainChar->MainController->ClientPlayCameraShake(CamShake, 1.0f);
		}
	}
}

void AWeapon::AimInitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::AimInitialize"));
	//UE_LOG(LogTemp, Warning, TEXT("Save End Firing Location And Init"));
	EndFiringRotation = GetInstigatorController()->GetControlRotation();
	if (bDetectLookInput)
	{
		bDetectLookInput = false;
		FRotator NewStartRot = FRotator(EndFiringRotation.Pitch - 5.f, EndFiringRotation.Yaw, EndFiringRotation.Roll);
		StartFiringRotation = NewStartRot;
	}
	UE_LOG(LogTemp, Warning, TEXT("End Rotating val : %s"), *EndFiringRotation.ToString());
	Time = 0.f;
	AlphaTime = 0.f;
	GetWorldTimerManager().SetTimer(AimInitHandle, [=] {
		//UE_LOG(LogTemp, Warning, TEXT("AimInit AlphaTime : %f"), AlphaTime);
		Time += GetWorld()->GetDeltaSeconds();
		AlphaTime = Time / 0.6f; // Time/되돌아오는 시간 (스텟)
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);
		/*UE_LOG(LogTemp, Warning, TEXT("Time : %f"), Time);
		UE_LOG(LogTemp, Warning, TEXT("AlphaTime : %f"), AlphaTime);
		UE_LOG(LogTemp, Warning, TEXT("StartFiringRotation : %s"), *StartFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("EndFiringRotation : %s"), *EndFiringRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());*/
		},GetWorld()->GetDeltaSeconds(),true);
}

void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();	
	if (OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		if (Main)
		{
			if (Main->EquippedWeapon == this)
			{
				Main->ChangeWeapon(0);
				Main->EquippedWeapon = nullptr;
				RifleAssign = ERifleAssign::ERA_MAX;
			}

			if (Main->PrimaryWeapon == this)
			{
				Main->PrimaryWeapon = nullptr;
			}
			else if(Main->SubWeapon == this)
			{
				Main->SubWeapon = nullptr;
			}
			else if(Main->PistolWeapon == this)
			{
				Main->PistolWeapon = nullptr;
			}
		}
	}
}
