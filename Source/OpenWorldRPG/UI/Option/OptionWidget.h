// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionWidget.generated.h"

/**
 * 
 */

 DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveWidget);

 class UCommonButton;

 

UCLASS()
class OPENWORLDRPG_API UOptionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnRemoveWidget OnRemoveWidget;
	
	UPROPERTY(meta = (bindwidget))
	UCommonButton* VideoBtn;
	UPROPERTY(meta = (bindwidget))
	UCommonButton* AudioBtn;
	UPROPERTY(meta = (bindwidget))
	UCommonButton* KeyBindBtn;

	UPROPERTY(meta = (bindwidget))
	UCommonButton* ApplyBtn;
	UPROPERTY(meta = (bindwidget))
	UCommonButton* CloseBtn;

	

	UPROPERTY(meta = (bindwidget))
	class UWidgetSwitcher* Switcher;

	UPROPERTY(meta = (bindwidget))
	class UVideoOption* VideoOptionWidget;
	UPROPERTY(meta = (bindwidget))
	class UAudioOption* AudioOptionWidget;
	UPROPERTY(meta = (bindwidget))
	class UKeybindOption* KeybindOptionWidget;

private:
	void ChangeWidgetSwitcher(int32 Switchindex);

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void ClickVideoButton();
	UFUNCTION()
	void ClickAudioButton();
	UFUNCTION()
	void ClickKeyBindButton();
	UFUNCTION()
	void ClickApplyButton();
	UFUNCTION()
	void ClickCloseButton();
};
