// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponParts.h"
#include "OpenWorldRPG/Item/WeaponPartsPDA.h"

#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"

AWeaponParts::AWeaponParts()
{
	ItemState = EItemState::EIS_Spawn;
	OwnerWeapon_OBJ = nullptr;
	OwnerWeapon = nullptr;
	bEmit = false;

	CapsuleComp->SetCapsuleSize(1.f,1.f,false);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//NiagaraComp->SetupAttachment(SKMesh, "Emit");

	//NiagaraComp->SetupAttachment(GetRootComponent());
	//NiagaraComp->Deactivate();
}



void AWeaponParts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEmit && OwnerWeapon.IsValid())
	{
		UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
		if ( PartsPDA && PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Laser )
		{
			GetEmitEndLocation();
			GetFinalEmitEndLocation();
		}
		else if ( PartsPDA && PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Flashlight )
		{

		}
		
	}
}

void AWeaponParts::SetEmitValue(bool bNeedToEmit)
{
	bEmit = bNeedToEmit;
	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
	if(!PartsPDA ) return;

	if ( bEmit == false )
	{
		if(PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Laser)
		{
			LaserEffect->Deactivate();
			LaserEndEffect->Deactivate();
		}
		else if ( PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Flashlight )
		{
			FlashLightComp->Deactivate();
		}
		
		
		UE_LOG(LogTemp,Warning,TEXT("AWeaponParts::SetEmitValue, Tactical DEV OFF"));
	}
	else
	{
		if ( PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Laser )
		{
			LaserEffect->Activate();
			LaserEndEffect->Activate();
		}		
		else if ( PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Flashlight )
		{
			FlashLightComp->Activate();
		}
		
		UE_LOG(LogTemp, Warning, TEXT("AWeaponParts::SetEmitValue, Tactical DEV ON"));
	}
}

void AWeaponParts::SetEndPointEffectLocation(FVector Location)
{
	if ( LaserEndEffect->GetAsset() )
	{
		if ( Location == FVector(0.f) )
		{
			LaserEndEffect->SetVisibility(false);
		}
		else
		{
			LaserEndEffect->SetVisibility(true);
			LaserEndEffect->SetWorldLocation(Location);
		}
		
	}
}


void AWeaponParts::GetEmitEndLocation()
{
	if(OwnerWeapon.IsValid() == false) return;

	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
	if ( PartsPDA)// && PartsPDA->TacticalEmitEffect)
	{
		FTransform OWAimPos = OwnerWeapon->GetAimPosition();
		FVector EmitEndVector = OWAimPos.GetLocation() + OWAimPos.GetRotation().Vector() * PartsPDA->TacticalEmitLimitRange;

		FHitResult EmitHit = OwnerWeapon->BulletTrace(OWAimPos.GetLocation(), EmitEndVector);

		

		if ( EmitHit.IsValidBlockingHit() )
		{
			//for debug
			/*if ( EmitHit.GetActor() )
			{
				FString HitActorName = EmitHit.Actor->GetFName().ToString();
				if ( HitActorName.Contains("Main") )
				{
					float dummyfloat1 = 23.f;
					float dummyfloat2 = 33.f;
					float DummyfloatResult = dummyfloat1+ dummyfloat2;

				}
			}*/

			EmitEndPosition = EmitHit.Location;
			SetEndPointEffectLocation(EmitEndPosition);

			if ( OwnerWeapon->MainCon )
			{
				FHitResult FPSEmitHit = OwnerWeapon->BulletTrace(OwnerWeapon->MainCon->Main->CameraFPS->GetComponentLocation(), EmitEndPosition);

				if ( FPSEmitHit.IsValidBlockingHit() )
				{					
					EmitEndPosition = FPSEmitHit.Location;
					SetEndPointEffectLocation(EmitEndPosition);
				}
			}
			
		}
		else
		{
			SetEndPointEffectLocation(FVector(0.f));
			EmitEndPosition = EmitEndVector;
		}

		//DrawDebugSphere(GetWorld(),EmitEndPosition,12.f, 5.f, FColor::Magenta, false, 5.f, 0, 3.f);
		//UE_LOG(LogTemp, Warning, TEXT("AWeaponParts::GetEmitEndLocation, Emit End Pos = %s"), *EmitEndPosition.ToString());
		LaserEffect->SetVectorParameter("LaserEndVector", EmitEndPosition);
	}
}


void AWeaponParts::GetFinalEmitEndLocation()
{
	if ( OwnerWeapon.IsValid() )
	{
		UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
		ABaseCharacter* Bchar = Cast<ABaseCharacter>(OwnerWeapon->GetOwningPlayer());
		if(PartsPDA == nullptr || Bchar == nullptr) return;

		//Weapon이 HighReady상태거나 Weapon을 들고있는 Character의 LeftHandIK가 빠진경우엔
		if ( OwnerWeapon->bIsHighReady || Bchar->GetLeftHandIK() < 1.f || Cast<UMainAnimInstance>(Bchar->GetMesh()->GetAnimInstance())->bIsinAir )
		{
			//Emit의 End Point를 계산하지 않도록 한다.
			FVector StraightForwardVector = LaserEffect->GetComponentLocation() + LaserEffect->GetComponentRotation().Vector() * PartsPDA->TacticalEmitLimitRange;

			FHitResult EmitHit = OwnerWeapon->BulletTrace(LaserEffect->GetComponentLocation(), StraightForwardVector);
			if ( EmitHit.IsValidBlockingHit() )
			{
				EmitEndPosition = EmitHit.Location;
				SetEndPointEffectLocation(EmitEndPosition);

			}
			else
			{
				SetEndPointEffectLocation(FVector(0.f));
				EmitEndPosition = StraightForwardVector;
			}
			

			LaserEffect->SetVectorParameter("LaserEndVector", EmitEndPosition);
		}
	}
}


void AWeaponParts::Emit()
{
	if ( OwnerWeapon.IsValid() == false ) return;

	//const USkeletalMeshSocket* EmitSocket = SKMesh->GetSocketByName("Emit");
	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
	if ( PartsPDA ) //EmitSocket&&
	{	
		LaserEffect->Activate();
	}
}



void AWeaponParts::SetOwnerWeapon(AWeapon* Weapon)
{
	OwnerWeapon = Weapon;

	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);
	//const USkeletalMeshSocket* EmitSocket = SKMesh->GetSocketByName("Emit");

	if ( PartsPDA )//&& EmitSocket )
	{
		AdjustAttachLocation();

		if ( PartsPDA->WeaponPartsType == EWeaponPartsType::EWPT_Tactical )
		{
			if ( PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Laser )
			{
				LaserEffect = NewObject<UNiagaraComponent>(this);
				LaserEffect->RegisterComponent();
				//NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
				FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative,false);
				LaserEffect->AttachToComponent(this->SKMesh,Rules, "Emit");

				LaserEndEffect = NewObject<UNiagaraComponent>(this);
				LaserEndEffect->RegisterComponent();

				if ( PartsPDA->TacticalEmitEffect )
				{
					LaserEffect->SetAutoActivate(false);
					LaserEffect->SetAsset(PartsPDA->TacticalEmitEffect);
					LaserEffect->SetColorParameter("LaserColor", PartsPDA->LaserEmitColor);//FLinearColor::Green);//);
					LaserEffect->SetFloatParameter("LaserWidth", PartsPDA->LaserWidth);
					LaserEffect->Deactivate();
				}			

				if ( PartsPDA->TacticalImpactEmitEffect )
				{
					LaserEndEffect->SetAsset(PartsPDA->TacticalImpactEmitEffect);
					LaserEndEffect->SetColorParameter("LaserColor", PartsPDA->LaserEmitColor);
					LaserEndEffect->SetFloatParameter("LaserWidth", PartsPDA->LaserWidth);
					LaserEndEffect->Deactivate();
				}
			}
			else if ( PartsPDA->TacticalPartsType == ETacticalPartsType::ETPT_Flashlight )
			{
				FlashLightComp = NewObject<USpotLightComponent>(this);
				FlashLightComp->RegisterComponent();
				FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
				FlashLightComp->AttachToComponent(this->SKMesh, Rules, "Emit");

				FlashLightComp->SetInnerConeAngle(PartsPDA->Flashlight_InnerConeRadius);
				FlashLightComp->SetOuterConeAngle(PartsPDA->Flashlight_OuterConeRadius);
				FlashLightComp->SetAttenuationRadius(PartsPDA->Flashlight_Distance);
				FlashLightComp->SetIntensity(PartsPDA->Flashlight_Brightness);
				FlashLightComp->bAffectsWorld = true;
				FlashLightComp->SetCastShadows(true);
				FlashLightComp->SetLightColor(FLinearColor::White);
				FlashLightComp->SetIntensityUnits(ELightUnits::Lumens);
				FlashLightComp->SetLightBrightness(500000.f);

			}

		}

	}
	
}

void AWeaponParts::AdjustAttachLocation()
{
	if ( OwnerWeapon.IsValid() && OwnerWeapon->WeaponDataAsset )
	{
		UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(ItemSetting.DataAsset);

		switch ( OwnerWeapon->WeaponDataAsset->WeaponName )
		{
			case EWeaponName::EWN_M4:
				this->SetActorRelativeLocation(PartsPDA->AttachLocForM4);
			break;
			case EWeaponName::EWN_AK:
				this->SetActorRelativeLocation(PartsPDA->AttachLocForAK);
			break;
			case EWeaponName::EWN_M9:
				this->SetActorRelativeLocation(PartsPDA->AttachLocForM9);
			break;
		}
	}
	
	
	
}