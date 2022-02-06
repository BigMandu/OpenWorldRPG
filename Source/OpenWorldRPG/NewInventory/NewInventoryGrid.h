// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/NewInventory/InventoryStruct.h"
#include "Blueprint/UserWidget.h"

//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "NewInventoryGrid.generated.h"


/**
 * 
 */
class UBorder;
class UCanvasPanel;
class UNewInventoryComponent;
class UNewItemObject;
class UNewItemwidget;
class AMainCharacter;
class AMainController;


UCLASS()
class OPENWORLDRPG_API UNewInventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:
	UNewInventoryGrid(const FObjectInitializer& ObjectInitializer);
private:
	UNewInventoryComponent* InventoryComp;
	AMainController* MainCon;
	AMainCharacter* Main;
	float TileSize;

	/* InventoryStruct */
	UPROPERTY(EditDefaultsOnly)
	TArray<FLine> Lines;

public:
	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UCanvasPanel* GridCanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* GridBorder;

	/*UPROPERTY(EditDefaultsOnly)
	UNewItemwidget* ItemWidget;*/
	
	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WNewItemWidget = nullptr;

	UNewItemwidget* newitemwidget;
	//UNewItemwidget

	

public:
	virtual void NativeConstruct() override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	void GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize);
	void CreateLineSegments();

	UFUNCTION(BlueprintCallable)
	void RefreshInventory();

	UFUNCTION()
	void OnItemRemove(UObject* T_ItemObj);


	/*template <typename WidgetT>
	FORCEINLINE_DEBUGGABLE WidgetT* ConstructWidget(TSubclassOf<UWidget>WidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None)
	{
		static_assert(TIsDerivedFrom<WidgetT, UWidget>::IsDerived, "WidgetTree::ConstructWidget can only create UWidget objects.");

		if (WidgetClass->IsChildOf<UUserWidget>())
		{
			return Cast<WidgetT>(CreateWidget(this, *WidgetClass, WidgetName));
		}

		return NewObject<WidgetT>(this, WidgetClass, WidgetName, RF_Transactional);
	}*/
};
