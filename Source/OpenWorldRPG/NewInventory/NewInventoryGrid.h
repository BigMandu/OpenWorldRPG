// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
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

UENUM(BlueprintType)
enum class EGridType : uint8
{
	EGT_Inventory	UMETA(DisplayName = "Inventory"),
	EGT_Slot		UMETA(DisplayName = "Slot"),

	EGT_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UCLASS()
class OPENWORLDRPG_API UNewInventoryGrid : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()
public:
	UNewInventoryGrid(const FObjectInitializer& ObjectInitializer);
private:
	UNewInventoryComponent* InventoryComp;
	AMainController* MainCon;
	AMainCharacter* Main;


	/* InventoryStruct */
	UPROPERTY(EditDefaultsOnly)
	TArray<FLine> Lines;

	FIntPoint DraggedTopLeftTile;
	bool bNeedDropLocation;
	
	USlateBrushAsset* Brush;

	UNewItemObject* PendingObj;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Settings)
	EGridType GridType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Settings)
	float Rows;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Settings)
	float Columns;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Settings)
	float TileSize;

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
	
	//bool bCanDrop;
	
private:
	
	FVector2D GetMousePositionInEachTile(FVector2D MousePos);

public:
	virtual void NativeConstruct() override;
	
	/* Line Segments?? ???????? ???? -> ???? ?????? */
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/* Item?? GridWidget?? ?????? Item?? ?????? index?? ??????????. */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	/* Mouse index?? ???????? ???? ?????? ???? */
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;



	void GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize);
	void CreateLineSegments();

	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void OnItemRemove(UObject* T_ItemObj);

	void BindDropWidget(UDropWidget* T_Dropwidget);

	void DrawDropLocation(FPaintContext& Context) const;

	bool MoveItemInSameContainer(UNewItemObject* Item);

	UNewInventoryComponent* GetInventoryComp();
	
	//bool RemoveItemAtBeforeIndex(UNewItemObject* Item);

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
