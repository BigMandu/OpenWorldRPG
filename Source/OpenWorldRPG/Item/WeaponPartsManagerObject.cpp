// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "OpenWorldRPG/Item/WeaponPartsPDA.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

void UWeaponPartsManagerObject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar << MuzzleParts;
	Ar << ScopeParts;
	Ar << TacticalParts;
}

void UWeaponPartsManagerObject::SetOwnerWeapon(AWeapon* Weapon)
{
	OwnerWeapon = Weapon;
}

AWeapon* UWeaponPartsManagerObject::GetOwnerWeapon()
{
	AWeapon* ReturnWeap = nullptr;
	if ( OwnerWeapon )
	{
		ReturnWeap = OwnerWeapon;
	}

	return ReturnWeap;
}

void UWeaponPartsManagerObject::AddParts(UNewItemObject* Parts)
{
	if ( Parts->ItemInfo.DataAsset == nullptr ) return;
	bool bIsSuccess = false;
	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(Parts->ItemInfo.DataAsset);
	if ( PartsPDA )
	{
		switch ( PartsPDA->WeaponPartsType )
		{
		case EWeaponPartsType::EWPT_Muzzle:
			if ( MuzzleParts == nullptr)
			{
				MuzzleParts = Parts;
				bIsSuccess = true;
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if ( ScopeParts== nullptr)
			{
				ScopeParts = Parts;
				//ScopeParts->WeaponPartsManager = this;
				bIsSuccess = true;
			}
			break;
		case EWeaponPartsType::EWPT_Tactical:
			if ( TacticalParts == nullptr )
			{
				TacticalParts = Parts;
				//TacticalParts->WeaponPartsManager = this;
				bIsSuccess = true;
			}
			break;
		}

		//추가할 수 있다면, 기존에 존재했던곳에서 remove한뒤 이 ManagerObj를 추가한다.
		if ( bIsSuccess )
		{
			DeleteLink(Parts);
			Parts->WeaponPartsManager = this;
		}
	}
	/**WeaponPartsWidget, Weapon에서 Bind된다
	 * WeaponPartsWidget의 Widget을 update하고
	 * Weapon에서 이 class의 함수인 UpdateParts를 호출한다.
	 */
	OnChangeParts.Broadcast();
}

void UWeaponPartsManagerObject::RemoveParts(UNewItemObject* Parts)
{
	if ( Parts->ItemInfo.DataAsset == nullptr ) return;
	bool bSuccessRemove = false;
	UWeaponPartsPDA* PartsPDA = Cast<UWeaponPartsPDA>(Parts->ItemInfo.DataAsset);
	if ( PartsPDA )
	{
		if(PartsPDA->EquipmentType != EEquipmentType::EET_WeaponParts) return;
		switch ( PartsPDA->WeaponPartsType )
		{
		case EWeaponPartsType::EWPT_Muzzle:
			if ( MuzzleParts )
			{
				MuzzleParts = nullptr;
				A_MuzzleParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if ( ScopeParts)//.IsValid() && ScopeParts.Get() == Parts )
			{
				ScopeParts = nullptr;
				A_ScopeParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Tactical:
			if ( TacticalParts)
			{
				TacticalParts = nullptr;
				A_TacticalParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		}

		if ( bSuccessRemove )
		{
			Parts->WeaponPartsManager = nullptr;
		}

	}

	OnChangeParts.Broadcast();
}

void UWeaponPartsManagerObject::DeleteLink(UNewItemObject* PartsObj)
{
	if ( PartsObj->WeaponPartsManager )
	{
		PartsObj->WeaponPartsManager->RemoveParts(PartsObj);
		PartsObj->WeaponPartsManager = nullptr;
	}
	else if ( PartsObj->MotherStorage )
	{
		//PartsObj->GetMotherStorage()->RemoveItem(PartsObj);
		PartsObj->MotherStorage->RemoveItem(PartsObj);
		PartsObj->MotherStorage = nullptr;
	}
}




/*WeaponPDA의 각 파츠는 null또는 data를 갖고이 ㅆ을 수 있다.
*
* 이 UpdateParts 함수는 WeaponPDA의 Parts들이 null이나 Data를 받았을때 마다 호출된다.
* 따라서, 이 함수가 호출 될 때마다
* 모든 parts들을 Remove시키고 Spawn해서 Attach시켜야 한다.
* */
void UWeaponPartsManagerObject::UpdateParts(UWorld* World, AWeapon* VarWeapon)
{
	//if(VarWeapon == nullptr) return;
	if ( OwnerWeapon == nullptr ) return;
	if ( OwnerWeapon != VarWeapon ) return;
	//OwnerWeapon = VarWeapon;

	if ( OwnerWeapon->WeaponDataAsset == nullptr ) return;
	DestroyAllAttachParts(OwnerWeapon);

	for ( EWeaponPartsType PartsType : TEnumRange<EWeaponPartsType>() )
	{
		UNewItemObject* Parts = nullptr;
		switch ( PartsType )
		{
		case EWeaponPartsType::EWPT_Muzzle:
			if ( OwnerWeapon && MuzzleParts)//OwnerWeapon.IsValid() && MuzzleParts.IsValid() )
			{
				Parts = MuzzleParts;//.Get();
				SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Muzzle);
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if ( OwnerWeapon && ScopeParts)
			{
				Parts = ScopeParts;
				SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Scope);

				if ( OwnerWeapon->OptionalStaticMesh->GetStaticMesh() )
				{
					OwnerWeapon->OptionalStaticMesh->SetVisibility(false, false);
				}

			}
			break;
		case EWeaponPartsType::EWPT_Tactical:
			if ( OwnerWeapon && TacticalParts)
			{
				Parts = TacticalParts;
				SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Tactical);
			}
			break;
		}
	}

}

void UWeaponPartsManagerObject::SpawnAndAttachParts(UWorld* World, UNewItemObject* PartsObj, EWeaponPartsType VarPartsType)// AWeapon* VarWeapon)
{
	if ( AEquipment* EquipmentParts = UCustomInventoryLibrary::SpawnEquipment(World, PartsObj) )
	{
		AWeaponParts* Parts = Cast<AWeaponParts>(EquipmentParts);
		if(Parts == nullptr) return;

		const USkeletalMeshSocket* WeaponSocket = nullptr;
		bool bSKMeshAttach = false;
		FName SKSocketName = NAME_None;
		switch ( VarPartsType )
		{
		case EWeaponPartsType::EWPT_Muzzle:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(MuzzleSocketName);
			A_MuzzleParts = Parts;
			SKSocketName = MuzzleSocketName;
			break;
		case EWeaponPartsType::EWPT_Scope:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(ScopeSocketName);
			A_ScopeParts = Parts;
			SKSocketName = ScopeSocketName;
			break;
		case EWeaponPartsType::EWPT_Tactical:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(TacticalSocketName);
			A_TacticalParts = Parts;
			SKSocketName = TacticalSocketName;
			break;
		}

		if ( WeaponSocket )
		{
			//MeshSetting을 하면서 SKMesh인 경우 bool setting.
			if ( Parts->SKMesh->SkeletalMesh )
			{
				Parts->SKMesh->SetHiddenInGame(false);
				Parts->SKMesh->SetSimulatePhysics(false);
				Parts->SKMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				bSKMeshAttach = true;
			}
			//Mesh Setting
			//StaticMesh와 SKMesh가 동시에 있는경우 StaticMesh를 숨긴다.
			if(Parts->Mesh && bSKMeshAttach )
			{
				Parts->Mesh->SetHiddenInGame(true);
				Parts->Mesh->SetSimulatePhysics(false);
				Parts->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else if ( Parts->Mesh )
			{
				Parts->Mesh->SetSimulatePhysics(false);
				Parts->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}


			//Attach 작업
			if ( bSKMeshAttach )
			{
				FMatrix SocketTM;
				if (WeaponSocket->GetSocketMatrix(SocketTM, OwnerWeapon->SKMesh))
				{
					UCustomPDA* PartsPDA = Cast<UCustomPDA>(Parts->ItemSetting.DataAsset);
					if ( PartsPDA == nullptr ) return;

					//왜 해놓은건지 모르겠다.
					/*Parts->Modify();
					Parts->SetActorLocation(SocketTM.GetOrigin(), false);
					Parts->SetActorRotation(SocketTM.Rotator());					
					Parts->SKMesh->AttachToComponent(OwnerWeapon->SKMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SKSocketName);*/					


					WeaponSocket->AttachActor(Parts, OwnerWeapon->SKMesh);
					//Parts->SetActorRelativeTransform(PartsPDA->MeshAttachTransform);
					Parts->SetOwnerWeapon(OwnerWeapon);
					Parts->SetActorEnableCollision(false);
				}
			}
			else
			{
				if ( WeaponSocket->AttachActor(Parts, OwnerWeapon->SKMesh) )
				{
					Parts->SetOwnerWeapon(OwnerWeapon);
					Parts->SetActorEnableCollision(false);
				}
			}
			

			/*
			FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, false);
			Parts->SKMesh->AttachToComponent(OwnerWeapon->Mesh, rules, ScopeSocketName);
			if ( Parts->SKMesh->AttachToComponent(OwnerWeapon->SKMesh, rules, ScopeSocketName) )
			{
				Parts->SetActorEnableCollision(false);
			}*/



			
		}

	}

}

void UWeaponPartsManagerObject::DestroyAllAttachParts(AWeapon* VarWeapon)
{
	if ( OwnerWeapon )
	{
		TArray<AActor*> AttachedParts;
		OwnerWeapon->GetAttachedActors(AttachedParts);

		if ( OwnerWeapon->OptionalStaticMesh->GetStaticMesh() )
		{
			OwnerWeapon->OptionalStaticMesh->SetVisibility(true, false);
		}

		if ( AttachedParts.Num() > 0 )
		{
			for ( auto* Parts : AttachedParts )
			{
				if ( AEquipment* PartsActor = Cast<AEquipment>(Parts) )
				{
					if ( PartsActor->ItemObj )
					{
						PartsActor->ItemObj->bIsDestoryed = true;
					}
					A_MuzzleParts = nullptr;
					A_ScopeParts = nullptr;
					A_TacticalParts = nullptr;
					/*switch ( Cast<UWeaponPartsPDA>(PartsActor->ItemSetting.DataAsset)->WeaponPartsType )
					{
						case EWeaponPartsType::EWPT_Muzzle:
							A_MuzzleParts = nullptr;
							break;
						case EWeaponPartsType::EWPT_Scope:
							A_ScopeParts = nullptr;
							break;
						case EWeaponPartsType::EWPT_Tactical:
							A_TacticalParts = nullptr;
							break;
					}*/
					PartsActor->Destroy();
				}
			}
		}
	}
}

AEquipment* UWeaponPartsManagerObject::GetWeaponParts(EWeaponPartsType PartsWantToGet)
{
	switch ( PartsWantToGet )
	{
	case EWeaponPartsType::EWPT_Muzzle:
		if( MuzzleParts && A_MuzzleParts)
		{
			return A_MuzzleParts;
		}
		return nullptr;
		break;
	case EWeaponPartsType::EWPT_Scope:
		if ( ScopeParts && A_ScopeParts)
		{
			return A_ScopeParts;
		}
		return nullptr;
		break;
	case EWeaponPartsType::EWPT_Tactical:
		if ( TacticalParts && A_TacticalParts)
		{
			return A_TacticalParts;
		}
		return nullptr;
		break;
	}
	return nullptr;
}

//
//
//void UWeaponPartsManagerObject::SpawnAndAttachParts(UWorld* World, UNewItemObject* PartsObj, AWeapon* VarWeapon)
//{
//	if(PartsObj == nullptr || World  == nullptr) return;
//	
//	if (AEquipment* Parts = UCustomInventoryLibrary::SpawnEquipment(World, PartsObj))
//	{
//		UCustomPDA* PartsPDA = Cast<UCustomPDA>(Parts->ItemSetting.DataAsset);
//		if (PartsPDA)
//		{
//			const USkeletalMeshSocket* WeaponSocket = nullptr;
//			switch (PartsPDA->WeaponPartsType)
//			{
//			case EWeaponPartsType::EWPT_Muzzle:
//				WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(MuzzleSocketName);
//			break;
//			case EWeaponPartsType::EWPT_Scope:
//				WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(ScopeSocketName);
//
//			break;
//			case EWeaponPartsType::EWPT_Tactical:
//				WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(TacticalSocketName);
//			break;
//			}
//			
//			if (WeaponSocket)
//			{
//				if (WeaponSocket->AttachActor(Parts, OwnerWeapon->SKMesh))
//				{
//					Parts->SetActorEnableCollision(false);
//				}
//			}
//
//		}
//
//	}
//}