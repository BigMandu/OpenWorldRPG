// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"


void UWeaponPartsManagerObject::AddParts(UNewItemObject* Parts)
{	
	if(Parts->ItemInfo.DataAsset == nullptr) return;
	bool bIsSuccess = false;
	UCustomPDA* PartsPDA = Cast<UCustomPDA>(Parts->ItemInfo.DataAsset);
	if (PartsPDA)
	{
		switch (PartsPDA->WeaponPartsType)
		{
		case EWeaponPartsType::EWPT_Muzzle:
			if (MuzzleParts.IsValid() == false)
			{
				MuzzleParts = Parts;
				bIsSuccess = true;
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if (ScopeParts.IsValid() == false)
			{
				ScopeParts = Parts;
				//ScopeParts->WeaponPartsManager = this;
				bIsSuccess = true;
			}
			break; 
		case EWeaponPartsType::EWPT_Tactical:
			if (TacticalParts.IsValid() == false)
			{
				TacticalParts = Parts;
				//TacticalParts->WeaponPartsManager = this;
				bIsSuccess = true;
			}
			break;
		}

		//추가할 수 있다면, 기존에 존재했던곳에서 remove한뒤 이 ManagerObj를 추가시킨다.
		if (bIsSuccess)
		{
			DeleteLink(Parts);
			Parts->WeaponPartsManager = this;
		}
	}
	/**WeaponPartsWidget, Weapon에서 Bind된다
	 * WeaponPartsWidget의 Widget을 update하고
	 * Weapon에서 이 class의 함수인 partsUpdate를 호출한다.
	 */
	OnChangeParts.Broadcast();
}

void UWeaponPartsManagerObject::RemoveParts(UNewItemObject* Parts)
{
	if (Parts->ItemInfo.DataAsset == nullptr) return;
	bool bSuccessRemove = false;
	UCustomPDA* PartsPDA = Cast<UCustomPDA>(Parts->ItemInfo.DataAsset);
	if (PartsPDA)
	{
		switch (PartsPDA->WeaponPartsType)
		{
		case EWeaponPartsType::EWPT_Muzzle:
			if (MuzzleParts.IsValid() && MuzzleParts.Get() == Parts)
			{
				MuzzleParts = nullptr;
				A_MuzzleParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if (ScopeParts.IsValid() && ScopeParts.Get() == Parts)
			{
				ScopeParts = nullptr;
				A_ScopeParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Tactical:
			if (TacticalParts.IsValid() && TacticalParts.Get() == Parts)
			{
				TacticalParts = nullptr;
				A_TacticalParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		}

		if (bSuccessRemove)
		{
			Parts->WeaponPartsManager = nullptr;
		}

	}

	OnChangeParts.Broadcast();
}

void UWeaponPartsManagerObject::DeleteLink(UNewItemObject* PartsObj)
{
	if (PartsObj->WeaponPartsManager.IsValid())
	{
		PartsObj->WeaponPartsManager->RemoveParts(PartsObj);
		PartsObj->WeaponPartsManager = nullptr;
	}
	else if (PartsObj->MotherStorage)
	{
		//PartsObj->GetMotherStorage()->RemoveItem(PartsObj);
		PartsObj->MotherStorage->RemoveItem(PartsObj);
		PartsObj->MotherStorage = nullptr;
	}
}



/*WeaponPDA의 각 Parts는 nullptr또는 data를 갖고 있을 수 있다.
* 
* 이 UpdateParts 함수는 WeaponPDA의 Parts들이 null이나 Data를 받았을때 마다 호출된다.
* 따라서, 이 함수가 호출 될 때마다
* 모든 parts들을 Remove시키고 Spawn해서 Attach시켜야 한다.
* */
void UWeaponPartsManagerObject::UpdateParts(UWorld* World, AWeapon* VarWeapon)
{
	//if(VarWeapon == nullptr) return;
	if(OwnerWeapon == nullptr) return;
	//OwnerWeapon = VarWeapon;

	if (OwnerWeapon->WeaponDataAsset == nullptr) return;
	DestroyAllAttachParts(OwnerWeapon);

	for (EWeaponPartsType PartsType : TEnumRange<EWeaponPartsType>())
	{
		UNewItemObject* Parts = nullptr;
		 switch(PartsType)
		 {
		 case EWeaponPartsType::EWPT_Muzzle:
			if (OwnerWeapon && MuzzleParts.IsValid())// OwnerWeapon->WeaponDataAsset->MuzzleParts.IsValid())
			{				
				//Parts = OwnerWeapon->WeaponDataAsset->MuzzleParts.Get();
				Parts = MuzzleParts.Get();
				SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Muzzle);//  OwnerWeapon);
			}
			break;
		 case EWeaponPartsType::EWPT_Scope:
			 if (OwnerWeapon && ScopeParts.IsValid())
			 {
				 Parts = ScopeParts.Get();
				 SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Scope);
			 }
			break;
		 case EWeaponPartsType::EWPT_Tactical:
			 if (OwnerWeapon && OwnerWeapon->WeaponDataAsset->MuzzleParts.IsValid())
			 {
				 Parts = OwnerWeapon->WeaponDataAsset->TacticalParts.Get();
				 SpawnAndAttachParts(World, Parts, EWeaponPartsType::EWPT_Tactical);
			 }
		  	break;
		 }
	}

	/*SpawnAndAttachParts(World, OwnerWeapon->WeaponDataAsset->MuzzleParts, OwnerWeapon);
	SpawnAndAttachParts(World, OwnerWeapon->WeaponDataAsset->TacticalParts, OwnerWeapon);
	SpawnAndAttachParts(World, OwnerWeapon->WeaponDataAsset->ScopeParts, OwnerWeapon);*/

}

void UWeaponPartsManagerObject::SpawnAndAttachParts(UWorld* World, UNewItemObject* PartsObj, EWeaponPartsType VarPartsType)// AWeapon* VarWeapon)
{
	if (AEquipment* Parts = UCustomInventoryLibrary::SpawnEquipment(World, PartsObj))
	{
		const USkeletalMeshSocket* WeaponSocket = nullptr;
		switch (VarPartsType)
		{
		case EWeaponPartsType::EWPT_Muzzle:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(MuzzleSocketName);
			A_MuzzleParts = Parts;
			break;
		case EWeaponPartsType::EWPT_Scope:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(ScopeSocketName);
			A_ScopeParts = Parts;
			break;
		case EWeaponPartsType::EWPT_Tactical:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(TacticalSocketName);
			A_TacticalParts = Parts;
			break;
		}

		if (WeaponSocket)
		{
			if (Parts->SKMesh->SkeletalMesh)
			{
				Parts->SKMesh->SetSimulatePhysics(false);
			}
			else
			{
				Parts->Mesh->SetSimulatePhysics(false);
			}

			FAttachmentTransformRules rules(EAttachmentRule::KeepRelative,false);
			if (Parts->SKMesh->AttachToComponent(OwnerWeapon->SKMesh, rules, ScopeSocketName))
			{
				Parts->SetActorEnableCollision(false);
			}

			

			/*if (WeaponSocket->AttachActor(Parts, OwnerWeapon->SKMesh))
			{


			}*/
		}

	}

}

void UWeaponPartsManagerObject::DestroyAllAttachParts(AWeapon* VarWeapon)
{
	if (OwnerWeapon)
	{
		TArray<AActor*> AttachedParts;
		OwnerWeapon->GetAttachedActors(AttachedParts);

		if (AttachedParts.Num() > 0)
		{
			for (auto* Parts : AttachedParts)
			{
				if(AEquipment* PartsActor = Cast<AEquipment>(Parts))
				{
					if (PartsActor->ItemObj)
					{
						PartsActor->ItemObj->bIsDestoryed = true;
					}
					PartsActor->Destroy();
				}
			}
		}
	}
}

AEquipment* UWeaponPartsManagerObject::GetWeaponParts(EWeaponPartsType PartsWantToGet)
{
	switch (PartsWantToGet)
	{
	case EWeaponPartsType::EWPT_Muzzle:
		return A_MuzzleParts.Get();
	break;
	case EWeaponPartsType::EWPT_Scope:
		return A_ScopeParts.Get();
	break;
	case EWeaponPartsType::EWPT_Tactical:
		return A_TacticalParts.Get();
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