// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/NewInventory/InventoryStruct.h"
#include "Blueprint/UserWidget.h"
#include "NewInventoryGrid.generated.h"


/**
 * 
 */
class UBorder;
class UCanvasPanel;
class UNewInventoryComponent;


UCLASS()
class OPENWORLDRPG_API UNewInventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:
	UNewInventoryGrid(const FObjectInitializer& ObjectInitializer);
private:
	UNewInventoryComponent* InventoryComp;
	float TileSize;

	/* InventoryStruct */
	UPROPERTY(EditDefaultsOnly)
	TArray<FLine> Lines;
	UPROPERTY(EditDefaultsOnly)
	FTile Tile;

public:
	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UCanvasPanel* GridCanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* GridBorder;

	

	

public:
	virtual void NativeConstruct() override;

	void GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize);
	void CreateLineSegments();
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//void CustomPaint();
};
