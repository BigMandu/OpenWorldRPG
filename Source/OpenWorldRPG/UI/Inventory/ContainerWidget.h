// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContainerWidget.generated.h"

/**
 * 
 */

class AMainCharacter;
class AContainer;
class UNewInventoryComponent;
class UNewInventoryGrid;
class UUserWidget;



UCLASS()
class OPENWORLDRPG_API UContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryGrid* ContainerGridwidget;

	void InitContainerWidget(AActor* actor);

	AContainer* Container;
//	class ALootBox* InteractBox;
//
//	AMainCharacter* Main;
//	
//	TWeakObjectPtr<UInventoryComponent>BoxInventory;
//
////	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
//	UUserWidget* InventoryItemsWidget;
//
	
	
//
//	UFUNCTION()
//	void InitLootBox();
};
