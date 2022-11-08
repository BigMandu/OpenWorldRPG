// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

FTile UItemStorageObject::IndexToTile(int32 index)
{
	FTile tile;
	tile.X = index % Columns;
	tile.Y = index / Columns;
	return tile;
}

/* grid�� x,y��ǥ�� index�� �ٲ۴�. */
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

bool UItemStorageObject::CheckValid(UNewItemObject* VarObj, int32 Varind)
{
	//Index�� 0 �̸�,
	//Try Add�ϴ� Obj�� �ڱ� �ڽ�,
	//Try Add�ϴ� Obj�� �ڽ��� ����ִ� Storage,
	
	//������ �ϳ��� �ش��ϸ� Add�� �� ����.
	if(Varind < 0 || this == VarObj || VarObj == MotherStorage)
	{
		return false;
	}
	return true;
}

// @TLind = TopLeftindex.
bool UItemStorageObject::IsAvailableSpace(UNewItemObject* ItemObj, int32 TLind)
{
	/*
	Inventory�� Top-Left���� item�� add�Ҷ�
	�� Item�� ������ Ÿ�ϸ�ŭ	(�ش� index�� Top-left����) ��� Inventory tile��
	loop ���鼭 ��ȿ�� ������ �ִ��� Ȯ���Ѵ�. �̹� �����ϰ� �ִٸ� false, ���ٸ� true�� ����.
	*/

	if (CheckValid(ItemObj, TLind) == false)
	{
		return false;
	}

	FTile tile = IndexToTile(TLind);
	FIntPoint Itemsize = ItemObj->GetItemSize();

	FTile checktile;
	checktile.X = 0; checktile.Y = 0;

	int32 HorizontalMAX = tile.X + Itemsize.X - 1;
	int32 VerticalMAX = tile.Y + Itemsize.Y - 1;

	//�������� ����ũ�Ⱑ Inventory�� �������� ������� Ȯ��.
	if (HorizontalMAX >= Columns || VerticalMAX >= Rows || HorizontalMAX < 0 || VerticalMAX < 0) return false;

	for (int32 i = tile.X; i <= HorizontalMAX; ++i)
	{
		for (int32 j = tile.Y; j <= VerticalMAX; ++j)
		{
			checktile.X = i;
			checktile.Y = j;

			//���� üũ�ϴ� �κ���  �ѹ��̶� �����ϸ� �� false�����ϰ� �ϴ°� �����غ�����.
			if ((checktile.X >= 0 && checktile.Y >= 0) && (checktile.X < Columns && checktile.Y < Rows))
			{
				int32 index = TileToIndex(checktile);
				
				UNewItemObject* ExistItemObj = GetItemAtIndex(index);
				if (ExistItemObj != nullptr && ExistItemObj != ItemObj)
				{
					return false;
				}
			}
		}
	}
	return true;// bReturn;
}


bool UItemStorageObject::TryAddItem(UNewItemObject* ItemObj, bool bWantToGenerateRandomCount)
{
	bool bResult = false;

	if (ItemObj)
	{
		bResult = TryAddItemStep(ItemObj);

		/* Item�߰��� �����߰� Item�� ���� �� �ִٸ� */
		if (!bResult && ItemObj->bCanRotated)
		{
			/* Item�� ������ �ٽ� �߰��Ѵ�. */
			ItemObj->ItemRotate();
			bResult = TryAddItemStep(ItemObj);

			/* �׷��� �����ߴٸ� ���� item�� �ٽ� ���� ��Ų��.*/
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
	//Add�ϱ� ����, ������ MotherStorage�� �ִٸ� �ű⼭ �����ش�.
	if (ItemObj->MotherStorage)
	{
		ItemObj->MotherStorage->RemoveItem(ItemObj);
	}
	else if (ItemObj->MotherEquipComp)
	{
		ItemObj->MotherEquipComp->RemoveEquipment(ItemObj);
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

	
	/*	AddItem�� �������̸� UNewInventoryGrid::RefreshInventory�� ȣ���ϱ� ����
		Delegate�� �����ϰ� broadcast�� �����ش�.
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
			ItemObj->bIsPendingDelete = true;
		}
	}
	OnInventoryAdd.Broadcast(ItemObj);
	return true;
}

const TMap<UNewItemObject*, FTile> UItemStorageObject::GetAllItems()
{
	/*
* Item��, �ش� item�� Top-Left index�� ���� Map�� �������ش�.
* Key���� ItemObject��, Value�� FTile�� �����ߴ�.
*/
	TMap<UNewItemObject*, FTile> InventoryStoredInfo;

	//���� InventoryItems�� Garbage�� ���� 
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
