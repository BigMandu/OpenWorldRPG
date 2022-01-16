// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InventoryComponent.h"
#include "Item.generated.h"

/**
 * 
 */
class UNewItemObject;
class UMaterialInterface;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	UNewItemObject* ItemObj;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UMaterialInterface* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UMaterialInterface* IconRotated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ItemDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	EItemState ItemState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FIntPoint ItemSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bCanRotate;

	UInventoryComponent* OwningInventory;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	
	FORCEINLINE EItemState GetItemState() { return ItemState; }

	FORCEINLINE void SetItemState(EItemState State) { ItemState = State; }

	/* child class���� override�� ������*/
	UNewItemObject* GetDefaultItemObj();

	void Pickup(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	virtual void Drop();

	UFUNCTION(BlueprintCallable)
	void Use(AActor* Actor);
};
