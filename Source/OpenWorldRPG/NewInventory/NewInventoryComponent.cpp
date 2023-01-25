// Fill out your copyright notice in the Description page of Project Settings.


#include "NewInventoryComponent.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"

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
	
	//�ش�InventoryComp�� Inventory�迭����� �����Ѵ�.
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
	* �� ItemObj�� ũ�⸦ ������ Ÿ���� �ݺ��Ѵ�.
	*/
	bool bResult = false;
	if (ItemObj)
	{
		bResult = TryAddItemStep(ItemObj);

		/* Item�߰��� �����߰� Item�� ���� �� �ִٸ� */
		if (!bResult && ItemObj->bCanRotated)
		{
#ifdef DEBUG_INVCOMP
			UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Inventory is full, Try Item Rotate."));
#endif
			/* Item�� ������ �ٽ� �߰��Ѵ�. */
			ItemObj->ItemRotate();
			bResult = TryAddItemStep(ItemObj);

			/* �׷��� �����ߴٸ� ���� item�� �ٽ� ���� ��Ų��.*/
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
				//������ InventoryLibrary::SpawnEquip �Ǵ� DropWidget�� OnDrop���� �Ѵ�.
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
	/*	AddItem�� �������̸� UNewInventoryGrid::RefreshInventory�� ȣ���ϱ� ����
		Delegate�� �����ϰ� broadcast�� �����ش�.
	*/
	OnInventoryUpdated.Broadcast();
}

bool UNewInventoryComponent::IsAvailableSpace(UNewItemObject* ItemObj, int32 TopLeftIndex)
{
	/*
		Inventory�� Top-Left���� item�� add�Ҷ� 
		�� Item�� ������ Ÿ�ϸ�ŭ	(�ش� index�� Top-left����) ��� Inventory tile�� 
		loop ���鼭 ��ȿ�� ������ �ִ��� Ȯ���Ѵ�. �̹� �����ϰ� �ִٸ� false, ���ٸ� true�� ����.
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
				/* �ش� index�� �̹� item obj�� ������ false�� return�Ѵ� */
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

/* item obj�� top left index�� ������ item�� ũ�⸸ŭ 
	index�� loop���鼭 item�� �����Ҽ� �ִ��� Ȯ���Ѵ�.
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

/* Index�� grid�� x, y��ǥ�� �ٲ��ش�.*/
//FTile UNewInventoryComponent::IndexToTile(int32 index)
//{
//	FTile tile;
//	tile.X = index % Columns;
//	tile.Y = index / Columns;
//	return tile;
//}

/* grid�� x,y��ǥ�� index�� �ٲ۴�. */
//int32 UNewInventoryComponent::TileToIndex(FTile tile)
//{
//	return tile.X + tile.Y * Columns;	
//}

/* index�� item�� �ִ��� Ȯ���Ѵ�.*/
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
	* Item��, �ش� item�� Top-Left index�� ���� Map�� �������ش�.
	* Key���� ItemObject��, Value�� FTile�� �����ߴ�.
	*/
	TMap<UNewItemObject*, FTile> InventoryStoredInfo;
	
	//���� InventoryItems�� Garbage�� ���� 
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

bool UNewInventoryComponent::TryAddItem(UItemStorageObject* StorageObj, FItemSetting ItemSetting, UNewItemObject* Obj, bool bWantToGenerateRandomCount)
{
	bool bCreated = false;
	bool bAddCount = false;
	UNewItemObject* ItemObj = nullptr;

	if(Obj == nullptr)
	{
#ifdef DEBUG_INVCOMP
		//UE_LOG(LogTemp,Warning,TEXT("InvComp::TryAddItem / No Obj, call CreateObj func "));
#endif
		ItemObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated); //CreateObject(ItemSetting, bCreated);
	}
	else
	{
#ifdef DEBUG_INVCOMP
		//UE_LOG(LogTemp, Warning, TEXT("InvComp::TryAddItem / already has Obj"));
#endif
		ItemObj = Obj;
		bCreated = true;
	}


	if (bCreated)
	{
		
		if (bWantToGenerateRandomCount)
		{
			//LootBox(Chest)�� Item�� Spawn��, �Ǵ� World�� �ѷ��� Backpack, EnemyAI Character�� Item�� Spawn�� ������ �����ϰ� �ش�.
			UCustomInventoryLibrary::GenerateRandomCount(ItemObj);
		}
		else if (ItemObj->ItemInfo.DataAsset->bCanStack)
		{
			//Stack�� ������ Item�� ���, Storage���� ������ Item�� ���� AddCount�� �����ش�.
			const auto ItemInStorage = StorageObj->GetAllItems();
			for (auto Item : ItemInStorage)
			{
				if (Item.Key->ItemInfo.DataAsset != ItemObj->ItemInfo.DataAsset) continue;
				if(Item.Key->ItemInfo.DataAsset->bCanStack == false) continue;
				

				//ItemAddCount�� �ߴٸ� for���� break�Ѵ�.
				if (AddItemCount(ItemObj, Item.Key))
				{
					bAddCount = true;
					break;

				}
			}
		}
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


/*DroppedItem�� Count�� ��� Stack�� �ƴٸ� true�� �����ϰ�
* DroppedItem�� Count�� �������� false�� �����Ѵ�.
*/
bool UNewInventoryComponent::AddItemCount(UNewItemObject* DroppedItemObj, UNewItemObject* OnItemObj)
{
	int32 OnMAXStack = OnItemObj->ItemInfo.DataAsset->MaxStackSize;
	
	//Stack������ ������ ���Ѵ�.
	int32 OnCanStackCount = OnMAXStack - OnItemObj->ItemInfo.Count;
	int32 DroppedStackCount = DroppedItemObj->ItemInfo.Count;

	bool bReturn = false;

	// 1 �̻� ���� �� ���ٸ�, Stack������ count�� 0�̸� �� �ٷ� false�� �����Ѵ�.
	if (OnCanStackCount <= 0)
	{
		return false;
	}

	/*��������� Stack������ �������� ũ�ٸ�
	* OnItem(�ִ���)�� Stack������ ������ �־��ְ� DroppedItem(����Ѱ�)���� Stack ������ ������ ���ش�.
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

		//Item�� Spawn�̶� Storage�� ���� ��찡 �֤�����.
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

//ItemObj�� Count�� �����ϰ� ����� �Լ�
void UNewInventoryComponent::RemoveItemCount(UNewItemObject* RemoveItemObj, int32 RemoveCount)
{	
	int32 RemoveStackCount = FMath::Clamp<int32>(RemoveItemObj->ItemInfo.Count - RemoveCount, 0, RemoveItemObj->ItemInfo.Count - RemoveCount);
#ifdef DEBUG_INVCOMP
	UE_LOG(LogTemp,Warning,TEXT("InvComp::RemoveItemCnt / RemoveStackCount is %d"),RemoveStackCount);
#endif
	//�ִ��� - �������� 0���ϸ� RemoveItem�� �����Ѵ�.
	if (RemoveStackCount <= 0)
	{
		if (RemoveItemObj->MotherStorage)
		{
			//���� ��� ���θ� True�� �����Ѵ�. 
			/*(MotherStorage�� RemoveItem���� InventoryGrid Widget�� RefreshInvtory�Լ����� 
			* Quickslot ������ �ؾ��ϴ��� �Ǵ��Ҷ� ���Ǵ� bool������.
			* */
			RemoveItemObj->bIsPendingDelete = true;
			RemoveItemObj->RemoveCount(RemoveCount);
			RemoveItemObj->MotherStorage->RemoveItem(RemoveItemObj);
		}
		else
		{
			//MotherStorage�� ���� ��� -> Spawn�� Item�� ���(Craft, Pickup)
			//�׳� RemoveCount�� �����Ѵ�. ItemClass�� Destory�� Craft, Pickup���� ����.
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


//CustomInventoryLibrary�� �ű�.
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