// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntelPageBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UIntelPageBaseWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (bindwidget))
	class UCanvasPanel* PanningCanvas;
	UPROPERTY(meta = (bindwidget))
	class UOverlay* PaperOverlay;
	UPROPERTY(meta = (bindwidget))
	class UButton* ExitButton;
	UPROPERTY(meta = (bindwidget))
	UButton* NextButton;
	UPROPERTY(meta = (bindwidget))
	UButton* PrevButton;

	UPROPERTY()
	TWeakObjectPtr<UIntelAcquiredListWidget> ListWidget;

	float CurrentScaleX;
	float CurrentScaleY;
	bool bMouseButtonDown;

protected:

	//virtual void NativeConstruct() override;
	/* keyboard event*/
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/* mouse event */
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	void PanningWidget(const FPointerEvent& InMouseEvent);

public:
	void SettingFocus();
};
