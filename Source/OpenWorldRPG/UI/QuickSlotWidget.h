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
	void SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj, bool bIsNeedOnlyDeleted);

	bool CheckCanBeRegister(UNewItemObject* WantToSlot);
	void JudgeCanbeInQuickSlotOrUpdate(UNewItemObject* ItemObj);

	/** �� �Լ��� this::SetItemInQuickSlot �Լ�, NewInventoryGrid::RefreshInventroy �Լ����� ȣ��Ǵ� �Լ���.
	* 
	* �̹� ��ϵǾ� �ִ��� Ȯ���ϰ� �̹� ���  �Ǿ� �ִٸ�, ��ϵ� Slot���� �����Ѵ�.
	* �ٸ�, WantToQuickSlot�� ���Ͽ� �̹� ��ϵ� Quickslot�� �� ����Ѵٸ�, ������ �����ϵ��� false�� �����Ѵ�.
	* NewInventoryGrid Ŭ�������� Referesh�Ҷ� ���� �� �Լ��� ȣ���ϴµ�, �� ���� ������ ������ �� �ֵ��� bNeedToPreventRemove �Ķ���͸� �����Ѵ�.
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
