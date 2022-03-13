// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewInventory.generated.h"

/**
 * 
 */
class UWidgetSwitcher;
class UNewInventoryComponent;
class UNewInventoryGrid;
class UDropWidget;
class ULootBoxWidget;
class UBorder;
class AMainCharacter;

UCLASS()
class OPENWORLDRPG_API UNewInventory : public UUserWidget
{
	GENERATED_BODY()
private:
	

public:
	AMainCharacter* Main;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryComponent* InventoryComp;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainInventorySwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* RightWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ContentBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryGrid* GridWidget;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDropWidget* DropWidget;

	//float TileSize;


public:
	UNewInventory(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void SetRightWidget(UUserWidget* Widget);
	void ChangeRightSwitcher();
	
};
