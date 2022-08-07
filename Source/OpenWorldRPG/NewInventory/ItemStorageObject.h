// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "ItemStorageObject.generated.h"


/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryAdd, UNewItemObject*);


UCLASS(Blueprintable)
class OPENWORLDRPG_API UItemStorageObject : public UNewItemObject
{
	GENERATED_BODY()
public:
	FOnInventoryAdd OnInventoryAdd;

	/* Instance Editable, ReadOnly, Expose on Spawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 Columns = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 Rows = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
	float TileSize = 60.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
	TArray<UNewItemObject*> Inventory;



	/***** Function *****/

	FTile IndexToTile(int32 index);
	int32 TileToIndex(FTile tile);

	void InitializeStorage(int32 Var_X, int32 Var_Y, float Var_TileSize);

	UNewItemObject* GetItemAtIndex(int32 index);


	bool IsAvailableSpace(UNewItemObject* ItemObj, int32 TLind);
	bool TryAddItem(UNewItemObject* ItemObj);
	bool TryAddItemStep(UNewItemObject* ItemObj);
	void AddItemAtIndex(UNewItemObject* ItemObj, int32 Index);
	bool RemoveItem(UNewItemObject* ItemObj);
	
	const TMap<UNewItemObject*, FTile> GetAllItems();
	

	

};
