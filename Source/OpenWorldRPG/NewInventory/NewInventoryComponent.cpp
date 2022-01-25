// Fill out your copyright notice in the Description page of Project Settings.


#include "NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Containers/Array.h"

#define DEBUG 1
// Sets default values for this component's properties
UNewInventoryComponent::UNewInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UNewInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//�ش�InventoryComp�� Inventory�迭����� �����Ѵ�.
	InventoryItems.SetNum(Columns * Rows);
	// ...
	
}

bool UNewInventoryComponent::RemoveItem(UNewItemObject* ItemObj)
{
#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::Remove"));
#endif
	return true;
}

bool UNewInventoryComponent::AddItem(UNewItemObject* ItemObj)
{
	/*
	* �� ItemObj�� ũ�⸦ ������ Ÿ���� �ݺ��Ѵ�.
	*/
	bool bResult = false;
	if (ItemObj)
	{
		for (int32 iter = 0; iter < InventoryItems.Num(); ++iter)
		{
			bResult = IsAvailableSpace(ItemObj, iter);
			if (bResult)
			{
				FTile tile;
				tile = ForEachIndex(ItemObj, iter);
				InventoryItems.Insert(ItemObj, iter);
#if DEBUG
				UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Success Add Item"));
#endif			
				/* AddItem�� �������̸� UNewInventoryGrid::RefreshInventory�� ȣ���ϱ� ����
				Delegate�� �����ϰ� broadcast�� �����ش�.
				*/
				//
				OnInventoryUpdated.Broadcast();

				return bResult;				
			}
		}
#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Inventory is full"));
#endif
	}
#if DEBUG
	UE_LOG(LogTemp, Warning, TEXT("NewInvComp::AddItem = Item Obj is invalid"));
#endif
	return bResult;
}

bool UNewInventoryComponent::IsAvailableSpace(UNewItemObject* ItemObj, int32 TopLeftIndex)
{
	/*
		Inventory�� Top-Left���� item�� add�Ҷ� 
		�� Item�� ������ Ÿ�ϸ�ŭ	(�ش� index�� Top-left����) ��� Inventory tile�� 
		loop ���鼭 ��ȿ�� ������ �ִ��� Ȯ���Ѵ�. �̹� �����ϰ� �ִٸ� false, ���ٸ� true�� ����.
	*/
	bool bReturn = false;
	FTile tile = ForEachIndex(ItemObj, TopLeftIndex);
	if ((tile.X >= 0 && tile.Y >= 0) && (tile.X < Columns && tile.Y < Rows))
	{
		int32 index = TileToIndex(tile);
		UNewItemObject* GettingItemObj = GetItemAtIndex(index);
		/* �ش� index�� �̹� item obj�� ������ false�� return�Ѵ� */
		if (GettingItemObj == nullptr)
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("NewInvComp::IsAvaSpace = Empty At index"));
#endif
			bReturn = true;
		}
		else
		{
#if DEBUG
			UE_LOG(LogTemp, Warning, TEXT("NewInvComp::IsAvaSpace = Not Empty At index"));
#endif
		}
	}
	else
	{
#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::IsAvaSpace = Tile is invalid, wrong range"));
#endif
	}
	return bReturn;
}


FTile UNewInventoryComponent::ForEachIndex(UNewItemObject* Obj, int32 TopLeftIndex)
{
	FTile tile = IndexToTile(TopLeftIndex);
	FIntPoint dimension = Obj->GetItemSize();
	FTile returntile;
	returntile.X = 0;
	returntile.Y = 0;

	int32 Outerendindex = tile.X + dimension.X;// -1;
	int32 Innerendindex = tile.Y + dimension.Y;
	
	for (int32 i = tile.X; i < Outerendindex; ++i)
	{
		returntile.X = i;
	}
	for (int32 j = tile.Y; j < Innerendindex; ++j)
	{
		returntile.Y = j;
	}
	return returntile;
}

/* Index�� grid�� x, y��ǥ�� �ٲ��ش�.*/
FTile UNewInventoryComponent::IndexToTile(int32 index)
{
	FTile tile;
	tile.X = index % Columns;
	tile.Y = index / Columns;
	return tile;
}

/* grid�� x,y��ǥ�� index�� �ٲ۴�. */
int32 UNewInventoryComponent::TileToIndex(FTile tile)
{
	return tile.X + tile.Y * Columns;	
}

/* index�� item�� �ִ��� Ȯ���Ѵ�.*/
UNewItemObject* UNewInventoryComponent::GetItemAtIndex(int32 index)
{
	UNewItemObject* ReturnObj = nullptr;
	if (index >= 0 && index < InventoryItems.Num())
	{
		ReturnObj = InventoryItems[index];
#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::GetItem = Success get Item Obj at index"));
#endif
	}
	else
	{
#if DEBUG
		UE_LOG(LogTemp, Warning, TEXT("NewInvComp::GetItem = Index is wrong range"));
#endif
	}
	return ReturnObj;
}

TMap<UNewItemObject*, FTile> UNewInventoryComponent::GetAllItems()
{
	/*
	* Item��, �ش� item�� Top-Left index�� ���� Map�� �������ش�.
	* Key���� ItemObject��, Value�� FTile�� �����ߴ�.
	*/
	TMap<UNewItemObject*, FTile> InventoryStoredInfo;
	

	for (int32 index = 0; index < InventoryItems.Num(); ++index)
	{
		UNewItemObject* CurItemObj = InventoryItems[index];
		bool bAlreadyHaveit = InventoryStoredInfo.Contains(CurItemObj);
		if (bAlreadyHaveit == false)
		{
			FTile CurItemSize = IndexToTile(index);
			InventoryStoredInfo.Add(CurItemObj, CurItemSize);
		}

	}

	//for (auto It = InventoryItems.CreateIterator(); It; ++It)
	//{

	//	bool bAlreadyHaveit = false; // InventoryStoredInfo.Contains(It);
	//	if (bAlreadyHaveit == false)
	//	{
	//		int32 index = It.GetIndex();
	//		FTile CurItemSize = IndexToTile(It.GetIndex());
	//		//InventoryStoredInfo.Add(It, CurItemSize);
	//	}
	//}

	return InventoryStoredInfo;
}


