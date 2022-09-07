// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewInventory.generated.h"

/**
 * 
 */
class UWidgetSwitcher;
class UBorder;
class UScrollBox;

class UEquipmentComponent;
class UNewInventoryComponent;
class UNewInventoryGrid;
class UEquipWidget;
class UCharacterInventoryWidget;
class UDropWidget;
class ULootBoxWidget;

class AMainCharacter;

UCLASS()
class OPENWORLDRPG_API UNewInventory : public UUserWidget
{
	GENERATED_BODY()
private:
	

public:
	AMainCharacter* Main;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//UNewInventoryComponent* PocketInventoryComp;
	//UNewInventoryComponent* SecureBoxInventoryComp;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* MainCanvas;

	UEquipmentComponent* EquipComp;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainInventorySwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* RightWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ContentBorder;

	UPROPERTY(meta = (BindWidget))
	class UAdditionalWidget* AdditionalWidget;

	/*UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* RightWidgetScrollBox;*/

	/*UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryGrid* PocketWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UNewInventoryGrid* SecureBoxWidget;*/

	UPROPERTY(meta = (BindWidget))
	UCharacterInventoryWidget* CharInvWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEquipWidget* EquipmentWidget;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDropWidget* DropWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<class UDraggInventoryWindow> WStorageWindow;
	
	UPROPERTY()
	TArray<UDraggInventoryWindow*> InventoryWindowArray;
	//UDraggInventoryWindow* StorageWindow;

	//float TileSize;


public:
	UNewInventory(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void SetRightWidget(UUserWidget* Widget);
	void ChangeRightSwitcher();
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void BindingOpenWidgetFunc(class UNewItemObject* ItemObj);
	void OpenAdditionalWidget(class UItemStorageObject* StorageObj);
	
	UFUNCTION()
	void CloseAddiionalWidget(UDraggInventoryWindow* WindowWidget);

	
};
