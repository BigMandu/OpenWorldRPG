// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Item.h"
#include "Equipment.generated.h"

/**
 * 
 */
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
public:

	AEquipment();

	virtual UNewItemObject* GetDefaultItemObj() override;

	AMainController* MainCon;
	AEnemyAIController* AICon;
	UEquipmentComponent* OwningEquipment;
	AActor* OwningPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	USkeletalMeshComponent* SKMesh;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	USoundCue* EquippedSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	bool bHasStorage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	//float GridTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TSubclassOf<UUserWidget> WEquipGridWidget;

	UNewInventoryGrid* EquipGridWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UNewInventoryComponent* EquipInventoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform MeshAttachTransform;
	

protected:
	virtual void BeginPlay() override;

public:
	void ReInitialize(UNewItemObject* Obj);
	void SetOwningPlayer(AActor* Actor);

	void SendToInventory(AActor* Actor);

	void SettingStorage();

	void StepEquip(AActor* Actor);
	virtual void Equip(AActor* Actor);
	
	virtual void Drop() override;

	virtual void Remove();

	/* Destory된 BeforeEquip과 동일한 장비를 SpawnActor로 spawn한뒤 해당 장비를 리턴한다.*/
	//AEquipment* SpawnEquip(UNewItemObject* Obj, AActor* Actor);
};
