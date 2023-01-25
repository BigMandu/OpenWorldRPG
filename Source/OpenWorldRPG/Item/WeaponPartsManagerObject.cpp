// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"


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

		//�߰��� �� �ִٸ�, ������ �����ߴ������� remove�ѵ� �� ManagerObj�� �߰���Ų��.
		if (bIsSuccess)
		{
			DeleteLink(Parts);
			Parts->WeaponPartsManager = this;
		}
	}
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
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Scope:
			if (ScopeParts.IsValid() && ScopeParts.Get() == Parts)
			{
				ScopeParts = nullptr;
				bSuccessRemove = true;
			}
			break;
		case EWeaponPartsType::EWPT_Tactical:
			if (TacticalParts.IsValid() && TacticalParts.Get() == Parts)
			{
				TacticalParts = nullptr;
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



/*WeaponPDA�� �� Parts�� nullptr�Ǵ� data�� ���� ���� �� �ִ�.
* 
* �� UpdateParts �Լ��� WeaponPDA�� Parts���� null�̳� Data�� �޾����� ���� ȣ��ȴ�.
* ����, �� �Լ��� ȣ�� �� ������
* ��� parts���� Remove��Ű�� Spawn�ؼ� Attach���Ѿ� �Ѵ�.
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
			break;
		case EWeaponPartsType::EWPT_Scope:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(ScopeSocketName);
			break;
		case EWeaponPartsType::EWPT_Tactical:
			WeaponSocket = OwnerWeapon->SKMesh->GetSocketByName(TacticalSocketName);
			break;
		}

		if (WeaponSocket)
		{
			if (WeaponSocket->AttachActor(Parts, OwnerWeapon->SKMesh))
			{
				Parts->SetActorEnableCollision(false);
			}
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