// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
 //For enum iterator
ENUM_RANGE_BY_COUNT(EQuickSlotNumber, EQuickSlotNumber::EQSN_MAX)

class UNewItemObject;
class UQuickSlotSlotWidget;

UCLASS()
class OPENWORLDRPG_API UQuickSlotWidget : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_Primary;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_Sub;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_Pistol;

	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_4;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_5;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_6;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_7;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_8;
	UPROPERTY(meta = (BindWidget))
	UQuickSlotSlotWidget* QSlot_9;

private:
	
public:
	void BindSlotWidget(class AMainCharacter* Player);
	UQuickSlotSlotWidget* GetQuickSlotSlot(EQuickSlotNumber QuickSlotNum);
	void SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj);

	bool CheckCanBeRegister(UNewItemObject* WantToSlot);
	void JudgeCanbeInQuickSlotOrUpdate(UNewItemObject* ItemObj);
	void CheckAlreadyRegistered(UNewItemObject* ItemObj);
	
	UFUNCTION()
	void UseItemInQuickSlot(EQuickSlotNumber QuickSlotNum);
	
};
