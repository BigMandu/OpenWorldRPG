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
class USlateBrushAsset;
class UNewInventoryComponent;
class UNewItemObject;
class UNewItemwidget;
class UDropWidget;
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

	FIntPoint DraggedTopLeftTile;
	bool bNeedDropLocation;
	
	USlateBrushAsset* Brush;


public:
	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UCanvasPanel* GridCanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* GridBorder;

	/*UPROPERTY(EditDefaultsOnly)
	UNewItemwidget* ItemWidget;*/
	
	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable")
	TSubclassOf<UUserWidget> WNewItemWidget = nullptr;

	//UNewItemwidget* newitemwidget;
	//UNewItemwidget
	UDropWidget* Dropwidget;
	
private:
	
	FVector2D GetMousePositionInEachTile(FVector2D MousePos);

public:
	virtual void NativeConstruct() override;
	
	/* Line Segments를 그려주는 함수 -> 자동 호출됨 */
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/* Item을 GridWidget에 드랍시 Item을 마우스 index에 추가시킨다. */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	/* Mouse index를 얻어오기 위해 필요한 함수 */
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;



	void GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize);
	void CreateLineSegments();

	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void OnItemRemove(UObject* T_ItemObj);

	void BindDropWidget(UDropWidget* T_Dropwidget);

	void DrawDropLocation(FPaintContext& Context) const;

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
