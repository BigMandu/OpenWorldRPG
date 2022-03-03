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
class UUserWidget;
class UNewItemObject;
class UTooltipWidget;
class USizeBox;
class UBorder;
class UImage;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoved, UObject*);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoved, TWeakPtr<UNewItemObject>, ItemObj);

UCLASS()
class OPENWORLDRPG_API UNewItemwidget : public UUserWidget
{
	GENERATED_BODY()

private:
	FVector2D widgetsize;
	const FLinearColor NormalColor = FLinearColor(0.0, 0.0, 0.0, 0.5);
	const FLinearColor HoverColor = FLinearColor(0.5, 0.5, 0.5, 0.2);
	
	UTooltipWidget* Tooltip;


	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WTooltipWidget = nullptr;
	
public:

	/* delegate, NewItemwidget::GetIconIamge,, NativeOnDragDetected ���� broad cast, 
	* NewInventoryGrid::RefreshInventory���� NewInventoryGrid::OnItemRemove
	* �� bind��Ŵ.
	* Item�� ���������� ȣ��ǵ���.
	*/
	FOnRemoved OnRemoved;
	

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BGSizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

	/* GridWidget���� data�� �־���*/
	UNewItemObject* ItemObj;
	float Tilesize;

	


private:
	
	FSlateBrush GetIconImage();

	void CreateTooltip();

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	//virtual void SynchronizeProperties() override;
	
	
	void Refresh();

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

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
