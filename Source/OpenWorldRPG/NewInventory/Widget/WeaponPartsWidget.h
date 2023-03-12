// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponPartsWidget.generated.h"

/**
 * 
 */



UCLASS()
class OPENWORLDRPG_API UWeaponPartsWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	//FOnEquipWeaponParts OnEquipWeaponParts;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNewItemwidget> WItemWidget;

	//Garbage collection������(��� �ȵ�, ���� �� ��ȿ�� ������ �����Ѱ�.) ���� Weak Pointer ���
	TWeakObjectPtr<class UNewItemObject> WeaponObj;
	//class UNewItemObject* WeaponObj;

	UPROPERTY(meta = (bindWidget))
	class UImage* WeaponImage;

	UPROPERTY(meta=(bindWidget))
	class UEquipmentSlot* MuzzleSlot;
	UPROPERTY(meta = (bindWidget))
	UEquipmentSlot* TacticalSlot;
	UPROPERTY(meta = (bindWidget))
	UEquipmentSlot* ScopeSlot;

private:
	void UpdatePartsSlotWidget(UEquipmentSlot* PartsSlot, UNewItemObject* PartsObj);

public:
	void WidgetInit(UNewItemObject* Var_WeaponObj);

	UFUNCTION()
	void RefreshWidget();

	/*
	void EquipWeaponParts(UNewItemObject* PartsObj);
	
	void CheckBeforeEquip(UNewItemObject* PartsObj);
	void RemoveWeaponParts(UNewItemObject* PartsObj);
	*/
};
