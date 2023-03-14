// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "QuickSlotSlotWidget.generated.h"

/**
 * 
 */

 class UBorder;
 class UImage;
 class UNewItemObject;
 class UNewItemwidget;

UCLASS()
class OPENWORLDRPG_API UQuickSlotSlotWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	UNewItemObject* MountedItemObj;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | QuickSlot")
	EQuickSlotType QuickSlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot | QuickSlot")
	EQuickSlotNumber QuickSlotNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWidget")
	TSubclassOf<UNewItemwidget> WItemWidget;

	UPROPERTY(meta=(BindWidget))
	UBorder* SlotBorder;

private:
	FORCEINLINE void SetMountedObject(UNewItemObject* ItemObj) {MountedItemObj = ItemObj;}
protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	void RegisterQuickSlot(UNewItemObject* WantToSlot);
	FORCEINLINE UNewItemObject* GetMountedObject() {return MountedItemObj;}
	void RemoveMountedObj();

	void SetWeaponQuickSlot(UNewItemObject* WantToSlot);
	
};
