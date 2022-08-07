// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Container.generated.h"


class UItemStorageObject;
class UNewInventoryComponent;
class ULootWidgetComponent;
class UInventoryWidget;
class UNewInventoryGrid;
class UContainerWidget;
class UUserWidget;

class UAIPerceptionStimuliSourceComponent;
class UAISense_Sight;

class AItem;
class AMainController;



//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLootBoxOpen);

UCLASS()
class OPENWORLDRPG_API AContainer : public AActor, public IInteractive_Interface
{
	GENERATED_BODY()
public:
	AContainer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LootBox)
	UNewInventoryComponent* ContainerInventoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ULootWidgetComponent* LootWidgetComp;

	/* Storage */
	UPROPERTY()
	UItemStorageObject* ContainerStorage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Storage)
	int32 StorageX = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Storage)
	int32 StorageY = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Storage)
	float StorageTileSize = 60.f;

	/* Spawn Items */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	bool bHasSpawnItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TArray<TSubclassOf<AItem>> SpawnItemList;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	UAIPerceptionStimuliSourceComponent* StimuliComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//void SelectAndStoreLootItem();
	virtual void PostInitializeComponents() override;

	virtual void Interaction(AActor* Actor) override;

	void SettingStorage();
	void OpenContainer(AActor* Actor);
	void ShowWidget(AMainController* MainCon);
	void SpawnItem();
	//void CloseBox(AActor* Actor);
	virtual void SetOutline() override;
	virtual void UnsetOutline() override;
};
