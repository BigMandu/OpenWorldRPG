// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftItemWidget.generated.h"

/**
 * 
 */

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnCraftWidgetClicked,UBasePDA);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftWidgetClicked, UCraftItemWidget*, CraftWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDropCraftWidget, UCraftItemWidget*, CraftWidget, UCustomDDOperation*, CustomDDOper);

UCLASS()
class OPENWORLDRPG_API UCraftItemWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	bool bCraftItem = false;
public:

	FOnCraftWidgetClicked OnCraftWidgetClicked;
	FOnDropCraftWidget OnDropCraftWidget;

	TWeakObjectPtr<UBasePDA> CraftItemDA;

	UPROPERTY(meta = (bindwidget))
	class UBorder* ItemBorder;

	UPROPERTY(meta = (bindwidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (bindwidget))
	class UImage* Thumbnail;

	UPROPERTY(meta = (bindwidget))
	class UHorizontalBox* CountBox;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* CurrentCount;

	UPROPERTY(meta = (bindwidget))
	UTextBlock* NeedCount;

	int32 CurrentCount_num;
	int32 NeedCount_num;
	FName MotherItemName;

protected:
	virtual void NativeConstruct() override;
public:
	
	void InitCraftItemWidget(UBasePDA* CraftableItemPDA, bool bIsCraftItem, FName MotherName = NAME_None);
	void SetBorderColor(FLinearColor Color);
	void SetCurrentCountTextBox(int32 NumberofCount);
	void SetNeedCountTextBox(int32 NumberofCount);

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	

};
