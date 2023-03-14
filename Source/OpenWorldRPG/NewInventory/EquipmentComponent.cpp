// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();	
}

bool UEquipmentComponent::AddEquipment(AEquipment* Equip)
{
	
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bAlreadyHave = false;
	bool bReturn = false;
	
	/*
	if (Equip)
	{
		//if (Equip->OwningInventory != nullptr) //?紐源?醫?????? Weapon???,
		//{
		//	Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //?紐源?醫??癒苑 筌??????
		//}

		// ?????Equip????? ???筌? ?類ㅼ???. Swap??곕르 AddEquip????由???揆??
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == Equip)
			{
				bAlreadyHave = true;
				break;
			}
		}

		// ????Equip????용筌??곕떽???沅?? 
		if (bAlreadyHave == false)
		{
			Equip->OwningEquipment = this;
			EquipmentItems.Add(Equip);
			Equip->SKMesh->SetHiddenInGame(false); //?袁⑸嚥???紐?猿??

			//Backpack, Vest???關媛 ??????類ㅼ???.
			if(Equip->EquipmentType == EEquipmentType::EET_Backpack)
			{
				bHasBackpack = true;
			}
			else if(Equip->EquipmentType == EEquipmentType::EET_Vest)
			{
				bHasVest = true;
			}
			bReturn = true;
		}
		//EquipWidget::RefreshEquipWidget??bind??沅?
		OnEquipmentUpdated.Broadcast();

		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}*/
	return bReturn;
}



bool UEquipmentComponent::RemoveEquipment(AEquipment* Equip)
{
	if (Equip)
	{
		/*
		* EquipSlot::Ondrop?癒苑 疫꿸?? SpawnActor???룐???癰?野?留???怨??
		* RemoveSingle??肉 揶?? ??苡???
		* ?怨???remove??癰?野?鍮??곕?.
		*/
		//if (Equip->EquipmentType == EEquipmentType::EET_Backpack)
		{
			//bHasBackpack = false;
		}
		//else if (Equip->EquipmentType == EEquipmentType::EET_Vest)
		{
			//bHasVest = false;
		}

		/*
		AEquipment* InnerEquipment = GetEquippedWeaponSameType(Equip->EquipmentType);
		if (InnerEquipment)
		{
			//??已???袁㏉??苑 揶?? ??已??揶戮???? 揶?밴???EquipmentItems Array?癒苑 ???????
			// Equip Object??Destory?????
			//if (InnerEquipment->ItemName.EqualTo(Equip->ItemName))
			{
				EquipmentItems.RemoveSingle(InnerEquipment);

				Equip->OwningPlayer = nullptr;
				InnerEquipment->OwningPlayer = nullptr;
				Equip->Destroy();
				InnerEquipment->Destroy();

				OnEquipmentUpdated.Broadcast();

				return true;
			}
		}*/
	}
	return false;
}




/*
AEquipment* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, AEquipment* Equip, ERifleSlot RifleSlot)
{

	for (auto& EquipItem : EquipmentItems)
	{
		AEquipment* Equipped = Cast<AEquipment>(EquipItem);
		if (Equipped)
		{
			//Rifle??野????RiffleAssign??癰?욱?揶??紐???
			//Rifle ??????野???Primary Rifle, Sub Rifle ???野?筌??類ｋ┷??????????由???揆??
			if ((EquipType == EEquipmentType::EET_Rifle || (Equip != nullptr && Equip->EquipmentType == EEquipmentType::EET_Rifle)
				&& RifleSlot != ERifleSlot::ERS_MAX))
			{
				AWeapon* EquippedWeapon = Cast<AWeapon>(Equipped);
				if (EquippedWeapon)
				{
					if (EquippedWeapon->RifleAssign == RifleSlot)
					{
						return EquippedWeapon;
					}
				}
			}
			else
			{
				//World????쎈？??Equipment??野?????留 ??????.
				if (Equip != nullptr && EquipType == EEquipmentType::EET_MAX)
				{
					if (Equipped->EquipmentType == Equip->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip??? EquipType??곗?筌???爰쇌빳????
				{
					if (Equipped->EquipmentType == EquipType)
					{
						return Equipped;
					}
				}
			}
		}
	}
	return nullptr;
}
*/


bool UEquipmentComponent::SwapEquipment(AEquipment* Before, AEquipment* Next)
{
	/*
	//double check
	bool bReturn = false;
	if(Before->EquipmentType == Next->EquipmentType)
	{
		if(RemoveEquipment(Before))
		{
			bReturn = AddEquipment(Next);
		}
	}
	return bReturn;
	*/
	return false;
}



/*******************************************************************************************/
/*********************************** New Version *******************************************/
/*******************************************************************************************/

UNewItemObject* UEquipmentComponent::GetEquipStorage(EEquipmentType Type)
{
	switch (Type)
	{
	case EEquipmentType::EET_Backpack:
		return BackpackObj;
	case EEquipmentType::EET_Vest:
		return VestObj;
	default:
		return nullptr;
	}
}

bool UEquipmentComponent::AddEquipment(FItemSetting ItemSetting, AEquipment* WantToEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bCreated = false;
	bool bReturn = false;

	UNewItemObject* EquipObj = nullptr;

	//Equip??ItemObj揶? ??용筌???밴쉐?????
	if (WantToEquip->ItemObj == nullptr)
	{
		EquipObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated); //CreateObject(ItemSetting, bCreated);
		if (bCreated == false) return false;
		//??밴쉐??援??醫????.
		WantToEquip->ItemObj = EquipObj;
	}
	else
	{
		EquipObj = WantToEquip->ItemObj;
	}
	
	UCustomPDA* CPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);

	if (EquipObj && CPDA)
	{		
		//Equip->OwningEquipment = this;
		if (EquipObj->MotherStorage)
		{
			EquipObj->MotherStorage->RemoveItem(EquipObj);
			EquipObj->MotherStorage = nullptr;
		}
		else if (EquipObj->MotherEquipComp)
		{
			EquipObj->MotherEquipComp->RemoveEquipment(EquipObj);
			EquipObj->MotherEquipComp = nullptr;
		}

		//?關媛?醫釉???얜?椰?ItemRotate??false??沅??
		EquipObj->bTempRotate = false;
		EquipObj->bRotated = false;

		EquipObj->Equipment = WantToEquip;
		EquipObj->MotherEquipComp = this;

		//RifleType??? RifleAssign??????鍮餓Β?? //for QuickSlot & EquipWidget
		if (CPDA->EquipmentType == EEquipmentType::EET_Rifle || CPDA->EquipmentType == EEquipmentType::EET_Pistol)
		{	
			EquipObj->RifleAssign = Cast<AWeapon>(WantToEquip)->RifleAssign;
			//SetWeaponPartsManager(WantToEquip, EquipObj);
			OnWeaponSetSlot.Broadcast(EquipObj);
		}
		
		//Backpack, Vest???關媛 ??????類ㅼ???., Enum Range iter???袁る? Obj?????館鍮?遺얜.
		if(CPDA->EquipmentType == EEquipmentType::EET_Backpack)
		{
			BackpackObj = EquipObj;
		}
		else if(CPDA->EquipmentType == EEquipmentType::EET_Vest)
		{
			VestObj = EquipObj;
		}

		EquipmentItems.Add(EquipObj);

		bReturn = true;
		

		//EquipWidget::RefreshEquipWidget??bind?????
		OnEquipmentUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(UNewItemObject* EquipObj)
{
	if (EquipObj)
	{
		/* EquipSlot::Ondrop?癒苑 疫꿸?? SpawnActor???룐???癰?野?留???怨??
		 * RemoveSingle??肉 揶?? ??苡???
		 * ?怨???remove??癰?野?鍮??곕?.
		 */

		UCustomPDA* OuterCPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);
		UNewItemObject* InnerEquipment = GetEquippedWeaponSameType(OuterCPDA->EquipmentType, EquipObj);// ->ItemInfo);

		if (InnerEquipment)
		{
			UCustomPDA* InnerCPDA = Cast<UCustomPDA>(InnerEquipment->ItemInfo.DataAsset);
			//??已???袁㏉??苑 揶?? ??已??揶戮???? 揶?밴???EquipmentItems Array?癒苑 ???????
			// Equip Object??Destory?????

			if (InnerCPDA->ItemName.EqualTo(OuterCPDA->ItemName))
			{
				//BackPack,Vest??野???Set??Obj???醫?餓Β??
				if (InnerCPDA->EquipmentType == EEquipmentType::EET_Backpack)
				{
					BackpackObj = nullptr;			
				}
				else if (InnerCPDA->EquipmentType == EEquipmentType::EET_Vest)
				{
					VestObj = nullptr;
				}

				//for QuickSlot
				if (InnerCPDA->EquipmentType == EEquipmentType::EET_Rifle || InnerCPDA->EquipmentType == EEquipmentType::EET_Pistol)
				{
					OnWeaponRemoveSlot.Broadcast(EquipObj->RifleAssign);
					EquipObj->RifleAssign = ERifleSlot::ERS_MAX;
					
				}

				EquipmentItems.RemoveSingle(InnerEquipment);

				if (EquipObj->SettedSlot)
				{
					EquipObj->RemoveLinkSlot();				
				}

				//Weapon::Remove?癒苑 OwningPlayer???????由???揆????苑?????野????.
				EquipObj->Equipment->Remove();
				/*EquipObj->Equipment->OwningPlayer = nullptr;
				EquipObj->Equipment->Destroy();*/

				OnEquipmentUpdated.Broadcast();

				return true;
			}
		}
	}
	return false;
}

/* EqiupType ?癒? Obj?? 揶?? Type??EquipObj???귐苑???.
*/
UNewItemObject* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, UNewItemObject* Object, ERifleSlot RifleSlot)
{
	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			UCustomPDA* CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);
			UCustomPDA* ObjCPDA = Cast<UCustomPDA>(Object->ItemInfo.DataAsset);
			
			if(!CPDA || !ObjCPDA) continue;


			//Rifle??野????RiffleAssign??癰?욱?揶??紐??? ?癒? ItemSetting??Type??Rifle?????Rifle Slot??筌??類ｋ? 野???
			//Rifle ??????野???Primary Rifle, Sub Rifle ???野?筌??類ｋ┷??????????由???揆??
			if (EquipType == EEquipmentType::EET_Rifle)
			{
				
				if (CPDA && CPDA->EquipmentType != EEquipmentType::EET_Rifle) continue;

				if (Object != nullptr && ObjCPDA && ObjCPDA->EquipmentType == EEquipmentType::EET_Rifle)
				{
					if (Equipped->RifleAssign == Object->RifleAssign)
					{
						return Equipped;
					}
				}
				else if(RifleSlot != ERifleSlot::ERS_MAX)
				{
					if (Equipped->RifleAssign == RifleSlot)
					{
						return Equipped;
					}
				}				
			}
			else
			{
				//World????쎈？??Equipment??野?????留 ??????.
				if (EquipType == EEquipmentType::EET_MAX && CPDA)
				{
					if (Object != nullptr && CPDA->EquipmentType == ObjCPDA->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip??? EquipType??곗?筌???爰쇌빳????
				{
					if (CPDA->EquipmentType == EquipType)
					{
						return Equipped;
					}
				}
			}
		}
	}
	return nullptr;
}

bool UEquipmentComponent::IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot)
{
	
	int32 RifleCnt = 0;

	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			UCustomPDA* Var_CPDA = Cast<UCustomPDA>(Equip->ItemSetting.DataAsset);
			UCustomPDA* Inner_CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);

			//?關媛?????EquipType??Weapon??욱??關媛??Type??Weapon??野?????以 ?袁㏉???.
			if (Inner_CPDA->EquipmentType == EEquipmentType::EET_Rifle && Var_CPDA->EquipmentType == EEquipmentType::EET_Rifle)
			{

				//Rifle Slot??筌??類ｋ┷筌? ??? 野???(WorldSpawn ?怨밴묶) 燁삳??????.
				//Rifle Type?? 2揶??댐?? ??????.
				if (RifleSlot == ERifleSlot::ERS_MAX)
				{
					RifleCnt++;
				}
				//Rifle Slot??筌??類ｋ? 野???Rifle??筌??類ｋ? ???????????野????筌? ?類ㅼ???.
				else
				{
					if (Equipped->RifleAssign == RifleSlot)
					{
						return true;
					}
				}
			}
			else if(Inner_CPDA->EquipmentType == Var_CPDA->EquipmentType) //???よ?紐
苑?Weapon??Type????? ???筌?true
			{
				return true;
			}

			if (RifleCnt >= 2) //RifleType???얜용┛揶? 2揶???怨몄?野???true
			{
				return true;
			}
		}
	}
	//???筌?false?귐苑?
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Weapon????? WPM??Obj??WPM??????鍮??揶源??????
//void UEquipmentComponent::SetWeaponPartsManager(AEquipment* WantToEquip, UNewItemObject* WeaponObj)
//{
//	if (UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset))
//	{
//		if (WeaponObj->WeaponPartsManager.IsValid() == false)
//		{
//			if (UWeaponPartsManagerObject* WeapWPM = Cast<AWeapon>(WantToEquip)->WeaponPartsManager)
//			{
//				WeaponObj->WeaponPartsManager = WeapWPM;
//			}
//			else
//			{
//				WeaponObj->WeaponPartsManager = NewObject<UWeaponPartsManagerObject>(UWeaponPartsManagerObject::StaticClass());
//			}
//		}
//	}	
//}
