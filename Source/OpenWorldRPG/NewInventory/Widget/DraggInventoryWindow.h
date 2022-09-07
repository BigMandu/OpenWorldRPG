// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggInventoryWindow.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloseWindow,UDraggInventoryWindow*,CloseWindow);

UCLASS()
class OPENWORLDRPG_API UDraggInventoryWindow : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FOnCloseWindow OnCloseWindow;

	UPROPERTY(meta = (BindWidget))
	class UNewInventoryGrid* GridInventory;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StorageWidgetName;

	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	bool bMouseDown;

	FVector2D FirstMousePos;
	FVector2D MousePos;

	virtual void NativeConstruct() override;
	FReply DetectDragWidget(const FPointerEvent& InMouseEvent, UWidget* DragWidget);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UFUNCTION()
	void CloseWindow();
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
