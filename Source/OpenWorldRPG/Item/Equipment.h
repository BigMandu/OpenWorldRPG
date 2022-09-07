// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Item.h"
#include "Equipment.generated.h"

/**
 * 
 */
class ABaseCharacter;
class AMainController;
class AEnemyAIController;
class USkeletalMeshComponent;
class UEquipmentComponent;
class USoundCue;
class UNewInventoryComponent;
class UNewInventoryGrid;


//UENUM(BlueprintType)
//enum class EEquipmentType : uint8
//{
//	EET_Rifle		UMETA(DisplayName = "Rifle"),
//	EET_Pistol		UMETA(DisplayName = "Pistol"),
//	EET_Helmet		UMETA(DisplayName = "Helmet"),
//	EET_Vest		UMETA(DisplayName = "Vest"),
//	EET_Backpack	UMETA(DisplayName = "Backpack"),
//
//	EET_MAX			UMETA(DisplayName = "DefaltMAX")
//};

UCLASS()
class OPENWORLDRPG_API AEquipment : public AItem
{
	GENERATED_BODY()

private:

	//TSubclassOf<UNewInventoryGrid>WEquipGridWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	USoundCue* EquippedSound;

protected:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform MeshAttachTransform;


public:

	AEquipment();

	AMainController* MainCon;
	AEnemyAIController* AICon;
	UEquipmentComponent* OwningEquipment;
	ABaseCharacter* OwningPlayer;
	
	

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;*/


	/********* Storage ********/
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage")
	bool bHasStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage")
	float GridTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment | Storage")
	int32 Columns = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment | Storage")
	int32 Rows = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage")
	TSubclassOf<UUserWidget> WEquipGridWidget;


	UNewInventoryGrid* EquipGridWidget;

	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage")
	UNewInventoryComponent* EquipBaseComponent;
	/* Storage */
	UPROPERTY()
	UItemStorageObject* StorageObj;

	/* Spawn Items */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment | Storage")
	bool bHasSpawnItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment | Storage")
	TArray<TSubclassOf<AItem>> SpawnItemList;
	
private:
	void SendToInventory(AActor* Actor);
	

protected:
	virtual void BeginPlay() override;
	
public:
	virtual UNewItemObject* GetDefaultItemObj() override;
	
	void ReInitialize(UNewItemObject* Obj);

	void SettingStorage();
	void SpawnItem();

	void SwapBetweenInvAndEquipped(ABaseCharacter* BChar, UNewItemObject* ToInventory);
	void SwapBetweenEquipped(ABaseCharacter* BChar, UNewItemObject* BeforeEquipped);

	bool Equip(AActor* Actor, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);
	virtual bool StepEquip(AActor* Actor, ERifleSlot RifleSlot = ERifleSlot::ERS_MAX);

	void SetOwningPlayer(AActor* Actor);
	AActor* GetOwningPlayer();

	
	virtual void Drop() override;
	virtual void Remove();

	
};
