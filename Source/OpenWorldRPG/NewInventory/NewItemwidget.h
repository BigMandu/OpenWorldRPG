// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "NewItemwidget.generated.h"

/**
 * 
 */
class UDragDropOperation;
class UNewInventoryGrid;
class UUserWidget;
class UNewItemObject;
class UTooltipWidget;
class USizeBox;
class UBorder;
class UImage;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoved, UObject*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDragDetect, UObject*);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoved, TWeakPtr<UNewItemObject>, ItemObj);

UCLASS()
class OPENWORLDRPG_API UNewItemwidget : public UUserWidget
{
	GENERATED_BODY()

private:
	//��ü���� �� Widget�� ������
	FVector2D widgetsize;

	//������
	const FLinearColor NormalColor = FLinearColor(0.0, 0.0, 0.0, 0.5);
	//������
	const FLinearColor HoverColor = FLinearColor(0.5, 0.5, 0.5, 0.2);
	
	//Horver������ ������ Tooltip widget
	UTooltipWidget* Tooltip;


	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WTooltipWidget = nullptr;
	
public:

	/* delegate, NewItemwidget::GetIconIamge,, NativeOnDragDetected ���� broad cast, 
	* NewInventoryGrid::RefreshInventory���� NewInventoryGrid::OnItemRemove
	* �� bind��Ŵ.
	* Item�� ���������� ȣ��ǵ���
	*/ 
	FOnRemoved OnRemoved;
	FOnDragDetect OnDragDetect;
	
	UDragDropOperation* DDOper;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BGSizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

	/* GridWidget���� data�� �־���*/
	UNewInventoryGrid* MotherContainer;
	UNewItemObject* ItemObj;
	float Tilesize;
	

	


private:
	
	FSlateBrush GetIconImage();

	void CreateTooltip();

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	//virtual void SynchronizeProperties() override;
	
	
	void Refresh();// float var_tilesize);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	//virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	/*FORCEINLINE_DEBUGGABLE WidgetT* ConstructWidget(TSubclassOf<UWidget>WidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None)
	{
		static_assert(TIsDerivedFrom<WidgetT, UWidget>::IsDerived, "WidgetTree::ConstructWidget can only create UWidget objects.");

		if (WidgetClass->IsChildOf<UUserWidget>())
		{
			return Cast<WidgetT>(CreateWidget(this, *WidgetClass, WidgetName));
		}

		return NewObject<WidgetT>(this, WidgetClass, WidgetName, RF_Transactional);
	}*/
};
