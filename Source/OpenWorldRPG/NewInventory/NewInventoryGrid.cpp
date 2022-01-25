// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/LineBatchComponent.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"


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
	
		
		//InventoryComp�� AddItem�Լ��� �����Ҷ����� broadcast,�ش� �Լ��� �����Ű�� ���� bind
		if (InventoryComp)
		{
			FDelegateHandle DelHandle = InventoryComp->OnInventoryUpdated.AddUFunction(this, FName("RefreshInventory"));
		}

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
	* border�� Top-Left���� �׷�������.
	*/
	for (FLine line : Lines)
	{
		FVector2D posA = FVector2D(line.Start + BorderLo);
		FVector2D posB = FVector2D(line.End + BorderLo);


		//DrawLine�� Tint = RGB all 0.5f, Alpha is 0.5f;
		FLinearColor LineColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, posA, posB, LineColor);
	}
	return bReturn;
}

void UNewInventoryGrid::CreateLineSegments()
{
	/* create vertical line */
	/* 0������ �������� ���������� �׸��� ���� ������ ����� �־���*/
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

/* �� �Լ��� Item�� Inventory���� ������Ʈ �ɶ� (�߰��ɶ�, �����ɶ�) ȣ���������Ѵ�.*/
void UNewInventoryGrid::RefreshInventory()
{
	/* ���ʿ� �ִ� CanvasPanel�� ���� �����ش�.
	* Line�� Border�� ������ ���� �ٽ� �����ʿ�� ����.
	* �׸��� InventoryComponent�� �ִ� ��� item�� �ҷ��ͼ� �ش��ϴ� �����ܰ� �����ŭ 
	* ä�� �ִ´�.
	* InventoryComponent�� ��� Item�� ������ �Լ��� ���� �߰����ش�.
	*/
	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain != nullptr) ? TMain : nullptr;
	if (Main)
	{
		MainCon = Main->MainController;
	}
	
	if (GridCanvasPanel && MainCon != nullptr)
	{
		//TMap<UNewItemObject*, FTile> ItemsMap;
		GridCanvasPanel->ClearChildren();
		auto ItemsMap = InventoryComp->GetAllItems();

		for (auto ele : ItemsMap)
		{
			UNewItemwidget* ItemWidget = Cast<UNewItemwidget>(CreateWidget<UUserWidget>(this, MainCon->WItemwidget));
			if (ItemWidget)
			{
				ItemWidget->Tilesize = TileSize;
				ItemWidget->ItemObj = ele.Key;

				//Item�� ���������� ����� function�� �����ѵ�, NewItemWidget�� Delegate�� �����Ѵ�.
				// func�� ������ delegate�� �Ķ���Ϳ� ����.
				ItemWidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));// &UNewInventoryGrid::OnItemRemove);
				
				ItemWidget->Refresh();

				//canvas panel�� �ڽ����� ItemWidget�� �߰���Ų��.
				UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(ItemWidget);


				/*�߰���Ų ItemWidget�� size�� Position�� ���� �ֱ� ���� �۾��� �����Ѵ�.
				* Setsize�� ����ϱ� ���� CanvasPanelSlot���� Cast�� ���ش�.
				* SetPosition�� ����Ѵ�.
				*/
				UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PanelSlot);
				if (CanvasSlot)
				{
					CanvasSlot->SetAutoSize(true);
					FVector2D ItemTopLeft = FVector2D(ele.Value.X * TileSize, ele.Value.Y * TileSize);
					CanvasSlot->SetPosition(ItemTopLeft);
				}


				
			}
		}
	}
}

void UNewInventoryGrid::OnItemRemove(UObject* T_ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("Call OnRemoveFunc"));
	UNewItemObject* ItemObj = Cast<UNewItemObject>(T_ItemObj);
	if (ItemObj && InventoryComp)
	{
		/* 
		* NewItemWidget�� delegate�� bind�� �Լ�.
		* �� �Լ��� ȣ��Ǹ鼭,
		* �ش� InventoryGrid�� ����� InventoryComponent�� Ư�� �Լ��� ȣ���Ѵ�.
		*/
		bool bRemoveResult = InventoryComp->RemoveItem(ItemObj);
	}
}

