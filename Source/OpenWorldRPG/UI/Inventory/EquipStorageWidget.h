// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "EquipStorageWidget.generated.h"

/**
 * 
 */

class UNewInventory;
class ABaseCharacter;
class UEquipmentSlot;
class UNewInventoryGrid;
class UItemStorageObject;
class UNewItemObject;
class UEquipmentComponent;

class UOverlay;






UCLASS()
class OPENWORLDRPG_API UEquipStorageWidget : public UUserWidget// , public IItemInterface
{
	GENERATED_BODY()
public:



	UPROPERTY()
	UNewInventory* MainWidget;

	UPROPERTY()
	ABaseCharacter* LootedChar_Owner;


	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* VestSlot;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* BackpackSlot;


	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UOverlay* VestOverlay;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UOverlay* BackpackOverlay;
	UPROPERTY(meta = (BindWidget))
	UNewInventoryGrid* PocketWidget;
	UPROPERTY(meta = (BindWidget))
	UNewInventoryGrid* SecureBoxWidget;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* SBVB;


	UPROPERTY()
	UEquipmentComponent* EquipComp;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WNewItemWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WGridInvWidget = nullptr;

	void EquipInitialize(UEquipmentComponent* p_EquipComp);
	void InitializeInventory(ABaseCharacter* BChar);


	UFUNCTION()
	void RefreshEquipWidget();

	void SetSlot(UNewItemObject* EquipObj, UEquipmentSlot* EquipSlot);
	void SettingStorageWidget(UOverlay* EquipOverlay, UItemStorageObject* Var_StorageObj);
	void RemoveSlot();

};
