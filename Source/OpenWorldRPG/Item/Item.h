// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InventoryComponent.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "Item.generated.h"

/**
 * 
 */
class UNewItemObject;
class UMaterialInterface;
class UAIPerceptionStimuliSourceComponent;
class ABaseCharacter;

//UENUM(BlueprintType)
//enum class EEquipmentType : uint8
//{
//	EET_Rifle		UMETA(DisplayName = "Rifle"),
//	EET_Pistol		UMETA(DisplayName = "Pistol"),
//	EET_Helmet		UMETA(DisplayName = "Helmet"),
//	EET_Plate		UMETA(Displayname = "Plate Carrier"),
//	EET_Vest		UMETA(DisplayName = "Vest"),
//	EET_Backpack	UMETA(DisplayName = "Backpack"),
//
//	EET_MAX			UMETA(DisplayName = "DefaltMAX")
//};


UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Spawn	UMETA(DisplayName = "Spawn"),
	EIS_Pickup	UMETA(DisplayName = "Pickup"),

	EIS_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UCLASS()
class OPENWORLDRPG_API AItem : public AInteractable //, public IItemInterface
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


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bCanEquip;


	UPROPERTY(EditDefaultsOnly, Category = AI)
	UAIPerceptionStimuliSourceComponent* StimuliComp;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	virtual void PostInitializeComponents() override;

	FORCEINLINE EItemState GetItemState() { return ItemState; }

	FORCEINLINE void SetItemState(EItemState State) { ItemState = State; }

	/* child class���� override�� ������*/
	virtual UNewItemObject* GetDefaultItemObj();

	bool Pickup(AActor* Actor);

	bool AddAtBackPack(ABaseCharacter* BChar);
	bool AddAtVest(ABaseCharacter* BChar);
	bool AddAtPocket(ABaseCharacter* BChar);
	bool AddAtSecureBox(ABaseCharacter* BChar);
	
	UFUNCTION(BlueprintCallable)
	virtual void Drop();

	UFUNCTION(BlueprintCallable)
	void Use(AActor* Actor);
};
