// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "LootBox.generated.h"

class UInventoryComponent;
class AItem;
class AMainCharacter;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLootBoxOpen);

UCLASS()
class OPENWORLDRPG_API ALootBox : public AInteractable
{
	GENERATED_BODY()
public:
	ALootBox();

	UPROPERTY(BlueprintAssignable, Category = Lootbox)
	FLootBoxOpen LootBoxOpen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LootBox)
	UInventoryComponent* BoxInvComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	int32 LootItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TSubclassOf<AItem> LootItem_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox)
	TSubclassOf<AItem> LootItem_2;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LootBox)
	TArray<TSubclassOf<AItem>> BoxItem;



	AMainCharacter* Main;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void SelectAndStoreLootItem();

	void OpenBox(AActor* Actor);
	void CloseBox(AActor* Actor);
	
};
