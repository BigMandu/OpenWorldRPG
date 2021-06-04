// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InventoryComponent.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AItem : public AInteractable
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ItemDescription;

	UInventoryComponent* OwningInventory;

	
	void Pickup(class AActor* Actor);
	
	void Drop();

	void Use(class AMainChrarcter* Main);
};
