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
//#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "OpenWorldRPG/MainAnimInstance.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"


#define DEBUG 0

AWeapon::AWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetupAttachment(RootComponent);	

	WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
	CurrentWeaponState = EWeaponState::EWS_Idle;
	RifleAssign = ERifleSlot::ERS_MAX;

	bIsFiring = false;
	bLMBDown = false;
	bDetectLookInput = false;

	//MuzzleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffectComp"));
	//MuzzleEffectComp->SetupAttachment(GetRootComponent());
	MuzzleFlashSocketName = FName("muzzleflash");
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/* New Aim System, */
	if (MainCon != nullptr ) //if (OwningPlayer != nullptr)
	{
		if (MainCon->Main->EquippedWeapon == this)
		{
			UpdateAim();
			WeaponClipping();

			//DrawDebugLine(GetWorld(), GetActorForwardVector(), GetActorLocation() + GetActorRotation().Vector() * 100.f, FColor::Cyan, false, 1.f, 0, 2.f);
		}
	}
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CapsuleComp->SetHiddenInGame(false); //for debug

	//CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnCollisionBegin);
	//CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnCollisionEnd);

	//CapsuleComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); //Weapon콜리전 Object로 지정.
	//CapsuleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	//CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}
void AWeapon::SettingRifleAssign(ABaseCharacter* BChar, ERifleSlot RifleSlot)
{
	switch (RifleSlot)
	{
	case ERifleSlot::ERS_Primary:
		BChar->PrimaryWeapon = this;
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::SettingRifleAssign , this is Primary"));
		break;
	case ERifleSlot::ERS_Sub:
		BChar->SubWeapon = this;
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::SettingRifleAssign , this is Sub"));
		break;
	}
	RifleAssign = RifleSlot;
}

bool AWeapon::StepEquip(AActor* Char, ERifleSlot RifleSlot)
{
	Super::StepEquip(Char);
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Char);
	check(BChar);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	UE_LOG(LogTemp, Warning, TEXT("Weapon Transform : %s"), *OriginalWeaponTransform.ToString());

	if (RifleAssign == ERifleSlot::ERS_MAX)
	{
		if (EquipmentType == EEquipmentType::EET_Rifle)
		{
			/* 1,2,3을 눌렀을때 Quick Swap하기 위해 */
			if (RifleSlot == ERifleSlot::ERS_MAX) //지정된 Slot이 없을때
			{
				if (BChar->PrimaryWeapon) //이미 주무기가 있으면
				{
					//RifleAssign = ERifleSlot::ERS_Sub;
					//BChar->SubWeapon = this; //부무기로 지정
					SettingRifleAssign(BChar, ERifleSlot::ERS_Sub);
				}
				else //주무기가 없으면
				{
					//RifleAssign = ERifleSlot::ERS_Sub;
					//BChar->PrimaryWeapon = this; //주무기로
					SettingRifleAssign(BChar, ERifleSlot::ERS_Primary);
				}
			}
			//Rifle Type을 Drag&Drop으로 Equip을 진행했을땐, 아래로 됨.
			else
			{
				//RifleAssign = RifleSlot;
				SettingRifleAssign(BChar, RifleSlot);

			}
		}
		
		else if (EquipmentType == EEquipmentType::EET_Pistol)
		{
			BChar->PistolWeapon = this;
		}
	}

	


	//들고 있는 무기가 없을 경우 지금 Weapon을 들도록 한다.
	if (BChar->EquippedWeapon == nullptr)
	{
		if (EquipmentType == EEquipmentType::EET_Rifle)
		{
			if (BChar->PrimaryWeapon)
			{
				BChar->ChangeWeapon(1);
			}
			else if (BChar->SubWeapon)
			{
				BChar->ChangeWeapon(2);
			}

		}
		else
		{
			BChar->ChangeWeapon(3);
		}

		//GunAttachToMesh(Main);
	}



	WeaponStat.FireRatePerSec = WeaponStat.FireRatePerMin / 60;
	WeaponStat.SecondPerBullet = 1 / WeaponStat.FireRatePerSec; //0.06

	//broadcast를 위해 AddEquipment 함수를 호출한다. 이미 추가되어있기 때문에, broadcast말고는 하는게 없다.
	BChar->Equipment->AddEquipment(this);
	
	GunAttachToMesh(BChar);
	return true;
}

void AWeapon::GunAttachToMesh(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	AMainCharacter* Main = Cast<AMainCharacter>(BChar);

	if (BChar)
	{
		/* 들고 있었던 무기가 있고, 들고있었던 무기와 이 무기가 다른경우
		* 들고 있었던 무기를 'SubWeapon Attach' 소켓에 부착시킨다.
		*/
		if (BChar->EquippedWeapon && (BChar->EquippedWeapon != this))// && ItemState == EItemState::EIS_Spawn)
		{
			const USkeletalMeshSocket* AttachSocket;
			FTransform AttachTransform;

			if(BChar->EquippedWeapon->RifleAssign == ERifleSlot::ERS_Primary)
			{
				AttachSocket = BChar->GetMesh()->GetSocketByName("PrimaryWeaponAttachSocket");
				AttachTransform = PrimaryWeaponAttachTransform;
			}
			else
			{
				AttachSocket = BChar->GetMesh()->GetSocketByName("SubWeaponAttachSocket");
				AttachTransform = SubWeaponAttachTransform;
			}

			if (AttachSocket)
			{
				AttachSocket->AttachActor(BChar->EquippedWeapon, BChar->GetMesh());
				BChar->EquippedWeapon->SetActorRelativeTransform(AttachTransform);
			}
			/*SubSocket->AttachActor(this, Main->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);*/

		}
		
	

		// 1, 3인칭 변경시 weapon attach를 변경한다. (Main에만 적용)
		if (Main)
		{
			const USkeletalMeshSocket* TPSocket = Main->GetMesh()->GetSocketByName("WeaponGrip");
			const USkeletalMeshSocket* FPSocket = Main->FPMesh->GetSocketByName("WeaponGrip");

			if (TPSocket && FPSocket)
			{
				switch (Main->CameraMode)
				{
				case ECameraMode::ECM_FPS:
					if (FPSocket->AttachActor(this, Main->FPMesh))
					{
						SetActorRelativeTransform(FPMeshAttachTransform);

						//Main->BaseWeapTransform = SKMesh->GetRelativeTransform();
					}
					break;
				case ECameraMode::ECM_TPS:
					if (TPSocket->AttachActor(this, Main->GetMesh()))
					{
						SetActorRelativeTransform(MeshAttachTransform);
					}
					break;
				}

				SKMesh->SetHiddenInGame(false);
				Main->SetEquippedWeapon(this);
			}
		}

		//BChar만 null이 아닌경우 -> AI에만 해당
		if (BChar && Main == nullptr)
		{
			const USkeletalMeshSocket* WeaponSocket = BChar->GetMesh()->GetSocketByName("WeaponGrip");
			if (WeaponSocket)
			{
				if (WeaponSocket->AttachActor(this, BChar->GetMesh()))
				{
					SetActorRelativeTransform(MeshAttachTransform);
					//SKMesh->SetHiddenInGame(false);
					BChar->SetEquippedWeapon(this);
				}
			}

		}

	}
}

FTransform AWeapon::GetSightSocketTransform()
{
	//if have any sight device. return that location.
	
	FTransform ReturnTransform;

	ReturnTransform = SKMesh->GetSocketTransform("AimPos", ERelativeTransformSpace::RTS_World);

	return ReturnTransform;
}

/*
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
*/

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
		// 점사면 끝내도 지정된 몇발 이상 안쐈으면 사격되도록 함.
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		//사격이 중단되어야 할때 EndFiring호출 
		 //점사일때는 냅둔다-> Check Firing함수에서 Firing을 호출하도록 함.
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


	/*
	* 쏠 수 있는 조건
	* 1. 조정간 안전이 아니어야한다.
	* 2. 장전중, 사격중이 아니어야 한다.
	* 3. 탄약이 한발이상 있어야 한다.
	*
	* 4. 장착중이 아니어야 한다. //미구현.
	* 5. 스프린트 중이 아니어야 한다. //미구현
	*/
bool AWeapon::CanFire()
{
	bool bCanFire = false;
	
	
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


   /* FiringMode에 따라 Delay를 줘야함.
	* semiauto인 경우 toggle방식으로.
	* burst인 경우 3발 사격
	* Fullauto인 경우 연속사격
	*
	* 여기서 timer체크를 한번 해주고 이후에 호출되는 함수를 또 두개로 나눠서 함.
	*/
void AWeapon::StartFire()
{
	
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
	
	// 점사 모드일때는 강제로 시간을 더 추가한다. 
	// Enum을 써도 되지만 if문이 너무 길어져서 boolean변수를 하나 추가한거다.
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//마지막 발사시간과 총의 SPB (발사당 필요한 초)를 더한 값이 worldTime보다 크다면
	// 아직 발사 할 수 없으므로 발사가능 시간을 구한다.

	//점사모드면 발사 가능 시간을 늦춘 조건으로 검사한다.
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
		//발사가 가능하다면 바로 호출한다.
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
		//UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}

	WeaponFX();
	//BulletOut(); //Weapon Instant에 구현함.
	New_BulletOut(); //Weapon Instant

	FireCount++;
	
	UE_LOG(LogTemp, Warning, TEXT("Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	
	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();
}

//ReFiring을 할 수 있는지 확인하는 함수다.
void AWeapon::ReFiring()
{
	//실제로 체크하는 함수를 호출한다.
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

// Firing이 끝나면 각종 변수들을 초기화 시켜준다.
void AWeapon::EndFiring()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode를 위함
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;

	// 사격을 끝냈을때 첫 사격 에임으로 되돌아 오는 기능 ,,, test를 위해 잠시 기능을 off.
	//AimInitialize();
}

//Refire가 가능한지 체크한다.
bool AWeapon::CheckRefire()
{
	bool bFlag = false;
	//격발이 가능한지 우선 체크하고
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			//Burst모드일때는 BurstRound이하로 쐈을때 refire가 가능하다.
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponStat.BurstRound)
				{
					bFlag = true;
				}
				break;
			case EWeaponFiringMode::EWFM_FullAuto:
				bFlag = true;
				break;
			}
		}
	}
	return bFlag;
}

/*
FVector AWeapon::GetAimLocation_TEST()
{
	check(OwningPlayer)

	if (MainCon == nullptr)
	{
		AMainCharacter* MainChar = Cast<AMainCharacter>(OwningPlayer);
		MainCon = MainChar ? Cast<AMainController>(MainChar->MainController) : nullptr;
	}
	//FVector ReturnAim = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	FVector ReturnAim = FVector::ZeroVector;

	if(MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		//ReturnAim = FTransform(AimRot, AimLoc);
		ReturnAim = AimLoc;
	}
	return ReturnAim;
}
*/

// 플레이어의 시점각도 (뷰포트가 바라보고 있는 회전각)를 Vector값으로 구한다.
FTransform AWeapon::GetAimPosition()
{
	check(OwningPlayer)

	FTransform ReturnAim = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	//FVector ReturnAim = FVector::ZeroVector;

	if (MainCon)
	{
		FVector AimLoc = FVector::ZeroVector;
		FRotator AimRot = FRotator::ZeroRotator;
		MainCon->GetPlayerViewPoint(AimLoc, AimRot);

		ReturnAim = FTransform(AimRot, AimLoc);
		//ReturnAim = AimRot.Vector();
	}

	return ReturnAim;
}

/* 위 함수에서 구한 각과 스타트 로케이션을 내적 벡터 해줘서 최종 탄 시작 위치를 구한다.*/
FVector AWeapon::GetTraceStartLocation(FVector Dir)
{
	check(OwningPlayer)
	check(MainCon)

	FVector ReturnLocation;
	FRotator Rot;

	//ClippingWall 함수가 실행되면, 탄 시작 위치를 Muzzle의 위치로 바꾼다.
	//함수가 실행되지 않으면,  탄시작 위치는 Actor와(Actor-CamLo = Actor의 옆위치가 된다) Dir의 사이각이 된다.
	if(bIsHighReady)
	{
		FVector WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		ReturnLocation = WeaponMuzzleLocation; //GetActorLocation();
	}
	else
	{
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir);
	}
	
	return ReturnLocation;
}

FVector AWeapon::GetTraceEndLocation(FVector StartVector, FVector Dir)
{
	FVector ReturnVec;

	//ClippingWall함수가 실행되면 탄의 종료 위치는 Muzzle의 방향*거리가 되며
	//함수가 실행되지 않으면, WorldAimPosition이 된다.
	if(bIsHighReady)
	{
		FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);
		ReturnVec = StartVector + MuzzleRotation.Vector() * WeaponStat.WeaponRange;
	}
	else
	{
		ReturnVec = WorldAimPosition + Dir * 30.f;
	}
	return ReturnVec;
}



FHitResult AWeapon::BulletTrace(FVector StartTrace, FVector EndTrace)
{
	FHitResult Hit;

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator를 IgnoreActor로 하면된다.

	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);

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

	//fire중 마우스 입력이 감지되면 AimInit을 단순히 아래로 내리게 한다.
	if (bDetectLookInput)
	{
		bDetectLookInput = false;
		FRotator NewStartRot = FRotator(EndFiringRotation.Pitch - 3.f, EndFiringRotation.Yaw, EndFiringRotation.Roll);
		StartFiringRotation = NewStartRot;
	}
	//UE_LOG(LogTemp, Warning, TEXT("End Rotating val : %s"), *EndFiringRotation.ToString());
	Time = 0.f;
	AlphaTime = 0.f;
	GetWorldTimerManager().SetTimer(AimInitHandle, [=] {
		
		Time += GetWorld()->GetDeltaSeconds();
		AlphaTime = Time / 0.8f; // : Time/되돌아오는 시간  (스텟)

		//FRotator LerpAimRotation = FMath::RInterpTo(EndFiringRotation, StartFiringRotation, GetWorld()->GetDeltaSeconds(), 20.f);
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);

		//UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());
		},GetWorld()->GetDeltaSeconds(),true);
}

void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();	
	if (OwningPlayer)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(OwningPlayer);
		if (BChar)
		{
			if (BChar->EquippedWeapon == this)
			{
				BChar->ChangeWeapon(0);
				BChar->EquippedWeapon = nullptr;
				//Main->SetEquippedWeapon(nullptr);
				RifleAssign = ERifleSlot::ERS_MAX;
				UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove , Remove RifleAssign"));
			}

			if (BChar->PrimaryWeapon == this)
			{
				UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Primary..."));
				BChar->PrimaryWeapon = nullptr;
			}
			else if(BChar->SubWeapon == this)
			{
				UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Sub..."));
				BChar->SubWeapon = nullptr;
			}
			else if(BChar->PistolWeapon == this)
			{
				BChar->PistolWeapon = nullptr;
			}
		}
	}
}

void AWeapon::UpdateAim()
{
	FTransform AimPos = GetAimPosition();
	FVector EndVec = AimPos.GetLocation() + AimPos.GetRotation().Vector() * 3000.f;
	FHitResult Hit = BulletTrace(AimPos.GetLocation(), EndVec);

	/*if (bIsHighReady)
	{
		WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		WorldAimPosition = WeaponMuzzleLocation + AimPos.GetRotation().GetForwardVector() * WeaponStat.WeaponRange;
	}*/
	//else
	{
		if (Hit.bBlockingHit)
		{
			//WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			WorldAimPosition = Hit.Location;


			//FPS시점에서 Trace를 같은곳에 한번 더 쏴서 맞으면 맞은 위치로 AimPos를 옮긴다.
			FHitResult WeaponHit = BulletTrace(MainCon->Main->CameraFPS->GetComponentLocation(), Hit.Location);
			//FHitResult WeaponHit = BulletTrace(WeaponMuzzleLocation, Hit.Location);
			if (WeaponHit.bBlockingHit)
			{
				WorldAimPosition = WeaponHit.Location;
			}
		}
		else
		{
			//DrawDebugPoint(GetWorld(), WorldAimPosition, 20.f, FColor::Blue, false, 0.1f);
			WorldAimPosition = EndVec;
		}
	}

	//DrawDebugPoint(GetWorld(), WorldAimPosition, 10.f, FColor::Green, false, -1.f);

}

void AWeapon::WeaponClipping()
{
	FTransform CurrentMeshAttachTransform;
	FVector CurrentMeshRightHand;
	if (ECameraMode::ECM_FPS == MainCon->Main->CameraMode)
	{
		CurrentMeshAttachTransform = FPMeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->FPMesh->GetSocketLocation("Hand_R");

	}
	else 
	{
		CurrentMeshAttachTransform = MeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->GetMesh()->GetSocketLocation("hand_r");
	}

	//LienTrace의 길이는 Weapon에 부착된 CapsuleComp길이
	float Length = CapsuleComp->GetScaledCapsuleHalfHeight() * 4;

	//방향벡터는 AimPosition으로 한다.
	FVector EndLo = CurrentMeshRightHand + GetAimPosition().GetRotation().GetForwardVector() * Length;
	TArray<AActor*> IgnoreActorList;
	IgnoreActorList.Add(GetInstigator());
	FHitResult Hit;

	ETraceTypeQuery TTQ = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CurrentMeshRightHand, EndLo, 5.f, TTQ, false,
		IgnoreActorList, EDrawDebugTrace::None, Hit, true);
	
	
	if(Hit.bBlockingHit)
	{
		FVector CurVec = CurrentMeshAttachTransform.GetTranslation();
		//FVector CalcVec = CurrentMeshAttachTransform.GetTranslation() - (NewY * 50.f);
		
		FRotator CurRot = CurrentMeshAttachTransform.GetRotation().Rotator();

		//Why Roll?? -> Skeleton에서 무기를 넣고 위로 돌리니 Roll값만 바껴서 Roll로 한거임.
		// Hit.Distance / Length하게되면 0부터 1이 나오는데.
		// Hit.Distance == Length -> 1이됨. 즉, 1일때는 0이되고 0일때는 90도가 되어야함.
		// 때문에 90을 곱했고, 90을 걍 곱해버리면 1일때 90이 되버림. 그리고 어차피 각도가 -90이 되야 위로 올라가니
		// 0*90 = 0, 0.5*90 = 45, 1*90 = 90이라 걍 -90을 빼버렸음,
		// 최종값은 0일때 -90, 0.5일때 -45, 1일때 0이 나옴.
		FRotator CalcRot = FRotator(CurRot.Pitch, CurRot.Yaw, (CurRot.Roll + (Hit.Distance / Length)*90.f) - 90.f);
		//UE_LOG(LogTemp, Warning, TEXT("Calc Rot ; %s"), *CalcRot.ToString());

		
		//FTransform NewTransform = FTransform(CalcRot, CurVec);
		CurrentMeshAttachTransform = FTransform(CalcRot, CurVec);
		
		SetActorRelativeTransform(CurrentMeshAttachTransform);
		bIsHighReady = true;
	}
	else
	{
		SetActorRelativeTransform(CurrentMeshAttachTransform);
		bIsHighReady = false;
	}
}


/* Fix Clipping wall */
/*
void AWeapon::OnCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT(" Weapon Overlap begin"));

	if(OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		
		if(Main->TPAnimInstance->bBeginHighReady == false)
		{
			//Weaponclass에 있는 bIsHighReady는 AnimInstance에서 완전히 내려왔을때 false를 시켜주도록 하자.
			bIsHighReady = true;
			//Main->TPAnimInstance->bBeginHighReady = true;
			//Main->FPAnimInstance->bBeginHighReady = true;

			// New Fix Clipping wall
			//Tick에서 Sphere Line Trace를 쏘는데, distance와 radius는 설정 가능함.(총마다 다를수 있으니..)
			//hit이 있다면, Original Transform에서 Location값  - (0.f,0.f, Curve.evaluate(hit.distance / distance)
			//이렇게 해서 나온값을 NewTransform.location으로 지정해주면 된다.
			


			FVector WeaponLocation = GetActorLocation();
			FVector OwnerLocation = Main->GetActorLocation();

		}
		
		//Main->TPAnimInstance->bEndHighReady = false;
		//Main->FPAnimInstance->bEndHighReady = false;

	}

}

void AWeapon::OnCollisionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT(" Weapon Overlap End"));

	if(OwningPlayer)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OwningPlayer);
		bIsHighReady = false;
		//Main->TPAnimInstance->bBeginHighReady = false;
		//Main->FPAnimInstance->bBeginHighReady = false;
		
		Main->TPAnimInstance->bEndHighReady = true;
		Main->FPAnimInstance->bEndHighReady = true;
	}
}
*/