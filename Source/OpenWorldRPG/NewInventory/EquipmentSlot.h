// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "EquipmentSlot.generated.h"


/* 
* 단순히, Image와 Border색상만을 변경한다.

*/
class UImage;
class UBorder;
class UNewItemObject;

UCLASS()
class OPENWORLDRPG_API UEquipmentSlot : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetVariable")
	EEquipmentType SlotType;

	//UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	//UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;


	void PaintBGBorder(UNewItemObject* Obj = nullptr);


	bool IsSupportedEquip(UNewItemObject* Obj);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
