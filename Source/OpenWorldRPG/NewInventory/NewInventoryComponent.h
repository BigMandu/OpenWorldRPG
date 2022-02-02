// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenWorldRPG/NewInventory/InventoryStruct.h"
#include "NewInventoryComponent.generated.h"

class UNewItemObject;

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoved, UObject*);
DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UNewInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNewInventoryComponent();

	FOnInventoryUpdated OnInventoryUpdated;

	/* Instance Editable, ReadOnly, Expose on Spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Constants")
	int32 Columns = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Constants")
	int32 Rows = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Constants")
	float TileSize = 50.f;

	

	TArray<UNewItemObject*> InventoryItems;

private:
	UNewItemObject* GetItemAtIndex(int32 index);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool AddItem(UNewItemObject* ItemObj);

	bool RemoveItem(UNewItemObject* ItemObj);

	bool IsAvailableSpace(UNewItemObject* ItemObj, int32 TopLeftIndex);

	
	/* FTile -> Inventory Struct */
	FTile IndexToTile(int32 index);
	int32 TileToIndex(FTile tile);

	FTile ForEachIndex(UNewItemObject* Obj, int32 TopLeftIndex);

	const TMap<UNewItemObject*,FTile> GetAllItems();
};
