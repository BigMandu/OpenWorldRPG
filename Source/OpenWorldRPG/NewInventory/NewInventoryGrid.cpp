// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/LineBatchComponent.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


UNewInventoryGrid::UNewInventoryGrid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNewInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNewInventoryGrid::GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize)
{
	InventoryComp = p_InvComp;
	TileSize = p_TileSize;
	
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
	if (CanvasSlot)
	{
		FVector2D Size = FVector2D(InventoryComp->Columns * TileSize, InventoryComp->Rows * TileSize);
		CanvasSlot->SetSize(Size);
	
		CreateLineSegments();
		RefreshInventory();
	}

}


int32 UNewInventoryGrid::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 bReturn = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FGeometry BorderGeo = GridBorder->GetCachedGeometry();
	FVector2D BorderLo = USlateBlueprintLibrary::GetLocalTopLeft(BorderGeo);



	/*
	* border의 Top-Left부터 그려나간다.
	*/
	for (FLine line : Lines)
	{
		FVector2D posA = FVector2D(line.Start + BorderLo);
		FVector2D posB = FVector2D(line.End + BorderLo);


		//DrawLine의 Tint = RGB all 0.5f, Alpha is 0.5f;
		FLinearColor LineColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, posA, posB, LineColor);
	}
	return bReturn;
}

void UNewInventoryGrid::CreateLineSegments()
{
	/* create vertical line */
	/* 0열부터 끝열까지 순차적으로 그리기 위해 라인을 만들어 넣어줌*/
	for (int32 iter = 0; iter <= InventoryComp->Columns; iter++)
	{
		float LocalX = iter * TileSize;
		FLine Line;
		Line.Start = FVector2D(LocalX, 0.f);
		Line.End = FVector2D(LocalX, InventoryComp->Rows * TileSize);
		Lines.Add(Line); 
		
	}
	
	/* create horizontal line */
	for (int32 iter = 0; iter <= InventoryComp->Rows; iter++)
	{
		float LocalY = iter * TileSize;
		FLine Line;
		Line.Start = FVector2D(0.f, LocalY);
		Line.End = FVector2D(InventoryComp->Columns * TileSize, LocalY);
		Lines.Add(Line);
	}
}

void UNewInventoryGrid::RefreshInventory()
{
	/* 안쪽에 있는 CanvasPanel을 전부 지워준다.
	* Line은 Border에 있으니 선을 다시 그을필요는 없음.
	* 그리고 InventoryComponent에 있는 모든 item을 불러와서 해당하는 아이콘과 사이즈만큼 
	* 채워 넣는다.
	* InventoryComponent에 모든 Item을 얻어오는 함수를 새로 추가해준다.
	*/

	if (GridCanvasPanel)
	{
		//TMap<UNewItemObject*, FTile> ItemsMap;
		GridCanvasPanel->ClearChildren();
		auto ItemsMap = InventoryComp->GetAllItems();

		for (auto ele : ItemsMap)
		{
			//CreateWidget>
			//ItemsMap.Find(mapkey.Key);
			
		}
		

	}
}


