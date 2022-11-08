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
class UCustomDDOperation;

class UNewInventoryGrid;
class UUserWidget;
class UNewItemObject;
class UTooltipWidget;
class USizeBox;
class UBorder;
class UImage;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoved, UObject*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDragDetect, UObject*);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoved, TWeakPtr<UNewItemObject>, ItemObj);

UCLASS()
class OPENWORLDRPG_API UNewItemwidget : public UUserWidget
{
	GENERATED_BODY()

private:
	//전체적인 이 Widget의 사이즈
	FVector2D widgetsize;

	//검은색
	const FLinearColor NormalColor = FLinearColor(0.0, 0.0, 0.0, 0.5);
	//붉은색
	const FLinearColor HoverColor = FLinearColor(0.5, 0.5, 0.5, 0.2);
	
	//Horver했을때 나오는 Tooltip widget
	UTooltipWidget* Tooltip;


	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WTooltipWidget = nullptr;
	
public:

	/* delegate, NewItemwidget::GetIconIamge,, NativeOnDragDetected 에서 broad cast, 
	* NewInventoryGrid::RefreshInventory에서 NewInventoryGrid::OnItemRemove
	* 와 bind시킴.
	* Item을 삭제했을때 호출되도록
	*/ 
	FOnRemoved OnRemoved;
	FOnDragDetect OnDragDetect;
	
	//UDragDropOperation* DDOper;
	//UCustomDDOperation* DDOper;

	

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BGSizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UTextBlock* ItemCount;

	/* GridWidget에서 data를 넣어줌*/
	UNewInventoryGrid* MotherContainer;
	UNewItemObject* ItemObj;
	float Tilesize;
	

	


private:
	FSlateBrush GetIconImage();
	void CreateTooltip();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	virtual bool Initialize() override;
	
	//virtual void SynchronizeProperties() override;
	
	
	void Refresh();// UNewItemObject* V_Obj, float V_Tilesize);// float var_tilesize);

	
	

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
