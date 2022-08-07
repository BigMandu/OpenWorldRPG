// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"


FTile UItemStorageObject::IndexToTile(int32 index)
{
	FTile tile;
	tile.X = index % Columns;
	tile.Y = index / Columns;
	return tile;
}

/* grid의 x,y좌표를 index로 바꾼다. */
int32 UItemStorageObject::TileToIndex(FTile tile)
{
	return tile.X + tile.Y * Columns;
}

void UItemStorageObject::InitializeStorage(int32 Var_X, int32 Var_Y, float Var_TileSize)
{
	Columns = Var_X;
	Rows = Var_Y;
	TileSize = Var_TileSize;

	Inventory.Init(nullptr, Columns * Rows);

}

UNewItemObject* UItemStorageObject::GetItemAtIndex(int32 index)
{
	UNewItemObject* ReturnObj = nullptr;
	if (index >= 0 && index < Inventory.Num())
	{
		ReturnObj = Inventory[index];
	}
	return ReturnObj;
}

// @TLind = TopLeftindex.
bool UItemStorageObject::IsAvailableSpace(UNewItemObject* ItemObj, int32 TLind)
{
	/*
	Inventory의 Top-Left부터 item을 add할때
	이 Item이 차지할 타일만큼	(해당 index의 Top-left부터) 모든 Inventory tile을
	loop 돌면서 유효한 공간이 있는지 확인한다. 이미 차지하고 있다면 false, 없다면 true를 리턴.
	*/
	FTile tile = IndexToTile(TLind);
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
				int32 index = TileToIndex(checktile);
				//UNewItemObject* GettingItemObj = GetItemAtIndex(index);
				if (GetItemAtIndex(index) != nullptr)
				{
					return false;
				}
			}
		}
	}
	return true;// bReturn;
}


bool UItemStorageObject::TryAddItem(UNewItemObject* ItemObj)
{
	bool bResult = false;

	if (ItemObj)
	{
		bResult = TryAddItemStep(ItemObj);

		/* Item추가에 실패했고 Item을 돌릴 수 있다면 */
		if (!bResult && ItemObj->bCanRotated)
		{
			/* Item을 돌리고 다시 추가한다. */
			ItemObj->ItemRotate();
			bResult = TryAddItemStep(ItemObj);

			/* 그래도 실패했다면 돌린 item을 다시 원복 시킨다.*/
			if (!bResult)
			{
				ItemObj->ItemRotate();
			}
		}
	}
	return bResult;
}

bool UItemStorageObject::TryAddItemStep(UNewItemObject* ItemObj)
{
	bool bResult = false;
	if (ItemObj)
	{
		for (int32 iter = 0; iter < Inventory.Num(); ++iter)
		{
			bResult = IsAvailableSpace(ItemObj, iter);
			if (bResult)
			{
				AddItemAtIndex(ItemObj, iter);
				//SetItemState(EItemState::EIS_Pickup);
				return bResult;
			}
		}
	}
	return bResult;
}

void UItemStorageObject::AddItemAtIndex(UNewItemObject* ItemObj, int32 Index)
{
	//Add하기 전에, 이전에 MotherStorage가 있다면 거기서 지워준다.
	if (ItemObj->MotherStorage)
	{
		ItemObj->MotherStorage->RemoveItem(ItemObj);
	}
	ItemObj->TopLeftIndex = Index;
	ItemObj->MotherStorage = this;
	ItemObj->bIsDestoryed = true;

	FTile tile = IndexToTile(Index);
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
			//Range check
			if ((Itemtile.X >= 0 && Itemtile.Y >= 0) && (Itemtile.X < Columns && Itemtile.Y < Rows))
			{
				int32 Addindex = TileToIndex(Itemtile);
				Inventory[Addindex] = ItemObj;
			}
		}
	}
#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Success Add Item"));
#endif

	
	/*	AddItem이 성공적이면 UNewInventoryGrid::RefreshInventory를 호출하기 위해
		Delegate를 생성하고 broadcast를 때려준다.
	*/
	OnInventoryAdd.Broadcast(ItemObj);
}

bool UItemStorageObject::RemoveItem(UNewItemObject* ItemObj)
{
	FIntPoint Size = ItemObj->GetItemSize();
	FTile ItemIndex = IndexToTile(ItemObj->TopLeftIndex);
	int32 loopX = ItemIndex.X + Size.X - 1;
	int32 loopY = ItemIndex.Y + Size.Y - 1;

	FTile removeTile;

	for (int32 i = ItemIndex.X; i <= loopX; i++)
	{
		for (int32 j = ItemIndex.Y; j <= loopY; j++)
		{

			removeTile.X = i;
			removeTile.Y = j;
			int32 removeIndex = TileToIndex(removeTile);

			if (Inventory[removeIndex] != ItemObj)
			{
				UE_LOG(LogTemp, Warning, TEXT("ItemStorageObj::RemoveItem : Some thing wrong.."));
				return false;
			}	
			Inventory[removeIndex] = nullptr;
		}
	}
	OnInventoryAdd.Broadcast(ItemObj);
	return true;
}

const TMap<UNewItemObject*, FTile> UItemStorageObject::GetAllItems()
{
	/*
* Item과, 해당 item의 Top-Left index를 담은 Map을 리턴해준다.
* Key값을 ItemObject로, Value를 FTile로 지정했다.
*/
	TMap<UNewItemObject*, FTile> InventoryStoredInfo;

	//여기 InventoryItems에 Garbage값 들어옴 
	for (int32 index = 0; index < Inventory.Num(); ++index)
	{
		UNewItemObject* CurItemObj = Inventory[index];
		if (CurItemObj != nullptr)
		{
			bool bAlreadyHaveit = InventoryStoredInfo.Contains(CurItemObj);
			if (bAlreadyHaveit == false)
			{
				FTile CurItemSize = IndexToTile(index);
				InventoryStoredInfo.Add(CurItemObj, CurItemSize);
			}
		}
	}
	return InventoryStoredInfo;
}
