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
	
	//해당InventoryComp의 Inventory배열사이즈를 설정한다.
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
	* 이 ItemObj의 크기를 차지할 타일을 반복한다.
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
				/* AddItem이 성공적이면 UNewInventoryGrid::RefreshInventory를 호출하기 위해
				Delegate를 생성하고 broadcast를 때려준다.
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
		Inventory의 Top-Left부터 item을 add할때 
		이 Item이 차지할 타일만큼	(해당 index의 Top-left부터) 모든 Inventory tile을 
		loop 돌면서 유효한 공간이 있는지 확인한다. 이미 차지하고 있다면 false, 없다면 true를 리턴.
	*/
	bool bReturn = false;
	FTile tile = ForEachIndex(ItemObj, TopLeftIndex);
	if ((tile.X >= 0 && tile.Y >= 0) && (tile.X < Columns && tile.Y < Rows))
	{
		int32 index = TileToIndex(tile);
		UNewItemObject* GettingItemObj = GetItemAtIndex(index);
		/* 해당 index에 이미 item obj가 있으면 false를 return한다 */
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

/* Index를 grid의 x, y좌표로 바꿔준다.*/
FTile UNewInventoryComponent::IndexToTile(int32 index)
{
	FTile tile;
	tile.X = index % Columns;
	tile.Y = index / Columns;
	return tile;
}

/* grid의 x,y좌표를 index로 바꾼다. */
int32 UNewInventoryComponent::TileToIndex(FTile tile)
{
	return tile.X + tile.Y * Columns;	
}

/* index에 item이 있는지 확인한다.*/
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
	* Item과, 해당 item의 Top-Left index를 담은 Map을 리턴해준다.
	* Key값을 ItemObject로, Value를 FTile로 지정했다.
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


