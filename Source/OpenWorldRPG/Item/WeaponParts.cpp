// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponParts.h"
#include "OpenWorldRPG/Item/WeaponPartsPDA.h"

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
		GetEmitEndLocation();
		GetFinalEmitEndLocation();
		//Emit();
	}
}

void AWeaponParts::SetEmitValue(bool bNeedToEmit)
{
	bEmit = bNeedToEmit;

	if ( bEmit == false )
	{
		NiagaraComp->Deactivate();
		UE_LOG(LogTemp,Warning,TEXT("AWeaponParts::SetEmitValue, Tactical DEV OFF"));
	}
	else
	{
		NiagaraComp->Activate();
		UE_LOG(LogTemp, Warning, TEXT("AWeaponParts::SetEmitValue, Tactical DEV ON"));
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
			if ( EmitHit.GetActor() )
			{
				FString HitActorName = EmitHit.Actor->GetFName().ToString();
				if ( HitActorName.Contains("Main") )
				{
					float dummyfloat1 = 23.f;
					float dummyfloat2 = 33.f;
					float DummyfloatResult = dummyfloat1+ dummyfloat2;

				}
			}

			EmitEndPosition = EmitHit.Location;			
			if ( OwnerWeapon->MainCon )
			{
				FHitResult FPSEmitHit = OwnerWeapon->BulletTrace(OwnerWeapon->MainCon->Main->CameraFPS->GetComponentLocation(), EmitEndPosition);

				if ( FPSEmitHit.IsValidBlockingHit() )
				{					
					EmitEndPosition = FPSEmitHit.Location;
				}
			}
			
		}
		else
		{
			EmitEndPosition = EmitEndVector;
		}

		//DrawDebugSphere(GetWorld(),EmitEndPosition,12.f, 5.f, FColor::Magenta, false, 5.f, 0, 3.f);
		//UE_LOG(LogTemp, Warning, TEXT("AWeaponParts::GetEmitEndLocation, Emit End Pos = %s"), *EmitEndPosition.ToString());
		
		NiagaraComp->SetVectorParameter("LaserEndVector", EmitEndPosition);
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
			FVector StraightForwardVector = NiagaraComp->GetComponentLocation() + NiagaraComp->GetComponentRotation().Vector() * PartsPDA->TacticalEmitLimitRange;

			EmitEndPosition = StraightForwardVector;

			NiagaraComp->SetVectorParameter("LaserEndVector", EmitEndPosition);
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
		NiagaraComp->Activate();	
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
			NiagaraComp = NewObject<UNiagaraComponent>(this);
			NiagaraComp->RegisterComponent();
			//NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative,false);
			NiagaraComp->AttachToComponent(this->SKMesh,Rules, "Emit");


			NiagaraComp->SetAutoActivate(false);
			NiagaraComp->SetAsset(PartsPDA->TacticalEmitEffect);
			NiagaraComp->SetColorParameter("LaserColor", PartsPDA->LaserEmitColor);//FLinearColor::Green);//);
			NiagaraComp->SetFloatParameter("LaserWidth", PartsPDA->LaserWidth);

			NiagaraComp->Deactivate();
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