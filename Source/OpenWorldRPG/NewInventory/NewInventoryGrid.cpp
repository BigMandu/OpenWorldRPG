// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/DropWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "GameFramework/HUD.h"

//#include "UObject/ConstructorHelpers.h"
//#include "Input/DragAndDrop.h"
//#include "Layout/Geometry.h"
#include "Slate/SlateBrushAsset.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/LineBatchComponent.h"




UNewInventoryGrid::UNewInventoryGrid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//E: / Unreal Projects / OpenWorldRPG / Content / Inventory / SB_DropLocation.uasset
	static ConstructorHelpers::FObjectFinder<USlateBrushAsset> BrushAsset(TEXT("/Game/Inventory/SB_DropLocation.SB_DropLocation"));

	if (BrushAsset.Succeeded())
	{
		Brush = BrushAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::BrushAsset load success."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::BrushAsset load failed."));
	}

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
			InventoryComp->OnInventoryUpdated.AddUFunction(this, FName("RefreshInventory"));
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

	bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();

	if (bNeedDropLocation && bDragging)
	{
		DrawDropLocation(Context);
		//UNewItemObject* Obj = Cast< UNewItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
		//if (Obj)
		//{
		//	FTile DraggedTile;
		//	DraggedTile.X = DraggedTopLeftTile.X;
		//	DraggedTile.Y = DraggedTopLeftTile.Y;

		//	int32 index = InventoryComp->TileToIndex(DraggedTile);

		//	bool bCanDrop = InventoryComp->IsAvailableSpace(Obj, index);

		//	FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
		//	FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
		//	FVector2D DropLocationPos = FVector2D(DraggedTile.X, DraggedTile.Y);
		//	FVector2D DropLocationSize = FVector2D(Obj->GetItemSize().X, Obj->GetItemSize().Y);
		//	FSlateBrush brush = FSlateBrush();

		//	//UWidgetBlueprintLibrary::MakeBrushFromTexture(UTexture2D::make)
		//	if (bCanDrop)
		//	{
		//		UWidgetBlueprintLibrary::DrawBox(Context, DropLocationPos, DropLocationSize, brush, Green);
		//		//DrawBox
		//	}
		//}
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
		const auto ItemsMap = InventoryComp->GetAllItems();

		for (auto ele : ItemsMap)
		{
			UNewItemwidget* testwidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);
			//newitemwidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);

			if (testwidget)
			{
				testwidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));
				testwidget->Tilesize = TileSize;
				testwidget->ItemObj = ele.Key;
				//testwidget->Refresh();

				UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(testwidget);


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

			//if (newitemwidget)
			//{
			//	newitemwidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));
			//	newitemwidget->Tilesize = TileSize;
			//	newitemwidget->ItemObj = ele.Key;
			//	newitemwidget->Refresh();

			//	UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(newitemwidget);


			//	/*�߰���Ų ItemWidget�� size�� Position�� ���� �ֱ� ���� �۾��� �����Ѵ�.
			//	* Setsize�� ����ϱ� ���� CanvasPanelSlot���� Cast�� ���ش�.
			//	* SetPosition�� ����Ѵ�.
			//	*/
			//	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PanelSlot);
			//	if (CanvasSlot)
			//	{
			//		CanvasSlot->SetAutoSize(true);
			//		FVector2D ItemTopLeft = FVector2D(ele.Value.X * TileSize, ele.Value.Y * TileSize);
			//		CanvasSlot->SetPosition(ItemTopLeft);
			//	}
			//		

			//}
		}
	}
}

void UNewInventoryGrid::BindDropWidget(UDropWidget* T_Dropwidget)
{
	Dropwidget = T_Dropwidget;
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
		if (Dropwidget)
		{
			Dropwidget->ChangeState();
		}
	}
}

bool UNewInventoryGrid::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UNewItemObject* ItemObj = Cast<UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		FTile DraggedTile;
		DraggedTile.X = DraggedTopLeftTile.X;
		DraggedTile.Y = DraggedTopLeftTile.Y;

		int32 index = InventoryComp->TileToIndex(DraggedTile);

		/* ������ �ȴٸ� �ش� ������ �ִ´�.*/
		if (InventoryComp->IsAvailableSpace(ItemObj, index))
		{
			InventoryComp->AddItemAtIndex(ItemObj, index);
			return true;
		}
		else
		{
			/* ������ ������ ���� ����� Top-Left Index�� �ִ´�.*/
			if (InventoryComp->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex))
			{
				InventoryComp->AddItemAtIndex(ItemObj, ItemObj->TopLeftIndex);
			}
			else
			{
				/* ���� �����ٸ� �׳� Item�� �ִ´�.*/
				InventoryComp->TryAddItem(ItemObj);
			}
		}
	}
	return false;
}

bool UNewInventoryGrid::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = false;

	UNewItemObject* ItemObj = Cast<UNewItemObject>(InOperation->Payload);

	if (ItemObj)
	{
		int32 AddRight = 0;
		int32 AddDown = 0;
		/** Returns The position of the cursor in screen space */
		const FVector2D MousePosInWindow = InDragDropEvent.GetScreenSpacePosition();

		/** Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in. */
		FVector2D MousePosInWidget = InGeometry.AbsoluteToLocal(MousePosInWindow); 

		FVector2D MousePosInEachTile = GetMousePositionInEachTile(MousePosInWidget);


		UE_LOG(LogTemp, Warning, TEXT("========================================="));
		UE_LOG(LogTemp, Warning, TEXT("MousePOSinWINDOW = %s"), *MousePosInWindow.ToString());
		UE_LOG(LogTemp, Warning, TEXT("MousePOSinWIDGET = %s"), *MousePosInWidget.ToString());
		UE_LOG(LogTemp, Warning, TEXT("MousePOSinTILE = %s"), *MousePosInEachTile.ToString());

		FIntPoint Itemsize = ItemObj->GetItemSize();
		/* �ش� Ÿ���� ���������� ���̻�, �Ʒ��� ���̻� ���������� �Ǵ�.*/
		if (MousePosInEachTile.X > TileSize / 2.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Right"));
			AddRight = 1;
		}

		if (MousePosInEachTile.Y > TileSize / 2.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Down"));
			AddDown = 1;
		}

		/* �ּ�, �ִ밪 ���� */
		int32 Reindex_X = FMath::Clamp<int32>(Itemsize.X - AddRight, 0, Itemsize.X - AddRight);
		int32 Reindex_Y = FMath::Clamp<int32>(Itemsize.Y - AddDown, 0, Itemsize.Y - AddDown);

		FIntPoint CenterPosition = FIntPoint(Reindex_X, Reindex_Y) /2;
		FIntPoint GetPos = FIntPoint((MousePosInWidget / TileSize).X, (MousePosInWidget / TileSize).Y);

		DraggedTopLeftTile = GetPos - CenterPosition;	
		UE_LOG(LogTemp, Warning, TEXT("CenterPosition = %s"), *CenterPosition.ToString());
		UE_LOG(LogTemp, Warning, TEXT("GetPos = %s"), *GetPos.ToString());
		UE_LOG(LogTemp, Warning, TEXT("DraggedTopLeftTile = %s"), *DraggedTopLeftTile.ToString());

		bReturn = true;
	}

	return bReturn;
}

FVector2D UNewInventoryGrid::GetMousePositionInEachTile(FVector2D MousePos)
{
	FVector2D VectorToReturn;

	VectorToReturn = FVector2D(FGenericPlatformMath::Fmod(MousePos.X, TileSize), fmod(MousePos.Y, TileSize));

	return VectorToReturn;
}


void UNewInventoryGrid::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bNeedDropLocation = true;
}

void UNewInventoryGrid::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bNeedDropLocation = false;
}

void UNewInventoryGrid::DrawDropLocation(FPaintContext& Context) const
{
	UNewItemObject* Obj = Cast< UNewItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
	if (Obj)
	{
		FTile DraggedTile;
		DraggedTile.X = DraggedTopLeftTile.X;
		DraggedTile.Y = DraggedTopLeftTile.Y;

		int32 index = InventoryComp->TileToIndex(DraggedTile);

		bool bCanDrop = InventoryComp->IsAvailableSpace(Obj, index);

		

		FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
		FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
		FVector2D DropLocationPos = FVector2D(DraggedTile.X, DraggedTile.Y);
		FVector2D DropLocationSize = FVector2D(Obj->GetItemSize().X * TileSize, Obj->GetItemSize().Y * TileSize);
		
		if (Brush)
		{
			if (bCanDrop)
			{
				UWidgetBlueprintLibrary::DrawBox(Context, DropLocationPos * TileSize, DropLocationSize, Brush, Green);
				//DrawBox
			}
			else
			{
				UWidgetBlueprintLibrary::DrawBox(Context, DropLocationPos * TileSize, DropLocationSize, Brush, Red);
			}
		}
		
		
		//USlateBrushAsset BrushAsset = USlateBrushAsset();
		//USlateBrushAsset Brush = USlateBrushAsset::
		//FSlateBrush Brush = FSlateBrush();
		//brushAsset.Brush = Brush;
		//UWidgetBlueprintLibrary::MakeBrushFromTexture(UTexture2D::make)
		
	}
}

