// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "EquipWidget.generated.h"


/*
각 슬롯을 생성.
*/
class UEquipmentSlot;
class UEquipmentComponent;

UCLASS()
class OPENWORLDRPG_API UEquipWidget : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetVariable")
	EEquipmentType SlotType;*/
	
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
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* VestSlot;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UEquipmentSlot* BackpackSlot;

	UEquipmentComponent* EquipComp;

	void EquipInitialize(UEquipmentComponent* p_EquipComp);
	void RefreshEquipWidget();

	void Set


	//virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
