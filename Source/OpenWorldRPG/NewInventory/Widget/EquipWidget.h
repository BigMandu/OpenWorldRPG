// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "EquipWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenAdditionalWidget_Equip, UNewItemObject*, ItemObj);
/*
각 슬롯을 생성.
*/
class ABaseCharacter;
class AEquipment;

class UNewInventory;
class UEquipmentSlot;
class UEquipmentComponent;
class UNewItemObject;
class UItemStorageObject;

class UOverlay;
class UBorder;

//ENUM_RANGE_BY_COUNT(EEquipSlot, EEquipSlot::EES_MAX)

UCLASS()
class OPENWORLDRPG_API UEquipWidget : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetVariable")
	EEquipmentType SlotType;*/

	FOpenAdditionalWidget_Equip OpenAdditionalWidget_Equip;

	UPROPERTY()
	UNewInventory* MainWidget;

	UPROPERTY()
	ABaseCharacter* LootedChar_Owner;

	/*UPROPERTY(VisibleAnywhere)
	EEquipSlot EquipSlot;*/
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* HelmetSlot;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* PlateSlot;


	

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* PistolSlot;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* PrimarySlot;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* SubSlot;


	//삭제 대기중
	/*UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
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
	UNewInventoryGrid* SecureBoxWidget;*/


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


	//UFUNCTION()
	//void RemoveEquipment(UObject* T_ItemObj);

	//Old Version
	//void SetSlot(AEquipment* Equip, UEquipmentSlot* EquipSlot);
	
	//New Version
	void SetSlot(UNewItemObject* EquipObj, UEquipmentSlot* EquipSlot);

	void SettingStorageWidget(UOverlay* EquipOverlay, UItemStorageObject* Var_StorageObj);

	void RemoveSlot();


	

	//virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
