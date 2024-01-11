// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/ConsumePDA.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"

#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"

#include "OpenWorldRPG/WorldControlVolume/SpawnVolume.h"

#include "OpenWorldRPG/MainAnimInstance.h"


#include "Engine/SkeletalMeshSocket.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "DrawDebugHelpers.h"
#include "Equipment.h"


AItem::AItem() : Super()
{
	ItemState = EItemState::EIS_Spawn;

	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StimuliComp->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliComp->RegisterWithPerceptionSystem();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);	
	
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	/*MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Block);*/

	//Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	if (ItemSetting.DataAsset)
	{
		//ItemSetting.DataAsset->ItemSize = FIntPoint(1, 1);
		//ItemSetting.DataAsset->bCanRotate = true;
	}
	
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UMeshComponent>(GetRootComponent())->SetSimulatePhysics(true);
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		//SetRootComponent(Mesh);
	}
	if (SKMesh)
	{
		SKMesh->SetSimulatePhysics(false);
		
	}
}

//UNewItemObject* AItem::GetDefaultItemObj()
//{
//	UNewItemObject* Obj = NewObject<UNewItemObject>(this, UNewItemObject::StaticClass(), FName(GetName()));
//	if (Obj && ItemSetting.DataAsset)
//	{
//		//Obj->thumbnail = Thumbnail;
//		Obj->item = this;
//		Obj->bCanRotated = ItemSetting.DataAsset->bCanRotate;
//		//Obj->itemsize = ItemSetting.DataAsset->ItemSize;
//		Obj->itemName = ItemSetting.DataAsset->ItemName;
//		Obj->itemDesc = ItemSetting.DataAsset->ItemDescription;
//
//		Obj->icon = ItemSetting.DataAsset->Icon;
//		Obj->iconRotated = ItemSetting.DataAsset->IconRotated;
//		Obj->InteractType = ItemSetting.DataAsset->InteractType;
//
//		UCustomPDA* CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
//		if(CPDA)
//		{
//			Obj->bCanEquip = CPDA->bCanEquip;
//			Obj->EquipmentType = CPDA->EquipmentType;
//			
//		}
//		//UE_LOG(LogTemp, Warning, TEXT("AItem::Create object"));
//	}
//	else
//	{
//		//UE_LOG(LogTemp, Warning, TEXT("AItem::fail to Create object"));
//	}
//	return Obj;
//}

bool AItem::IsUsableItem()
{
	EItemType itemType = this->ItemSetting.DataAsset->ItemType;
	if (itemType != EItemType::EIT_Scrap && itemType != EItemType::EIT_NONE && itemType != EItemType::EIT_MAX)
	{
		return true;
	}
	return false;
}


bool AItem::Pickup(class AActor* Actor, UNewItemObject* obj)
{
	//bool bReturn = false;
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	bool bIsAdded = false;

	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	if (BChar)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));
		if (BChar->Equipment)
		{

			
			/** 포켓, 베스트에 있어야 되는 아이템(총알, 메디컬, 푸드 등등) 이면
			 *  베스트와 포켓 부터 넣는 시도를 한다.
			 */
			if (IsUsableItem())
			{
				
				if (UItemStorageObject* Storage = BChar->Equipment->GetEquipStorage(EEquipmentType::EET_Vest))
				{
					bIsAdded = BChar->BaseInventoryComp->TryAddItem(Storage, ItemSetting,this);
				}
				if (bIsAdded == false)
				{
					bIsAdded = AddAtCharInv(BChar, BChar->PocketStorage, nullptr);
				}
			}

			if(bIsAdded == false)
			{
				//New Version. EnumRange를 이용해 Backpack, Vest순으로 넣을 수 있다면 넣는다.
				for (EEquipmentType EquipTp : TEnumRange<EEquipmentType>())
				{
					if(UItemStorageObject* TempObj = BChar->Equipment->GetEquipStorage(EquipTp))
					{
						UCustomPDA* Var_CPDA = nullptr;
						Var_CPDA = Cast<UCustomPDA>(TempObj->ItemInfo.DataAsset);
						if (Var_CPDA == nullptr || (Var_CPDA && Var_CPDA->bHasStorage == false))
						{
							continue;
						}

						bIsAdded = BChar->BaseInventoryComp->TryAddItem(TempObj, ItemSetting, this, obj);
						if (bIsAdded) break;
					}
				}

				//장착중인 장비에 AddItem이 실패했다면 Pocket, Securebox순으로 AddItem을 시도한다.
				//pocket, secure box
				if(bIsAdded == false)
				{
					//bFlag = AddAtCharInv(BChar->PocketInventoryComp);
					bIsAdded = AddAtCharInv(BChar, BChar->PocketStorage, obj);
					if(bIsAdded == false)
					{
						bIsAdded = AddAtCharInv(BChar, BChar->SecureBoxStorage, obj);
						if(bIsAdded == false)
						{
							//DDOPer의 defaultVisual을 복제하면서 필요 없어짐.
							//PickUP상태의 Item Add에 실패했다면 CustomInventoryLibrary의 BackToItem을 실행한다.
							/*if (ItemState == EItemState::EIS_Pickup)
							{
								ItemObj->bIsDestoryed = true;
								Destroy();
							
								UCustomInventoryLibrary::DirectInToInventory(ItemObj, BChar);							
							}*/
							UE_LOG(LogTemp, Warning, TEXT("Item Add fail. No Space"));
						}
					}
				}
			}

			//추가 했다면 나머지 정리를 한다.
			if (bIsAdded)
			{
				SetItemState(EItemState::EIS_Pickup);
				//ItemObj->bIsDestoryed = true;

				//Player가 이 Item을 습득하면 SpawnVolume의 destorycount를 증가시킨다.
				if ( OwningVolume.IsValid() )
				{
					OwningVolume->IncreaseDestroyCount();
				}


				//Weapon이었다면 부착된 Parts Actor또한 destory시켜준다.
				if (AWeapon* WeapDest = Cast<AWeapon>(this))
				{
					if(WeapDest->WeaponPartsManager)
					{
						WeapDest->WeaponPartsManager->DestroyAllAttachParts(WeapDest);
					}
				}
				

				Destroy();
			}

			
		}
		
		//old version
		/*if (BChar->InventoryComp)
		{
			if (BChar->InventoryComp->TryAddItem(ItemObj))
			{
				SetItemState(EItemState::EIS_Pickup);
				Destroy();
				UE_LOG(LogTemp, Warning, TEXT("success Add item"));
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("fail to Add item"));
			}
		}*/
	}
	//bReturn = bFlag;
	return bIsAdded;
}

bool AItem::AddAtCharInv(ABaseCharacter* Character, UItemStorageObject* Storage, UNewItemObject* obj)
{
	if (Storage)
	{
		if (Character->BaseInventoryComp->TryAddItem(Storage, ItemSetting, this, obj))
		{
			return true;
		}

		/*if (InvComp->TryAddItem(ItemObj))
		{
			SetItemState(EItemState::EIS_Pickup);
			ItemObj->bIsDestoryed = true;
			Destroy();
			return true;
		}*/
	}
	return false;
}

bool AItem::AddAtEquip(AEquipment* Equipped)
{
	if (Equipped)
	{
		//if (Equipped->EquipInventoryComp->TryAddItem(ItemObj))
		{
			SetItemState(EItemState::EIS_Pickup);
			//ItemObj->bIsDestoryed = true;
			Destroy();
			return true;
		}
	}
	return false;
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		if (OwningInventory->RemoveItem(this->ItemObj))
		{
			/*Mesh->SetHiddenInGame(false);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetEnableGravity(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/

			SetItemState(EItemState::EIS_Spawn);
			FVector OwnerLocation = OwningInventory->GetOwner()->GetActorLocation();
			FVector OwnerForwardLo = OwningInventory->GetOwner()->GetActorForwardVector();
			FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;			
			
			
			/* Item을 Drop할때 캐릭터 앞으로 버리게 하는데, 힘을 랜덤으로 줘서 살짝씩 다르게 한다.*/
			//SetActorLocation(DropLocation);
			//Mesh->SetWorldLocation(DropLocation);
			
			float Rand = FMath::RandRange(1.f, 100000.f);
			//Mesh->AddForce(FVector(OwnerForwardLo * Rand));

			/* debug */
			//DrawDebugSphere(GetWorld(), DropLocation, 10.f, 8, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
			//UE_LOG(LogTemp, Warning, TEXT("AItem::Drop success"));

			return;
		}
	}
	/* debug */
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Drop fail"));
}


//Remove Weapon Or Item before Attached.
// And Change Holding Item.
//Attach하기 전 과정
void AItem::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj, bool bIsNeedToDestory)
{
	if (Actor->EquippedWeapon)
	{
		//AimMode 였다면 Aim을 풀어준다.
		if ( Actor->bIsAim )
		{
			Actor->SetAimMode(EAimMode::EAM_NotAim);
		}
		Actor->EquippedWeapon->GunAttachToSubSocket(Actor);

		//재장착을 위해 임시저장한다.
		BeforeEquipppedWeapon = Actor->EquippedWeapon;
		Actor->SetEquippedWeapon(nullptr);

		Actor->ChangeWeapon(0);
	}
	//이미 다른 HoldingItem이 있다면 비교한다.
	else if (Actor->HoldingItem.IsValid())
	{
		/**만일 이거랑 동일한거면 새로 생성한 이 item을 destory한다.
		 *다만, 원래 있던 Item을 재호출 하는 경우엔 삭제가 되면 안되므로 bIsNeedToDestory가 false인 경우 패스한다.
		 * False로 Setting해 호출 하는곳은 MainCharacter::VkeyDN
		 */
		if (Actor->HoldingItem == this && bIsNeedToDestory)
		{
			this->Destroy();
			return;
		}
		//만일 다른 item을 들고 있었다면, 기 등록된 holding actor를 삭제한다.
		else if (Actor->HoldingItem != this)
		{
			//여기 싹 뜯어 고쳐야함.
			/**
			 * 지금 발생하는 문제.
			 *
			 *  --> 1번 문제는 AnimBP문제 였음.. 해결완료.
			 * 1. HoldingItem -> ChangeWeapon시 equpping animation이 제대로 수행되지 않는점.
			 *    (Grenade RMB Down중에 change weapon할땐 Anim이 제대로 동작함. 참고할것)
			 * 
			 * 2. CoreUsableItem을 사용중에 다른 HoldingItem을 attach했을때,
			 *    CoreUsable의 ToggleOff가 안꺼지는 문제 발생.
			 *    아래 코드는 해당 문제를 해결하기 위한 코드인데, 아래 코드를 실행하면 정작 CoreUsable을 재사용 했을시 ToggleOff 함수가 호출되지 않음.
			 *     
			 */

			/*if (ACoreUsableItem* CoreItem = Cast<ACoreUsableItem>(Actor->HoldingItem))
			{
				CoreItem->ToggleOff(Cast<AMainCharacter>(Actor), Obj);

			}
			else*/
			{
				Actor->HoldingItem->ReadyToDestory(Actor,Obj);
				Actor->SetHoldingItem(nullptr);
			}
			
		}
	}

	if (Obj)
	{
		ItemObj = Obj;
	}


	Mesh->SetSimulatePhysics(false);

	//손에 든 순간부터 Interaction이 되지 않도록 한다.
	bCanNotInteractable = true;

	//MainChar의 LMBDown, Up을 위해 EquippedGrenade변수를 선언, 대입한다.
	Actor->SetHoldingItem(this);//HoldingItem = this;

	AttachToHand_Step(Actor);
}

void AItem::AttachToHand_Step(ABaseCharacter* Actor)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Actor);
	if (Player)
	{
		/**Item을 Player Character에 부착시킨다.
		 * 따로 사용하는 이유는 FPS, TPS시점에 따라 attach될 mesh가 다르기 때문.
		 * 또, 아래 함수는 V key(시점 변경)를 누를때마다 호출된다.
		*/
		Player->ReAttachHoldingItem();
	}
	//Player가 아닌 AI Character의 Attach
	else
	{
		const USkeletalMeshSocket* AttachSocket = nullptr;
		FTransform AttachTransform;

		if (Actor)
		{
			//이 Item이 왼손에 attach될 item
			if (ItemSetting.DataAsset->bAttachedToLeftHand)
			{
				//AnimBP의 Layered blend per bone node에 연관됨
				Actor->TPAnimInstance->SetIsRightHandAttached(false);
				AttachSocket = Actor->GetMesh()->GetSocketByName(Actor->LeftHandGripSocketName);
			}
			//이 Item은 오른손에 attached.
			else
			{
				//AnimBP의 Layered blend per bone node에 연관됨
				Actor->TPAnimInstance->SetIsRightHandAttached(true);
				AttachSocket = Actor->GetMesh()->GetSocketByName(Actor->GripSocketName);
			}
			//const USkeletalMeshSocket* LeftHandSocket = Actor->GetMesh()->GetSocketByName(Actor->LeftHandGripSocketName);

			if(AttachSocket == nullptr ) return;
			
			if ( AttachSocket->AttachActor(this,Actor->GetMesh()))
			{
				if (ItemSetting.DataAsset)
				{
					SetActorRelativeTransform(ItemSetting.DataAsset->TPS_HandAttachTransform);
				}

				//SKMesh->SetHiddenInGame(false);
				//BChar->SetEquippedWeapon(this);
			}
			

		}
	}


	Actor->PlayAnimation(this->ItemSetting.DataAsset->TPS_AttachAnimMontage, this->ItemSetting.DataAsset->FPS_AttachAnimMontage);

	/*if ( Player )
	{
		Player->PlayAttachItemAnim(this);
	}
	else
	{
		Actor->PlayAttachItemAnim(this);
	}*/

}


/**
 * 단순하게 Detach만 시켜준다.
 * detach했다고 Destory까진 안함, grenade도 이 함수를 사용하기 때문에...
 */
void AItem::DetachFromHand(ABaseCharacter* Actor, bool bIsNeedToEquipBeforeWeapon)
{
	//Detach를 한다.
	FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	this->DetachFromActor(Rules);

	//던진뒤 PrimaryWeapon을 장착시킨다.
	Actor->SetHoldingItem(nullptr);// = nullptr;

	if(bIsNeedToEquipBeforeWeapon)
	{
		EquipBeforeWeapon(Actor);
	}
}

void AItem::Use(ABaseCharacter* Char, UNewItemObject* Obj)
{
	check(Char)
	UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	
	//Char->PlayUseItemAnim(this);

	if (ItemSetting.DataAsset->ItemType == EItemType::EIT_Food || ItemSetting.DataAsset->ItemType == EItemType::EIT_Medical)
	{
		Char->PlayAnimation(this->ItemSetting.DataAsset->TPS_UseAnimMontage, this->ItemSetting.DataAsset->FPS_UseAnimMontage);

		UConsumePDA* ConsumablePDA = Cast<UConsumePDA>(ItemSetting.DataAsset);		
		if (ConsumablePDA->bIsHPRecovery)
		{			
			Char->RecoveryHealthDelegate(ConsumablePDA->RecoveryTime, ConsumablePDA->RecoveryAmount,this);
			UE_LOG(LogTemp,Warning, TEXT("AItem::Use // HP Recovery "));
		}
		if (ConsumablePDA->bIsStaminaRecovery)
		{
			Char->RecoveryStaminaDelegate(ConsumablePDA->RecoveryTime, ConsumablePDA->RecoveryAmount, this);
			UE_LOG(LogTemp, Warning, TEXT("AItem::Use // Stamina Recovery "));
		}

		RemoveCountAtIventory(Char, 1);
	}	
}


void AItem::RemoveCountAtIventory(ABaseCharacter* Char, int32 RemoveCount)
{
	if (ItemObj)// && ItemSetting.DataAsset->bCanStack)
	{
		Char->BaseInventoryComp->RemoveItemCount(ItemObj, RemoveCount);
	}
}

bool AItem::EquipBeforeWeapon(ABaseCharacter* Actor)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(Actor))
	{
		Player->ChangeWeaponTypeNumber(0);
		Player->FPAnimInstance->WeaponTypeNumber = 0;
	}
	else
	{
		if (Actor)
		{
			Actor->ChangeWeaponTypeNumber(0);
		}
	}

	
	if (BeforeEquipppedWeapon.IsValid())
	{
		switch (BeforeEquipppedWeapon->RifleAssign)
		{
			case ERifleSlot::ERS_Primary:
				Actor->ChangePrimaryWeapon();
			break;
			case ERifleSlot::ERS_Sub:
				Actor->ChangeSubWeapon();
			break;
			case ERifleSlot::ERS_Pistol:
				Actor->ChangePistolWeapon();
			break;
		}
		return true;
	}

	return false;
}

void AItem::ReadyToDestory(ABaseCharacter* Actor, UNewItemObject* Obj)
{
	Actor->SetHoldingItem(nullptr);
	Destroy();
}


//void AItem::SettingStorage()
//{
//	//EquipGridWidget이 없을때만 생성한다.
//	if (ItemInvComponent == nullptr)
//	{
//		if (MainCon == nullptr)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("SettingStorage::MainCon is nullptr try Get Controller"));
//			if (OwningPlayer)
//			{
//				//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
//				MainCon = Cast<AMainController>(OwningPlayer->GetController());
//			}
//		}
//
//		if (WEquipGridWidget && MainCon)
//		{
//			ItemInvComponent = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
//			if (ItemInvComponent && ItemObj->GetItemInvComp() != nullptr)// && EquipInventoryComp)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("SettingStorage::Try Initialize EquipGridWidget "));
//				//ItemObj->SetItemInvComp(EquipInventoryComp);
//				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap시에 문제 발생 (ItemObj의 InvComp가 null만 가지고 있음)
//				ItemInvComponent->GridInitialize(ItemObj->GetItemInvComp(), ItemObj->GetItemInvComp()->TileSize);
//
//			}
//		}
//	}
//}
