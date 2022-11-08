// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "EquipmentSlot.generated.h"


/* 
* �ܼ���, Image�� Border������ �����Ѵ�.

*/
class UImage;
class UBorder;
class UNewItemObject;

class ABaseCharacter;



UCLASS()
class OPENWORLDRPG_API UEquipmentSlot : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | EquipSlot")
	EEquipmentType SlotType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | RifleSlot")
	bool bIsforRifleSlot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | RifleSlot")
	ERifleSlot RifleSlotType;

	//UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	//UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UBorder* BGBorder;

	UNewItemObject* SettedObj;

	ABaseCharacter* LootedChar_Owner;

private:

	bool IsEmpty();

protected:

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

	void PaintBGBorder(UNewItemObject* ItemObj = nullptr);
	bool IsSupportedEquip(UNewItemObject* ItemObj);
	
	bool TrySlotEquip(UNewItemObject* Var_ItemObj);

	void RegisterQuickSlot(UNewItemObject* WantToSlot);
	

};
