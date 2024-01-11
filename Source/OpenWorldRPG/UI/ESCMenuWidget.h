// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ESCMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UESCMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	TWeakObjectPtr<class UOptionWidget> OptionWidget;
public:
	UPROPERTY(meta = (bindwidget))
	class UButton* ResumeBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* OptionBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* ToTitleBtn;

	/*UPROPERTY(meta = (bindwidget))
	class UBorder* OptionSlotBorder;*/

	/*UPROPERTY(meta = (bindwidget))
	class UNamedSlot* OptionSlot;*/

private:
	void ShowOptionWidget();
	UFUNCTION()
	void RemoveOptionWidget();

protected:
	virtual void NativeConstruct() override;
	//virtual void NativePreConstruct() override;

public:
	UFUNCTION()
	void ClickResumeBtn();
	UFUNCTION()
	void ClickOptionBtn();
	UFUNCTION()
	void ClickTitleBtn();


};
