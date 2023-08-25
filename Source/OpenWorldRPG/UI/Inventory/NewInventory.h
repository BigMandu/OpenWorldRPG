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

	//for Z order
	int32 HighestZ = 2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* MainCanvas;

	UEquipmentComponent* EquipComp;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StatusButton;
	UPROPERTY(meta = (BindWidget))
	UButton* InventoryButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CraftButton;
	UPROPERTY(meta = (BindWidget))
	UButton* IntelButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainInventorySwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* RightWidgetSwitcher;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ContentBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDropWidget* DropWidget;

	/*UPROPERTY(meta = (BindWidget))
	class UAdditionalWidget* AdditionalWidget;*/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UResourceStatusWidgetInInventory* ResourceStatusWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCharacterOverviewWidget* CharacterOverviewWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEquipStorageWidget* EquipmentStorageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEquipWidget* EquipmentWidget;	

	UPROPERTY(meta = (BindWidget))
	class UIntelAcquiredListWidget* IntelAcquiredListWidget;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<class UDraggInventoryWindow> WStorageWindow;
	
	TWeakObjectPtr<UWidget> CurrentHoldingWidget;
	bool bCanMoveWindow = false;
	FVector2D LastWindowPos;
	FVector2D LastMousePos;
	

	//Widget Container (Z-Order and prevent duplicate)
	UPROPERTY()
	TArray<UDraggInventoryWindow*> OpenedWindowArray;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	//TSubclassOf<class UWeaponPartsWidget> WWeaponPartsWindow;
	//UPROPERTY()
	//TArray<UWeaponPartsWidget*> WeaponPartsWindowArray;



public:
	UNewInventory(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ChangeMainSwitchToStatus();
	UFUNCTION()
	void ChangeMainSwitchToInventory();
	UFUNCTION()
	void ChangeMainSwitchToCraft();
	UFUNCTION()
	void ChangeMainSwitchToIntel();

	void SetRightWidget(UUserWidget* Widget);
	void ChangeRightSwitcher();
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void BindingOpenWidgetFunc(class UNewItemObject* ItemObj);
	void CreateAdditionalWidget(UNewItemObject* T_Obj);

	/* AdditionalWindow안에 들어갈 실제 Widget을 생성한다. */
	UWidget* CreateChildWidget(UDraggInventoryWindow* StorageWindow, UNewItemObject* Obj);
	bool CheckAlreadyOpened(UNewItemObject* T_Obj);


	
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//void AdditionalOtherWidget(UNewItemObject* ItemObj);

	UFUNCTION()
	void CloseAddiionalWidget(UDraggInventoryWindow* WindowWidget);

	UFUNCTION()
	void MoveZOrderTop(UDraggInventoryWindow* Window);

	UFUNCTION()
	void DraggingWindow(UDraggInventoryWindow* DraggingWindow, const FPointerEvent InMouseEvent);


	void CloseAllAdditionalWidget(int32 ArrayNum);

	//Container || NiceCar의 LootWidget과 bind하기 위한 함수
	void BindInteractLootBox(UNewInventoryGrid* LootBoxInvGrid);

	void BindInteractCharLoot(UEquipWidget* CharLoot);

	/*UFUNCTION()
	void ReleaseWindow();*/

	
};
