// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Container.generated.h"

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
class OPENWORLDRPG_API AContainer : public AInteractable
{
	GENERATED_BODY()
public:
	AContainer();

	/*UPROPERTY(BlueprintAssignable, Category = Lootbox)
	FLootBoxOpen LootBoxOpen;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LootBox)
	UNewInventoryComponent* ContainerInventoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ULootWidgetComponent* LootWidgetComp;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = LootBox)
	//TSubclassOf<ULootBoxWidget>WLootBoxWidget;

	//ULootBoxWidget* LootBoxWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	bool bHasSpawnItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TArray<TSubclassOf<AItem>> SpawnItemList;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	UAIPerceptionStimuliSourceComponent* StimuliComp;

	

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	int32 LootItemCount;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TSubclassOf<AItem> LootItem_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TSubclassOf<AItem> LootItem_2;*/
	

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LootBox)
	TArray<TSubclassOf<AItem>> BoxItem;*/


	//UInventoryWidget* WidgetInv;

	//AMainCharacter* Main;
	//AMainController* MainCon;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//void SelectAndStoreLootItem();
	virtual void PostInitializeComponents() override;
	void OpenContainer(AActor* Actor);
	void ShowWidget(AMainController* MainCon);
	void SpawnItem();
	//void CloseBox(AActor* Actor);
	
};
