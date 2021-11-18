// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InventoryComponent.h"
#include "Item.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Spawn	UMETA(DisplayName = "Spawn"),
	EIS_Pickup	UMETA(DisplayName = "Pickup"),
	EIS_Drop	UMETA(DisplayName = "Drop"),

	EIS_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UCLASS()
class OPENWORLDRPG_API AItem : public AInteractable
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
public:

	AItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ItemDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Enums")
	EItemState ItemState;
	
	UInventoryComponent* OwningInventory;

	
	FORCEINLINE EItemState GetItemState() { return ItemState; }

	FORCEINLINE void SetItemState(EItemState State) { ItemState = State; }

	void Pickup(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	virtual void Drop();

	UFUNCTION(BlueprintCallable)
	void Use(AActor* Actor);
};
