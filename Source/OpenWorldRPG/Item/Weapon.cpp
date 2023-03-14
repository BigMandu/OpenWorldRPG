// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
//#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/MainAnimInstance.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"

#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Curves/CurveFloat.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h" //?遺얠쒔繹???
//#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


#define WeaponDEBUG 0

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
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CheckWeaponPartsManager();
	UpdateWeaponParts();
}

bool AWeapon::StepEquip(AActor* Char, ERifleSlot RifleSlot)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Char);
	check(BChar)

	//筌??類λ립 揶?????苡??????由??袁る? Cast??紐??
	WeaponDataAsset = Cast<UWeaponPDA>(ItemSetting.DataAsset);
	check(WeaponDataAsset)

	CapsuleAdjust();
	
	
	if (RifleAssign == ERifleSlot::ERS_MAX)
	{
		if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
		{
			/* 1,2,3??????袁⑤르 Quick Swap??由??袁る? Rifle??筌??類λ립??*/
			if (RifleSlot == ERifleSlot::ERS_MAX) //筌??類ｋ? Slot???????
			{
				if (BChar->PrimaryWeapon) //??? 雅?겆€묾怨? ???筌?
				{
					BChar->SetWeaponAssign(this,ERifleSlot::ERS_Sub);
					//SettingRifleAssign(BChar, ERifleSlot::ERS_Sub);
				}
				else //雅?겆€묾怨? ???筌?
				{
					BChar->SetWeaponAssign(this, ERifleSlot::ERS_Primary);
					//SettingRifleAssign(BChar, ERifleSlot::ERS_Primary);
				}
			}
			//Drag&Drop??곗? Equip??筌袁る????? ?袁⑥ ?브쑨由경에????彛??
			else
			{
				//RifleAssign = RifleSlot;
				//SettingRifleAssign(BChar, RifleSlot);
				BChar->SetWeaponAssign(this, RifleSlot);

			}
		}
		else if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol)
		{
			BChar->SetWeaponAssign(this,ERifleSlot::ERS_Pistol);
		}
	}

	//??블???? ?얜용┛揶? ????野???筌?疫?Weapon????삳?嚥????.
	if (BChar->EquippedWeapon == nullptr)
	{
		if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
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
	}
	//??블??? ?얜용┛揶? ??? 野??? 筌?疫?Weapon??SubSocket??곗? ??由??
	else
	{
		GunAttachToSubSocket(BChar);
	}
	
	//Fire???④쑴沅???紐
????由??袁る? 揶???沃紐???紐
?????.
	WeaponDataAsset->WeaponStat.FireRatePerSec = WeaponDataAsset->WeaponStat.FireRatePerMin / 60;
	WeaponDataAsset->WeaponStat.SecondPerBullet = 1 / WeaponDataAsset->WeaponStat.FireRatePerSec; //0.06
	
	
	//筌?ㅻ??紐
?????멸돌??AddEquipment???紐???由??袁る? ??夷???紐????.
	Super::StepEquip(Char);

	/* ?袁⑥??AddEquipment ?????筌袁る???鍮????λ??쇱??紐??.
	* ?袁⑥??λ??? ItemObj揶? ?袁⑹??
	*/
	UCustomInventoryLibrary::SetWeaponPartsManager(this,this->ItemObj);
	UpdateWeaponParts();

	/**?關媛?筌怨??Ammo Widget??update??由??袁る?
	 * CheckAmmo嚥?obj??AmmoInMag 癰???? ?븍???블?broadcast?????.
	 */
	if (OwningPlayer)
	{
		CheckAmmo();
		OwningPlayer->OnGetAmmo.Broadcast(this);
	}


	return true;
}


/*
*  Weapon??WeaponGrip Socket???關媛???.
*/
void AWeapon::GunAttachToMesh(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	AMainCharacter* Main = Cast<AMainCharacter>(BChar);

	const USkeletalMeshSocket* AttachSocket = nullptr;
	FTransform AttachTransform;
	

	// 1, 3?紐臾?癰?野???weapon attach??癰?野?釉?? (Main?癒? ?怨몄)
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
					if(WeaponDataAsset)
					{
						SetActorRelativeTransform(WeaponDataAsset->FPMeshAttachTransform);
					}
				}
				break;
			case ECameraMode::ECM_TPS:
				if (TPSocket->AttachActor(this, Main->GetMesh()))
				{
					if (WeaponDataAsset)
					{
						SetActorRelativeTransform(WeaponDataAsset->MeshAttachTransform);
					}
				}
				break;
			}

			if(SKMesh)
			{
				SKMesh->SetHiddenInGame(false);
			}
			//Main->SetEquippedWeapon(this);
		}
	}

	//BChar筌?null???袁⑤뜹???-> AI?癒? ????
	if (BChar && Main == nullptr)
	{
		const USkeletalMeshSocket* WeaponSocket = BChar->GetMesh()->GetSocketByName("WeaponGrip");
		if (WeaponSocket)
		{
			if (WeaponSocket->AttachActor(this, BChar->GetMesh()))
			{
				if (WeaponDataAsset)
				{
					SetActorRelativeTransform(WeaponDataAsset->MeshAttachTransform);
				}
				
				//SKMesh->SetHiddenInGame(false);
				//BChar->SetEquippedWeapon(this);
			}
		}

	}
}


/*
* WeaponGrip???袁⑤?AttachSocket??????Weapon???봔筌???沅??
*/
void AWeapon::GunAttachToSubSocket(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	if(BChar == nullptr) return;

	const USkeletalMeshSocket* AttachSocket = nullptr;
	FTransform AttachTransform;

	if(WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (RifleAssign)
		{
		case ERifleSlot::ERS_Primary :
			AttachSocket = BChar->GetMesh()->GetSocketByName("PrimaryWeaponAttachSocket");
			AttachTransform = WeaponDataAsset->PrimaryWeaponAttachTransform;
		break;
		case ERifleSlot::ERS_Sub:
			AttachSocket = BChar->GetMesh()->GetSocketByName("SubWeaponAttachSocket");
			AttachTransform = WeaponDataAsset->SubWeaponAttachTransform;
		break;
		}
	}
	else if (WeaponDataAsset->EquipmentType == EEquipmentType::EET_Pistol)
	{
		AttachSocket = BChar->GetMesh()->GetSocketByName("PistolAttachSocket");
		AttachTransform = WeaponDataAsset->PistolAttachTransform;
	}


	if (AttachSocket)
	{
		AttachSocket->AttachActor(this, BChar->GetMesh());
		SetActorRelativeTransform(AttachTransform);

		if (WeaponDataAsset->EquippedSound)
		{
			UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->EquippedSound, OwningPlayer->GetRootComponent());
		}
	}

	
}

FTransform AWeapon::GetSightSocketTransform()
{
	//if have any sight device. return that location.

	//Socket???袁⑺????곕┛ ?袁⑸?, Weapon???袁⑺??揶類????.
	//Animation????鍮 Attach??Weapon???袁⑺揶? 獄遺???嚥?揶類???肉?Socket?????????? ?袁⑺???귐苑??由??袁る맙.
	//SetActorRelativeTransform(WeaponDataAsset->FPMeshAttachTransform);

	FTransform ReturnTransform;

	
	if(AEquipment* A_Scope = WeaponPartsManager->GetWeaponParts(EWeaponPartsType::EWPT_Scope))
	{
		ReturnTransform = A_Scope->SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
	}
	else
	{
		ReturnTransform = SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_World);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / Sight Socket World : %s"), *ReturnTransform.GetLocation().ToString())
	//FTransform TestTF = SKMesh->GetSocketTransform(SightSocketName, ERelativeTransformSpace::RTS_ParentBoneSpace);
	/*FVector DebugLocation = TestTF.GetLocation() + GetActorLocation();
	DrawDebugSphere(GetWorld(), DebugLocation, 6.f,6,FColor::Blue,false,10.f,0,3.f);
	
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / Sight Socket World : %s"), *ReturnTransform.GetLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::GetSightSocketTransform / TEST TF : %s"), *DebugLocation.ToString());
	*/

	return ReturnTransform;
}

//?얜?椰???밴쉐???.
void AWeapon::CheckWeaponPartsManager()
{
	if(WeaponPartsManager.IsValid() == false)
	{
		WeaponPartsManager = NewObject<UWeaponPartsManagerObject>();
		WeaponPartsManager->SetOwnerWeapon(this);
		WeaponPartsManager->OnChangeParts.AddDynamic(this, &AWeapon::UpdateWeaponParts);
	}
	
}

void AWeapon::UpdateWeaponParts()
{
	if(ItemObj && ItemObj->WeaponPartsManager.IsValid())
	{
		ItemObj->WeaponPartsManager->UpdateParts(GetWorld(),this);
	}
	else
	{
		WeaponPartsManager->UpdateParts(GetWorld(), this);
	}
}


void AWeapon::Remove()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove func called"));
	Super::Remove();
	if (OwningPlayer == nullptr) return;
	
	RifleAssign = ERifleSlot::ERS_MAX;
	if (ItemObj)
	{
		ItemObj->RifleAssign = ERifleSlot::ERS_MAX;
	}

	if (OwningPlayer->EquippedWeapon == this)
	{
		OwningPlayer->ChangeWeapon(0);
		OwningPlayer->EquippedWeapon = nullptr;
		//Main->SetEquippedWeapon(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove , Remove RifleAssign"));
	}


	if (OwningPlayer->PrimaryWeapon == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Primary..."));
		OwningPlayer->PrimaryWeapon = nullptr;
	}
	else if (OwningPlayer->SubWeapon == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::Remove, was Sub..."));
		OwningPlayer->SubWeapon = nullptr;
	}
	else if (OwningPlayer->PistolWeapon == this)
	{
		OwningPlayer->PistolWeapon = nullptr;
	}

	OwningPlayer = nullptr;
	if(WeaponPartsManager.IsValid())
	{
		WeaponPartsManager->DestroyAllAttachParts(this);
	}
	
	//Destory ??λ嚥?Attach??苡?Destory??由븝??紐????鍮??

	Destroy();

}


void AWeapon::CapsuleAdjust()
{
	CapsuleComp->SetRelativeTransform(WeaponDataAsset->CapsuleComponentTF);
	CapsuleComp->SetCapsuleHalfHeight(WeaponDataAsset->CapsuleHalfHeight);
	CapsuleComp->SetCapsuleRadius(WeaponDataAsset->CapsuleRadius);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
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
	//UCustomPDA* CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
	if (WeaponDataAsset && WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		switch (WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_Safe:
			WeaponFiringMode = EWeaponFiringMode::EWFM_SemiAuto;
			break;
		case EWeaponFiringMode::EWFM_SemiAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Burst;
			break;
		case EWeaponFiringMode::EWFM_Burst:
			WeaponFiringMode = EWeaponFiringMode::EWFM_FullAuto;
			break;
		case EWeaponFiringMode::EWFM_FullAuto:
			WeaponFiringMode = EWeaponFiringMode::EWFM_Safe;
			break;
		}
	}
}

void AWeapon::ChangeSafetyLeverForAI(EWeaponFiringMode FiringMode)
{
	if (WeaponDataAsset && WeaponDataAsset->EquipmentType == EEquipmentType::EET_Rifle)
	{
		WeaponFiringMode = FiringMode;
	}
}


void AWeapon::TempNewWeaponState()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::TempNewWeaponState"));
	EWeaponState State = EWeaponState::EWS_Idle;

	if (bLMBDown)
	{
		if (CanFire())//獄?沅?????????筌? Firing??곗? ?怨밴묶??癰?野?釉??
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("TempState -> Firing"));
#endif
			State = EWeaponState::EWS_Firing;
		}
	}
	
	//???袁⑸???館釉?State??Setting??由??袁る? SetWeaponState??λ???紐????.
	SetWeaponState(State);
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::SetWeaponState"));
	//疫꿸????獄?沅餓λ?????? ?遺우??LMB???袁ⓥ
ㅿ?? ??
???삠
	if (CurrentWeaponState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		// ?癒??筌???멸땀??筌??類ｋ? 筌?而???湲???由??겹 ??爰??猷嚥???
		bool bCanEndFire = true;
		if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Burst mode Continuous Firing"));
#endif			
			bCanEndFire = false;

		}


		//??爰??餓λ????堉???醫釉?EndFiring?紐??
		 //?癒????곕르????
紐??> Check Firing??λ?癒?? Firing???紐???猷嚥???
		if (bCanEndFire)
		{
#if WeaponDEBUG
			UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Call EndFiring func"));
#endif			
			CurrentWeaponState = NewState;
			EndFiring(); 
			
		}
		
	}
	//疫꿸????獄?沅??? ??釉?? LMB???????삠
	else if (CurrentWeaponState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{

#if WeaponDEBUG
		UE_LOG(LogTemp, Warning, TEXT("SetWeponState: Start Firing Call ControlFiring func"));
#endif			
		//??爰????????.
		CurrentWeaponState = NewState;
		ControlFiring();	
	}
}


	/*
	* ??????? 鈺곌?援?
	* 1. 鈺곌??揶???????袁⑤??鍮???.
	* 2. ?關?얌빳? ??爰썰빳臾? ?袁⑤??鍮 ???.
	* 3. ?袁⑸?????而??湲???堉?????.
	*
	* 4. ?關媛餓λ?? ?袁⑤??鍮 ???. //沃硫???
	* 5. ??쎈늄?깃???餓λ????袁⑤??鍮 ???. //沃硫???
	*/
bool AWeapon::CanFire()
{
	bool bCanFire = false;
	
	
	//if (ItemObj->bIsDestoryed == false)
	{
		if (WeaponFiringMode != EWeaponFiringMode::EWFM_Safe && CurrentWeaponState != EWeaponState::EWS_Reloading
		&& OwningPlayer->bIsSprinting == false)
		{
			if (!(WeaponFiringMode == EWeaponFiringMode::EWFM_SemiAuto && FireCount > 0))
			{
				if(CheckAmmo())
				{	
					bCanFire = true;					
				}
				else
				{
					//?源? ??? Sound ?곕떽???由?
				}
			}
		}
	}
	return bCanFire;
}

int32 AWeapon::GetCurrentAmmoInMag()
{
	return AmmoLeftInMag;
}

bool AWeapon::CheckAmmo()
{
	if (ItemObj)
	{
		AmmoLeftInMag = ItemObj->GetAmmoLeftInMag();
	}

	if (AmmoLeftInMag <= 0)
	{	
		// -1??釉嚥?????????? ???筌?獄????뱀嚥?
		AmmoLeftInMag = 0;
		return false;
	}
	return true;
}

void AWeapon::UseAmmo()
{
	--AmmoLeftInMag;
	if (ItemObj)
	{
		ItemObj->SetAmmoLeftInMag(AmmoLeftInMag);
	}

	OwningPlayer->OnGetAmmo.Broadcast(this);
}


void AWeapon::Reload()
{
	SetWeaponState(EWeaponState::EWS_Reloading);
	bool bHasAmmo = OwningPlayer->CheckAmmo();
	CntAmmoSameType = 0;
	if(bHasAmmo)
	{
		//Current ?酉源???關??揶??館釉??袁⑸????酉釉??
		AmmoLeftInMag += OwningPlayer->GetNumberofCanReload();

		//?遺용연 ammo????
???? ???.
		CntAmmoSameType = OwningPlayer->GetTotalNumberofSameTypeAmmo();
		if (ItemObj)
		{
			ItemObj->SetAmmoLeftInMag(AmmoLeftInMag);
		}

	}
	else
	{
		//Ammo揶? ??용筌?reload ??쎈?
	}

	OwningPlayer->OnGetAmmo.Broadcast(this);
}

void AWeapon::ReloadEnd()
{
	SetWeaponState(EWeaponState::EWS_Idle);


	//AmmoLeftInMag = WeaponDataAsset->WeaponStat.AmmoPerMag;
	//ConsumeAmmoCnt = 0;
}


bool AWeapon::CanEndFire()
{
	return false;
}


   /* FiringMode???怨??Delay??餓μ鍮??
	* semiauto??野???toggle獄????곗?.
	* burst??野???3獄???爰?
	* Fullauto??野????怨???爰?
	*
	* ??由??timer筌ｋ寃????苡???竊????????紐???? ??λ?????癒?뻣嚥????????
	*/
void AWeapon::StartFire()
{
	
	if (!bLMBDown)
	{
		bLMBDown = true;
		/*
		?猷뱀??????袁⑤르 疫꿸????Shot?袁⑤르?귐??????遺?, idling?怨밴묶???遺? 筌ｋ寃??鍮??
		?猷뱀????????? 餓λ?????筌??④쑴???苑 ?⑤????λ ?紐????욧?筌癒????삘
???λ)
		?袁⑤뀐? ?⑤???筌?????λ?紐???鍮??(獄???λ 筌癒????삘
ⓨ?
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
	/* SetWeaponState?癒?? ?紐????.
	* 
	* Timer?????? Firing??λ???紐????.
	* Timer????而??
	* (筌??筌?獄?沅???而?? 獄?沅?醫釉???遺얜굡 ????
	* 筌??筌?獄?沅??而 + 獄?沅揶袁㏐봄 ????= 獄?沅?揶?????而 ?????.
	* 域밸??? ??獄?沅?揶??關?揶袁⑹ ?遺얜굡 ???袁⑤??????獄?沅????????용.
	* ??쇱?獄?沅???而??
	* ??獄?沅揶??關?揶袁⑸? ?遺얜굡???袁⑹??????닌鍮筌袁??
	* 
	* ??? ??쇰선, 1?λ肉?????2?λ??揶袁㏐??揶?筌袁?筌?
	* 3?λ???遺얜굡???袁⑸? ???????.
	* 3?λ釉???????????袁⑹???遺얜굡???袁⑹ 2?λ?わ????????용.
	* 
	* ??쇱?獄?沅??而?袁⑹????삠
	* 3?λ釉?????쇱?遺얜? ?袁⑹??遺얜굡???袁⑹?2?λ? ????
	* 1?λ? ??ㅻ?? 1?λ???3?λ? ??堉????????.
	* ??? ??뱀嚥???????????.	
	*/

#if WeaponDEBUG
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::ControlFiring"));
#endif			
	
	float WorldTime = GetWorld()->GetTimeSeconds();
	
	// ?癒?? 筌?ㅻ??곕르??揶類?ｆ에???而?????곕떽????. 
	// Enum????ㅻ? ???筌?if?얜??????疫뀀?堉?紐苑 boolean癰???? ??援??곕떽???援
??
	bool bIsBurstmode = false;
	if (WeaponFiringMode == EWeaponFiringMode::EWFM_Burst)
	{
		bIsBurstmode = true;
	}

	//筌??筌?獄?沅??而???μ??SPB (獄?沅???袁⑹???????酉釉?揶???worldTime癰??????筌?
	// ?袁⑹
 獄?沅????????沃?嚥?獄?沅揶?????而???닌釉??

	//?癒??筌?ㅻ굡筌?獄?沅?揶?????而??????鈺곌?援??곗? 野???釉??
	if((bIsBurstmode && LastFireTime > 0 && LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet *4 >WorldTime) ||
		(LastFireTime > 0 && LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet > WorldTime))
	{
		//獄?沅?揶??關?揶袁⑹??닌釉?? 
		float RemainingTime = 0.f;// = LastFireTime + WeaponStat.SecondPerBullet - WorldTime;

		if (bIsBurstmode) 
		{
			//?癒?? 筌?ㅻ굡筌?SecondPerBullet??4???④?釉?揶?
			RemainingTime = (LastFireTime + WeaponDataAsset->WeaponStat.SecondPerBullet * 4) - WorldTime;
		}
		GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::Firing, RemainingTime, false);
		
	}
	else
	{
		//獄?沅揶? 揶??館釉??삠 獄遺얠??紐????.
		Firing();
	}
	
}

void AWeapon::Firing()
{
	/* ControlFiring, ReFiring?癒?? ?紐??? */
	/*
		ammo 筌ｋ寃? FireCount 筌??, ??λ ??밸? Timer疫꿸?以?(筌??筌?獄?沅???而 疫꿸?以??
		?④쑴???苑 ??爰썰빳臾???겹 ReFiring??λ ?紐??
	*/

#if WeaponDEBUG
	UE_LOG(LogTemp, Warning, TEXT("Firing function"));
#endif			
	bIsFiring = true;
	AMainCharacter* Main = Cast<AMainCharacter>(GetInstigator());

	if (bDetectLookInput == false)
	{		
		if (Main)
		{
			if (Main->bIsLookInput)
			{
				bDetectLookInput = true;
			}
		}
	}

	/* 筌?獄?沅筌?ControlRotation揶??????館釉?? -> ??爰??ル
利???癒???由??袁る맙 */
	if (FireCount == 0)
	{
		StartFiringRotation = GetInstigatorController()->GetControlRotation();
		//UE_LOG(LogTemp, Warning, TEXT("Save Start Firing Rotation"));
		//UE_LOG(LogTemp, Warning, TEXT("Start Rotating val : %s"), *StartFiringRotation.ToString());
	}

	
	if(Main)
	{
		//for player
		New_BulletOut(); //Weapon Instant???닌???
	}
	else
	{
		//for ai
		AIBulletOut(); //Weapon Instant???닌???
	}


	WeaponFX();
	UseAmmo();
	FireCount++;
	
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::Firing/  Fire cnt : %d"), FireCount);
	//UE_LOG(LogTemp, Warning, TEXT("LastFiretime : %f"), LastFireTime);
	
	GetWorldTimerManager().SetTimer(FiringTimer, this, &AWeapon::ReFiring, WeaponDataAsset->WeaponStat.SecondPerBullet, false);
	LastFireTime = GetWorld()->GetTimeSeconds();
}

//ReFiring?????????筌? ?類ㅼ??? ??λ??
void AWeapon::ReFiring()
{
	//??쇱ｆ?筌ｋ寃??? ??λ???紐????.
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

// Firing????멸돌筌?揶怨몄?癰???諭???λ由?????餓Β??
void AWeapon::EndFiring()
{
	//UE_LOG(LogTemp, Warning, TEXT("AWeapon::EndFiring"));
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FiringTimer);
	
	FireCount = 0;
	CurrentWeaponState = EWeaponState::EWS_Idle; //Burst mode???袁る맙
	PreviousSpread = FVector::ZeroVector;

	RecoilTime = 0.f;

	// ??爰????멸??袁⑤르 筌???爰??癒???곗? ??猷????삳 疫꿸???,,, test???袁る? ?醫? 疫꿸????off.
	//AimInitialize();
}

//Refire揶? 揶??館釉놂?? 筌ｋ寃???.
bool AWeapon::CheckRefire()
{
	bool bFlag = false;
	//野?몄??揶??館釉놂?? ?怨苑 筌ｋ寃????
	if (CanFire())
	{
		if (CurrentWeaponState == EWeaponState::EWS_Firing)
		{
			//Burst筌?ㅻ??곕르??BurstRound??釉嚥??????refire揶? 揶??館釉??
			switch (WeaponFiringMode)
			{
			case EWeaponFiringMode::EWFM_Burst:
				if (FireCount > 0 && FireCount < WeaponDataAsset->WeaponStat.BurstRound)
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

// ???
??堉?????揶怨룸?(?怨猷?硫? 獄遺?よ??욱???? ???얍???Vector揶??嚥??닌釉??
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

/* ????λ?癒?? ?닌釉?揶怨???????嚥≪??????????甕겸リ???夷??筌ㅼ伊?????? ?袁⑺???닌釉??*/
FVector AWeapon::GetTraceStartLocation(FVector Dir)
{
	check(OwningPlayer)
	check(MainCon)

	FVector ReturnLocation;
	FRotator Rot;

	//ClippingWall ??λ揶? ??쎈뻬???, ????? ?袁⑺??Muzzle???袁⑺嚥?獄遺???
	//??λ揶? ??쎈뻬??? ???筌?  ?袁⑸???袁⑺??Actor??(Actor-CamLo = Actor?????燁살? ???) Dir?????揶怨몄 ???.
	
	//For debug
	if (SKMesh->GetSocketByName(MuzzleFlashSocketName) == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("AWeapon::GetTraceStartLocation / Can not find MuzzleFlashSocket"));
	}

	if(bIsHighReady)
	{
		FVector WeaponMuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
		ReturnLocation = WeaponMuzzleLocation; //GetActorLocation();
	}
	else if(MainCon)
	{
		MainCon->GetPlayerViewPoint(ReturnLocation, Rot);
		ReturnLocation = ReturnLocation + Dir * (OwningPlayer->GetActorLocation() - ReturnLocation | Dir);
	}
	
	return ReturnLocation;
}

FVector AWeapon::GetTraceEndLocation(FVector StartVector, FVector Dir)
{
	FVector ReturnVec;

	//ClippingWall??λ揶? ??쎈뻬??? ?袁⑹??ル
利??袁⑺??Muzzle??獄?븍?椰怨?揶? ???
	//??λ揶? ??쎈뻬??? ???筌? WorldAimPosition?????.
	if(bIsHighReady)
	{
		//Muzzle Flash Rotation揶??嚥???? 筌ｌ??? ??由?遺얜? ??而 筌???? 癰???Rotation揶?????湲??苡?????? ?癒?? 癰????. -> ActorQuat ??? ?곕벡?嚥?癰???釉?
		//FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);
		//ReturnVec = StartVector +  MuzzleRotation.Vector() * WeaponDataAsset->WeaponStat.WeaponRange;
		 
		//??Actor Mesh???袁④? ?곕벡??揶??紐???
		ReturnVec = StartVector + this->GetActorQuat().GetAxisY() * WeaponDataAsset->WeaponStat.WeaponRange;
		
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

	FCollisionQueryParams params(NAME_None, true, GetInstigator()); //Instigator??IgnoreActor嚥???????.

	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON_INST, params);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green,false,2.f,(uint8)nullptr,2.f);

	return Hit;
}

/* ?????? ?μ???????*/
void AWeapon::WeaponFX()
{
	//?????
	if (WeaponDataAsset->FireSound)
	{
		UGameplayStatics::SpawnSoundAttached(WeaponDataAsset->FireSound, OwningPlayer->GetRootComponent());
	}

	//?μ???????
	if (WeaponDataAsset->FireMuzzleEffect)
	{
		const USkeletalMeshSocket* MuzzleSocket = SKMesh->GetSocketByName(MuzzleFlashSocketName);
		
		if (MuzzleSocket)
		{
			FVector MuzzleLocation = SKMesh->GetSocketLocation(MuzzleFlashSocketName);
			FRotator MuzzleRotation = SKMesh->GetSocketRotation(MuzzleFlashSocketName);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponDataAsset->FireMuzzleEffect, MuzzleLocation, MuzzleRotation);
		}
	}

	//Animatoin ??源?
	//PlayWeaponAnimAndCamShake(FireAnimaton);
}

/*
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
*/

void AWeapon::AimInitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::AimInitialize"));
	//UE_LOG(LogTemp, Warning, TEXT("Save End Firing Location And Init"));
	EndFiringRotation = GetInstigatorController()->GetControlRotation();

	//fire餓?筌??????
???揶癒???? AimInit????λ???袁⑥嚥????野????.
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
		AlphaTime = Time / 0.8f; // : Time/??猷?袁⑹????而  (??쎈?

		//FRotator LerpAimRotation = FMath::RInterpTo(EndFiringRotation, StartFiringRotation, GetWorld()->GetDeltaSeconds(), 20.f);
		FRotator LerpAimRotation = FMath::Lerp(EndFiringRotation, StartFiringRotation, AlphaTime);
		GetInstigatorController()->SetControlRotation(LerpAimRotation);

		//UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpAimRotation.ToString());
		},GetWorld()->GetDeltaSeconds(),true);
}

void AWeapon::UpdateAim()
{
	FTransform AimPos = GetAimPosition();
	FVector EndVec = AimPos.GetLocation() + AimPos.GetRotation().Vector() * 3000.f;
	FHitResult Hit = BulletTrace(AimPos.GetLocation(), EndVec);


	if (Hit.bBlockingHit)
	{
		WorldAimPosition = Hit.Location;

		//FPS????癒?? Trace??揶???⑤
밸???苡?????苑 筌??筌?筌?? ?袁⑺嚥?AimPos????由??
		FHitResult WeaponHit = BulletTrace(MainCon->Main->CameraFPS->GetComponentLocation(), Hit.Location);
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

	//DrawDebugPoint(GetWorld(), WorldAimPosition, 10.f, FColor::Green, false, -1.f);

}

void AWeapon::WeaponClipping()
{
	FTransform CurrentMeshAttachTransform;
	FVector CurrentMeshRightHand;
	if (ECameraMode::ECM_FPS == MainCon->Main->CameraMode)
	{
		CurrentMeshAttachTransform = WeaponDataAsset->FPMeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->FPMesh->GetSocketLocation("Hand_R");

	}
	else 
	{
		CurrentMeshAttachTransform = WeaponDataAsset->MeshAttachTransform;
		CurrentMeshRightHand = MainCon->Main->GetMesh()->GetSocketLocation("hand_r");
	}

	//LienTrace??疫뀀????Weapon???봔筌△몃?CapsuleComp疫뀀???
	float Length = CapsuleComp->GetScaledCapsuleHalfHeight() * 4;

	//獄?븍?린?リ숲??AimPosition??곗? ???.
	FVector EndLo = CurrentMeshRightHand + GetAimPosition().GetRotation().GetForwardVector() * Length;
	TArray<AActor*> IgnoreActorList;
	IgnoreActorList.Add(GetInstigator());
	FHitResult Hit;

	ETraceTypeQuery TTQ = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	//??삘
⑨??癒????Aim???館釉??獄?븍??곗? Weapon??Capsule疫뀀??筌?곌껍 LineTrace?????.
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CurrentMeshRightHand, EndLo, 5.f, TTQ, false,
		IgnoreActorList, EDrawDebugTrace::None, Hit, true);
	
	//??Line Trace揶? Hit??? ??肉 ?얜堉드?? ?關釉룩?깆 ???????댁??
	//Weapon??Roll????????袁⒲??곗? ?????沅?? 'HighReady?類κ묶嚥?
	if(Hit.bBlockingHit)
	{
		FVector CurVec = CurrentMeshAttachTransform.GetTranslation();
		//FVector CalcVec = CurrentMeshAttachTransform.GetTranslation() - (NewY * 50.f);
		
		FRotator CurRot = CurrentMeshAttachTransform.GetRotation().Rotator();

		//Why Roll?? -> Skeleton?癒?? ?얜용┛???節???袁⑥? ?????Roll揶?彛?獄遺쎈??Roll嚥???援
??
		// Hit.Distance / Length??苡??? 0?봔??1??????遺얜?
		// Hit.Distance == Length -> 1??留? 筌? 1??곕르??0??由뷸?0??곕르??90?袁? ??堉??노맙.
		// ?????90???④?六썸? 90??椰??④?鍮甕곌쑬?筌?1??곕르 90????苡?? 域밸??????媛??揶怨룸?揶? -90????鍮 ?袁⑥? ???ゅ????
		// 0*90 = 0, 0.5*90 = 45, 1*90 = 90????椰?-90????곗?紐??
		// 筌ㅼ伊揶誘? 0??곕르 -90, 0.5??곕르 -45, 1??곕르 0?????.
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

	//UE_LOG(LogTemp,Warning,TEXT("high Ready is %s"),bIsHighReady ? "true" : "false");
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
			//Weaponclass????? bIsHighReady??AnimInstance?癒?? ?袁⑹??????遺우??false?????雅?곕?嚥????.
			bIsHighReady = true;
			//Main->TPAnimInstance->bBeginHighReady = true;
			//Main->FPAnimInstance->bBeginHighReady = true;

			// New Fix Clipping wall
			//Tick?癒?? Sphere Line Trace??????? distance?? radius????쇱 揶??館釉?(?μ몄????????????.)
			//hit?????筌? Original Transform?癒?? Location揶? - (0.f,0.f, Curve.evaluate(hit.distance / distance)
			//???野???苑 ???⒴????NewTransform.location??곗? 筌??類λ?雅??????.
			


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
