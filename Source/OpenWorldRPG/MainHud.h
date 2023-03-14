// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHud.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UMainHud : public UUserWidget
{
	GENERATED_BODY()
public:
	class AMainController* MainCon;
	
	bool bIsCompassWidgetVisible = false;
	bool bIsInventoryWindowShow = false;

	UPROPERTY(meta = (BindWidget))
	class UNewInventory* NewInventoryWidget;

	UPROPERTY(meta = (BindWidget))
	class UResourceStatusWidget* ResourceWidget;
	UPROPERTY(meta = (BindWidget))
	class UWeaponStatusWidget* WeaponStatus;
	UPROPERTY(meta = (BindWidget))
	class UQuickSlotWidget* QuickSlot;

	UPROPERTY(meta = (BindWidget))
	class UCompassWidget* CompassWidget;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	TSubclassOf<UUserWidget> WInteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	UUserWidget* InteractText;

	//for ZOrder
	int32 HighestZ = 2;

private:
	void ShowOtherUI();
	void HideOtherUI();

protected:
	void SetFocus(UUserWidget* Focus);


public:
	//Initialize보다 먼저 실행됨.
	virtual void NativeConstruct() override;

	void ShowInventoryWindow();
	void HideInventoryWindow();

	void ShowInteractText();
	void HideInteractText();

	void ShowCompassWidget();
	void HideCompassWidget();

	void SettingIsCompassWidgetVisible(bool bIsVisible);
	

};
