// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropWidget.generated.h"

/**
 * 
 */
class USizeBox;
class UBorder;
class UTextBlock;

UCLASS()
class OPENWORLDRPG_API UDropWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/*UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* DropSizeBox;*/
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* DropBorder;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UTextBlock* DropText;

	FText NormalText = FText::FromString(" ");
	FText DragDetectText = FText::FromString("Drop item here");

	bool bReturnNormal = false;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void ChangeState();
};
