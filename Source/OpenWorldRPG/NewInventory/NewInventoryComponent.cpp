// Fill out your copyright notice in the Description page of Project Settings.


#include "NewInventoryComponent.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "Containers/Array.h"

#define DEBUG_INVCOMP 0
// Sets default values for this component's properties
UNewInventoryComponent::UNewInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}


// Called when the game starts
void UNewInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//해당InventoryComp의 Inventory배열사이즈를 설정한다.
	//InventoryItems[Columns * Rows] = { nullptr, };
	//InventoryItems.SetNum(Columns * Rows);
	InventoryItems.Init(nullptr, Columns * Rows);
	
}

bool UNewInventoryComponent::RemoveItem(UNewItemObject* ItemObj)
{
#ifdef DEBUG_INVCOMP
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::Remove"));
#endif	
	
	//for (int32 iter = 0; iter < InventoryItems.Num(); ++iter)
	//{
	//	if (InventoryItems[iter] == ItemObj)
	//	{
	//		InventoryItems[iter] = nullptr;
	//	}
	//}

	FIntPoint Size = ItemObj->GetItemSize();
	FTile ItemIndex = FTile(0, 0); //IndexToTile(ItemObj->TopLeftIndex);
	int32 loopX = ItemIndex.X + Size.X - 1;
	int32 loopY = ItemIndex.Y + Size.Y - 1;

	FTile removeTile;

	for (int32 i = ItemIndex.X; i <= loopX; i++)
	{
		for (int32 j = ItemIndex.Y; j <= loopY; j++)
		{

			removeTile.X = i;
			removeTile.Y = j;
			int32 removeIndex = 0;// = TileToIndex(removeTile);

			InventoryItems[removeIndex] = nullptr;
		}
	}
	

	return true;
}

bool UNewInventoryComponent::TryAddItem(UNewItemObject* ItemObj)
{
	/*
	* 이 ItemObj의 크기를 차지할 타일을 반복한다.
	*/
	bool bResult = false;
	if (ItemObj)
	{
		bResult = TryAddItemStep(ItemObj);

		/* Item추가에 실패했고 Item을 돌릴 수 있다면 */
		if (!bResult && ItemObj->bCanRotated)
		{
#ifdef DEBUG_INVCOMP
			UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Inventory is full, Try Item Rotate."));
#endif
			/* Item을 돌리고 다시 추가한다. */
			ItemObj->ItemRotate();
			bResult = TryAddItemStep(ItemObj);

			/* 그래도 실패했다면 돌린 item을 다시 원복 시킨다.*/
			if (!bResult)
			{
				ItemObj->ItemRotate();
#ifdef DEBUG_INVCOMP
				UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Inventory is Completely full"));
#endif
			}
		}
	}
#ifdef DEBUG_INVCOMP
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Item Obj is invalid"));
#endif
	return bResult;
}

bool UNewInventoryComponent::TryAddItemStep(UNewItemObject* ItemObj)
{
//	UE_LOG(LogTemp, Warning, TEXT("UNEWINVCOMP : TryaddItemStep"));
	bool bResult = false;
	if (ItemObj)
	{
//		UE_LOG(LogTemp, Warning,  TEXT("UNEWINVCOMP : Item Obj is valid"));
		for (int32 iter = 0; iter < InventoryItems.Num(); ++iter)
		{
			bResult = IsAvailableSpace(ItemObj, iter);
			if (bResult)
			{
				AddItemAtIndex(ItemObj, iter);
				//해제는 InventoryLibrary::SpawnEquip 또는 DropWidget의 OnDrop에서 한다.
				ItemObj->bIsDestoryed = true;

				//SetItemState(EItemState::EIS_Pickup);
				return bResult;
			}
#ifdef DEBUG_INVCOMP
			UE_LOG(LogTemp, Warning, TEXT("UNEWINVCOMP : there is no space"));
#endif // DEBUG_INVCOMP
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNEWINVCOMP : Item Obj is invalid"));
	}
	return bResult;
}

void UNewInventoryComponent::AddItemAtIndex(UNewItemObject* ItemObj, int32 Index)
{
	ItemObj->TopLeftIndex = Index;
	FTile tile = FTile(0, 0);// IndexToTile(Index);
	FIntPoint Itemsize = ItemObj->GetItemSize();

	FTile Itemtile;
	Itemtile.X = 0;
	Itemtile.Y = 0;

	int32 HorizontalMAX = tile.X + Itemsize.X - 1;
	int32 VerticalMAX = tile.Y + Itemsize.Y - 1;

	for (int32 i = tile.X; i <= HorizontalMAX; ++i)
	{
		for (int32 j = tile.Y; j <= VerticalMAX; ++j)
		{
			Itemtile.X = i;
			Itemtile.Y = j;
			if ((Itemtile.X >= 0 && Itemtile.Y >= 0) && (Itemtile.X < Columns && Itemtile.Y < Rows))
			{
				int32 Addindex = 0;// TileToIndex(Itemtile);
				InventoryItems[Addindex] = ItemObj;
			}
		}
	}
#ifdef DEBUG_INVCOMP
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Success Add Item"));
#endif			
	/*	AddItem이 성공적이면 UNewInventoryGrid::RefreshInventory를 호출하기 위해
		Delegate를 생성하고 broadcast를 때려준다.
	*/
	OnInventoryUpdated.Broadcast();
}

bool UNewInventoryComponent::IsAvailableSpace(UNewItemObject* ItemObj, int32 TopLeftIndex)
{
	/*
		Inventory의 Top-Left부터 item을 add할때 
		이 Item이 차지할 타일만큼	(해당 index의 Top-left부터) 모든 Inventory tile을 
		loop 돌면서 유효한 공간이 있는지 확인한다. 이미 차지하고 있다면 false, 없다면 true를 리턴.
	*/
	bool bReturn = true;
	//FTile tile = ForEachIndex(ItemObj, TopLeftIndex);
	FTile tile = FTile(0, 0);// IndexToTile(TopLeftIndex);
	FIntPoint Itemsize = ItemObj->GetItemSize();

	FTile checktile;
	checktile.X = 0; checktile.Y = 0;

	int32 HorizontalMAX = tile.X + Itemsize.X - 1;
	int32 VerticalMAX = tile.Y + Itemsize.Y - 1;

	for (int32 i = tile.X; i <= HorizontalMAX; ++i)
	{
		for (int32 j = tile.Y; j <= VerticalMAX; ++j)
		{
			checktile.X = i;
			checktile.Y = j;
			if ((checktile.X >= 0 && checktile.Y >= 0) && (checktile.X < Columns && checktile.Y < Rows))
			{
				int32 index = 0;// TileToIndex(checktile);
				UNewItemObject* GettingItemObj = GetItemAtIndex(index);
				/* 해당 index에 이미 item obj가 있으면 false를 return한다 */
				if (GettingItemObj != nullptr)
				{
					bReturn = false;
					return bReturn;
				}
			}
			else
			{
#ifdef DEBUG_INVCOMP
				UE_LOG(LogTemp, Warning, TEXT("NewInvComp::IsAvaSpace = Tile is invalid, wrong range"));
#endif
				bReturn = false;
				return bReturn;
			}
		}
	}
	return bReturn;
}

/* item obj와 top left index를 가지고 item의 크기만큼 
	index를 loop돌면서 item이 차지할수 있는지 확인한다.
*/
/*
FTile UNewInventoryComponent::ForEachIndex(UNewItemObject* Obj, int32 TopLeftIndex)
{
	FTile tile = IndexToTile(TopLeftIndex);
	FIntPoint Itemsize = Obj->GetItemSize();
	FTile returntile;
	returntile.X = 0;
	returntile.Y = 0;

	int32 HorizontalMAX = tile.X + Itemsize.X - 1;
	int32 VerticalMAX = tile.Y + Itemsize.Y - 1;
	
	for (int32 i = tile.X; i < HorizontalMAX; ++i)
	{
		for (int32 j = tile.Y; j < VerticalMAX; ++j)
		{
			returntile.X = i;
			returntile.Y = j;
		}
		//returntile.X = i;
	}
	
	return returntile;
}
*/

/* Index를 grid의 x, y좌표로 바꿔준다.*/
//FTile UNewInventoryComponent::IndexToTile(int32 index)
//{
//	FTile tile;
//	tile.X = index % Columns;
//	tile.Y = index / Columns;
//	return tile;
//}

/* grid의 x,y좌표를 index로 바꾼다. */
//int32 UNewInventoryComponent::TileToIndex(FTile tile)
//{
//	return tile.X + tile.Y * Columns;	
//}

/* index에 item이 있는지 확인한다.*/
UNewItemObject* UNewInventoryComponent::GetItemAtIndex(int32 index)
{
	UNewItemObject* ReturnObj = nullptr;
	if (index >= 0 && index < InventoryItems.Num())
	{
		ReturnObj = InventoryItems[index];
#ifdef DEBUG_INVCOMP
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::GetItem = Success get Item Obj at index"));
#endif
	}
	else
	{
#ifdef DEBUG_INVCOMP
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::GetItem = Index is wrong range"));
#endif
	}
	return ReturnObj;
}

const TMap<UNewItemObject*, FTile> UNewInventoryComponent::GetAllItems()
{
	/*
	* Item과, 해당 item의 Top-Left index를 담은 Map을 리턴해준다.
	* Key값을 ItemObject로, Value를 FTile로 지정했다.
	*/
	TMap<UNewItemObject*, FTile> InventoryStoredInfo;
	
	//여기 InventoryItems에 Garbage값 들어옴 
	for (int32 index = 0; index < InventoryItems.Num(); ++index)
	{
		UNewItemObject* CurItemObj = InventoryItems[index];
		if (CurItemObj != nullptr)
		{
			bool bAlreadyHaveit = InventoryStoredInfo.Contains(CurItemObj);
			if (bAlreadyHaveit == false)
			{
				FTile CurItemSize = FTile(0, 0);// IndexToTile(index);
				InventoryStoredInfo.Add(CurItemObj, CurItemSize);
			}
		}
	}
	return InventoryStoredInfo;
}


bool UNewInventoryComponent::SpawnItem(UNewItemObject* ItemObj, AActor* Actor)
{
	FVector ActorLocation = Actor->GetActorLocation();
	FVector ActorForwardLo = Actor->GetActorForwardVector();
	FVector DropLocation = ActorLocation + ActorForwardLo * 120.f;

	FTransform form;
	form.SetLocation(DropLocation);
	/*SpawnActor<AItem>()
	UWorld::SpawnActor(ItemObj->GetItemClass(), DropLocation);*/
	return false;
}


/*************************************************************/
/*************************  New version **********************/
/*************************************************************/

bool UNewInventoryComponent::IsAvailableSpace(UItemStorageObject* StorageObj, UNewItemObject* ItemObj, int32 TopLeftIndex)
{
	return StorageObj->IsAvailableSpace(ItemObj, TopLeftIndex);
}

void UNewInventoryComponent::AddItemAtIndex(UItemStorageObject* StorageObj, UNewItemObject* ItemObj, int32 Index)
{
	StorageObj->AddItemAtIndex(ItemObj, Index);
}

bool UNewInventoryComponent::TryAddItem(UItemStorageObject* StorageObj, FItemSetting ItemSetting, AItem* WantToAddItem, UNewItemObject* Obj, bool bWantToGenerateRandomCount)
{
	bool bCreated = false;
	bool bAddCount = false;
	UNewItemObject* ItemObj = nullptr;


	if ( (WantToAddItem && WantToAddItem->ItemObj) || Obj )
	{
#ifdef DEBUG_INVCOMP
		//UE_LOG(LogTemp, Warning, TEXT("InvComp::TryAddItem / already has Obj"));
#endif
		if ( WantToAddItem->ItemObj )
		{
			ItemObj = WantToAddItem->ItemObj;
		}
		else if ( Obj )
		{
			ItemObj = Obj;
		}
		bCreated = true;
	}
	else
	{
#ifdef DEBUG_INVCOMP
		//UE_LOG(LogTemp,Warning,TEXT("InvComp::TryAddItem / No Obj, call CreateObj func "));
#endif
		ItemObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated);
	}


	if (bCreated)
	{
		//LootBox(Chest)에 Item을 Spawn시, 또는 World에 뿌려진 Backpack, EnemyAI Character에 Item을 Spawn시 수량을 랜덤하게 준다.
		if (bWantToGenerateRandomCount)
		{			
			UCustomInventoryLibrary::GenerateRandomCount(ItemObj);
		}
		else if (ItemObj->ItemInfo.DataAsset->bCanStack)
		{
			//Stack이 가능한 Item인 경우, Storage에서 동일한 Item을 구해 AddCount를 시켜준다.
			const auto ItemInStorage = StorageObj->GetAllItems();
			for (auto Item : ItemInStorage)
			{
				if (Item.Key->ItemInfo.DataAsset != ItemObj->ItemInfo.DataAsset) continue;
				if(Item.Key->ItemInfo.DataAsset->bCanStack == false) continue;
				

				//ItemAddCount를 했다면 for문을 break한다.
				if (AddItemCount(ItemObj, Item.Key))
				{
					bAddCount = true;
					break;

				}
			}
		}
		//ItemObj가 Weapon의 OBJ이고 && WPM이 없는 경우  WPM을 생성 해주고 Data를 넘겨준다.
		if ( UWeaponPDA* WeapPDA = Cast<UWeaponPDA>(ItemObj->ItemInfo.DataAsset))
		{
			if ( (WeapPDA->EquipmentType == EEquipmentType::EET_Rifle || WeapPDA->EquipmentType == EEquipmentType::EET_Pistol) &&
			WantToAddItem != nullptr )
			{
				
				UCustomInventoryLibrary::SetWeaponPartsManager(ItemObj, Cast<AWeapon>(WantToAddItem));
				if ( ItemObj->WeaponPartsManager )
				{
					ItemObj->WeaponPartsManager->OnChangeParts.Clear();
					ItemObj->WeaponPartsManager->OnChangeParts.AddDynamic(Cast<AWeapon>(WantToAddItem), &AWeapon::UpdateWeaponParts);
				}
			}
		}
		////만약 새로 생성한(INV에 ADD할) obj가 storage타입이라면, World에 배치된 Item의 StorageInventory를 그대로 넘겨준다.
		//else if ( UItemStorageObject* HasStorage = Cast<UItemStorageObject>(ItemObj))
		//{
		//	if ( UItemStorageObject* AddItemsStorage = Cast<UItemStorageObject>(WantToAddItem->ItemObj))
		//	{
		//		if ( AddItemsStorage->Inventory.Num() > 0 )
		//		{
		//			UCustomInventoryLibrary::CopyItem(*HasStorage,*AddItemsStorage);
		//			
		//			//HasStorage = AddItemsStorage;
		//			//HasStorage->Inventory = AddItemsStorage->Inventory;
		//		}				
		//	}			
		//}


#ifdef DEBUG_INVCOMP
		//UE_LOG(LogTemp, Warning, TEXT("InvComp::TryAddItem / confirm, Call StorageObj::TryAddItem"));
#endif
		
		if (bAddCount)
		{
			return true;
		}
		else
		{
			return StorageObj->TryAddItem(ItemObj, bWantToGenerateRandomCount);
		}
	}
	return false;
}

bool UNewInventoryComponent::RemoveItem(UItemStorageObject* StorageObj, UNewItemObject* ItemObj)
{
	return StorageObj->RemoveItem(ItemObj);
}


/*DroppedItem의 Count가 모두 Stack이 됐다면 true를 리턴하고
* DroppedItem의 Count가 남았으면 false를 리턴한다.
*/
bool UNewInventoryComponent::AddItemCount(UNewItemObject* DroppedItemObj, UNewItemObject* OnItemObj)
{
	int32 OnMAXStack = OnItemObj->ItemInfo.DataAsset->MaxStackSize;
	
	//Stack가능한 개수를 구한다.
	int32 OnCanStackCount = OnMAXStack - OnItemObj->ItemInfo.Count;
	int32 DroppedStackCount = DroppedItemObj->ItemInfo.Count;

	bool bReturn = false;

	// 1 이상 쌓을 수 없다면, Stack가능한 count가 0이면 걍 바로 false를 리턴한다.
	if (OnCanStackCount <= 0)
	{
		return false;
	}

	/*드랍개수가 Stack가능한 개수보다 크다면
	* OnItem(있던것)에 Stack가능한 개수를 넣어주고 DroppedItem(드랍한것)에서 Stack 가능한 개수를 빼준다.
	*/
	if (DroppedStackCount >= OnCanStackCount)
	{
		OnItemObj->AddCount(OnCanStackCount);
		RemoveItemCount(DroppedItemObj,OnCanStackCount);
		//DroppedItemObj->RemoveCount(OnCanStackCount);
	}
	else
	{
		OnItemObj->AddCount(DroppedStackCount);

		//Item이 Spawn이라 Storage가 없는 경우가 있ㄷㄷㅏ.
		//if(DroppedItemObj->MotherStorage)
		{
			RemoveItemCount(DroppedItemObj, DroppedStackCount);
			//DroppedItemObj->MotherStorage->RemoveItem(DroppedItemObj);
		}
	}

	if (DroppedItemObj && DroppedItemObj->ItemInfo.Count >= 1)
	{
		bReturn = false;
	}
	else
	{
		bReturn = true;
	}
	return bReturn;
}

//ItemObj의 Count를 안전하게 지우는 함수
void UNewInventoryComponent::RemoveItemCount(UNewItemObject* RemoveItemObj, int32 RemoveCount)
{	
	int32 RemoveStackCount = FMath::Clamp<int32>(RemoveItemObj->ItemInfo.Count - RemoveCount, 0, RemoveItemObj->ItemInfo.Count - RemoveCount);
#ifdef DEBUG_INVCOMP
	UE_LOG(LogTemp,Warning,TEXT("InvComp::RemoveItemCnt / RemoveStackCount is %d"),RemoveStackCount);
#endif
	//있던것 - 뺄갯수가 0이하면 RemoveItem을 진행한다.
	if (RemoveStackCount <= 0)
	{
		if (RemoveItemObj->MotherStorage)
		{
			//삭제 대기 여부를 True로 변경한다. 
			/*(MotherStorage의 RemoveItem에서 InventoryGrid Widget의 RefreshInvtory함수에서 
			* Quickslot 삭제를 해야하는지 판단할때 사용되는 bool변수임.
			* */
			RemoveItemObj->bIsPendingDelete = true;
			RemoveItemObj->RemoveCount(RemoveCount);
			RemoveItemObj->MotherStorage->RemoveItem(RemoveItemObj);
		}
		else
		{
			//MotherStorage가 없는 경우 -> Spawn된 Item인 경우(Craft, Pickup)
			//그냥 RemoveCount를 진행한다. ItemClass의 Destory는 Craft, Pickup에서 진행.
			RemoveItemObj->RemoveCount(RemoveCount);
		}
	}
	else
	{
		RemoveItemObj->RemoveCount(RemoveCount);
	}

}

int32 UNewInventoryComponent::GetItemCount(UNewItemObject* ItemObj)
{
	if (ItemObj)
	{
		return ItemObj->ItemInfo.Count;
	}
	return 799;
}


//CustomInventoryLibrary로 옮김.
/*
UNewItemObject* UNewInventoryComponent::CreateObject(FItemSetting ItemStruct, bool& bIsCreated)
{
	if (ItemStruct.DataAsset)
	{
		UNewItemObject* ReturnObj = nullptr;
		if (ItemStruct.DataAsset->bHasStorage)
		{
			ReturnObj = NewObject<UItemStorageObject>();
		}
		else
		{
			ReturnObj = NewObject<UNewItemObject>();
		}
		ReturnObj->ItemInfo = FItemSetting(ItemStruct.DataAsset, 1, 0);
		bIsCreated = true;
		return ReturnObj;
	}
	else
	{
		bIsCreated = false;
		return nullptr;
	}	
}
*/