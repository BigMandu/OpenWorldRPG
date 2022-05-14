// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "LootBox.generated.h"

class UNewInventoryComponent;
class UInventoryWidget;
class UNewInventoryGrid;
class ULootBoxWidget;
class UUserWidget;
class AItem;
class AMainController;

class UAIPerceptionStimuliSourceComponent;
class UAISense_Sight;
//class AMainCharacter;


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLootBoxOpen);

UCLASS()
class OPENWORLDRPG_API ALootBox : public AInteractable
{
	GENERATED_BODY()
public:
	ALootBox();

	/*UPROPERTY(BlueprintAssignable, Category = Lootbox)
	FLootBoxOpen LootBoxOpen;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LootBox)
	UNewInventoryComponent* BoxInventoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = LootBox)
	TSubclassOf<ULootBoxWidget>WLootBoxWidget;
	
	ULootBoxWidget* LootBoxWidget;

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
	AMainController* MainCon;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//void SelectAndStoreLootItem();
	virtual void PostInitializeComponents() override;
	void OpenBox(AActor* Actor);
	void ShowWidget();
	//void CloseBox(AActor* Actor);
	
};
