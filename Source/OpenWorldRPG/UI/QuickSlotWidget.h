// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
 //For enum iterator
ENUM_RANGE_BY_COUNT(EQuickSlotNumber, EQuickSlotNumber::EQSN_MAX)

class UNewItemObject;
class UQuickSlotSlotWidget;

UCLASS()
class OPENWORLDRPG_API UQuickSlotWidget : public UUserWidget//, public IItemInterface
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
	void SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj, bool bIsNeedOnlyDeleted);

	bool CheckCanBeRegister(UNewItemObject* WantToSlot);
	void JudgeCanbeInQuickSlotOrUpdate(UNewItemObject* ItemObj);

	/** 이 함수는 this::SetItemInQuickSlot 함수, NewInventoryGrid::RefreshInventroy 함수에서 호출되는 함수다.
	* 
	* 이미 등록되어 있는지 확인하고 이미 등록  되어 있다면, 등록된 Slot에서 삭제한다.
	* 다만, WantToQuickSlot과 비교하여 이미 등록된 Quickslot에 또 등록한다면, 삭제만 진행하도록 false를 리턴한다.
	* NewInventoryGrid 클래스에서 Referesh할때 마다 이 함수를 호출하는데, 이 때는 삭제를 방지할 수 있도록 bNeedToPreventRemove 파라미터를 제공한다.
	* 
	* @ItemObj - Item object that want to register at quick slot.
	* @bNeedToPreventRemove - if true, always return false.
	* @WantToQuickNum - Quick slot number that want to register.
	* 
	* return value only affect when called from this class.
	* @return value - if false, below process will not proceed.
	*/
	bool CheckAlreadyRegistered(UNewItemObject* ItemObj, bool bIsOnlyDeleted, EQuickSlotNumber WantQuickNum = EQuickSlotNumber::EQSN_MAX);
	
	UFUNCTION()
	void UseItemInQuickSlot(EQuickSlotNumber QuickSlotNum);
	
	UFUNCTION()
	void SetWeaponQuickSlot(UNewItemObject* WantToSet);
	UFUNCTION()
	void RemoveWeaponQuickSlot(ERifleSlot WantToRemoveSlot);

};
