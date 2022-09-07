// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/Widget/TooltipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Styling/SlateBrush.h"


bool UNewItemwidget::Initialize() 
{
	bool bReturn = Super::Initialize();

	return bReturn;
}

void UNewItemwidget::NativeConstruct()
{
	Super::NativeConstruct();

	Refresh();
	CreateTooltip();
}

void UNewItemwidget::CreateTooltip()
{
	Tooltip = CreateWidget<UTooltipWidget>(this, WTooltipWidget);
	if (Tooltip && ItemObj && ItemObj->ItemInfo.DataAsset)
	{
		Tooltip->SetData(ItemObj, Tilesize);
		SetToolTip(Tooltip);
	}
}

void UNewItemwidget::Refresh() //UNewItemObject* V_Obj, float V_Tilesize)
{
	
	if (ItemObj && BGSizeBox && ItemIcon && BGBorder)
	{
		//Tilesize = var_tilesize;
		if (ItemObj->GetItemSize() != FIntPoint(0))
		{
			FIntPoint Itemsize = ItemObj->GetItemSize();

			//Item의 사이즈를 가져와서 TileSize만큼 곱해 Widget의 사이즈를 결정한다.
			widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);
			UE_LOG(LogTemp, Warning, TEXT("TileSize : %f"), Tilesize);
			UE_LOG(LogTemp, Warning, TEXT("itemsize.x : %f, itemsize.y :f"), Itemsize.X, Itemsize.Y);

			BGSizeBox->SetWidthOverride(widgetsize.X);
			BGSizeBox->SetHeightOverride(widgetsize.Y);

			BGBorder->SetBrushColor(NormalColor);
			//ItemIcon->SetBrush(GetIconImage());

			//Debug
			/*UE_LOG(LogTemp, Warning, TEXT("Widget size : %s"), *widgetsize.ToString());
			UE_LOG(LogTemp, Warning, TEXT("sizebox size : %s"), *BGSizeBox->GetDesiredSize().ToString());
			UE_LOG(LogTemp, Warning, TEXT("Border size : %s"), *BGBorder->GetDesiredSize().ToString());
			UE_LOG(LogTemp, Warning, TEXT("Item icon size : %s"), *ItemIcon->Brush.GetImageSize().ToString());*/

			//그림넣기
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
			if (CanvasSlot)
			{
				//CanvasSlot->SetSize(FVector2D(200.f));
				CanvasSlot->SetSize(widgetsize);
				ItemIcon->SetBrush(GetIconImage());
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
	}
	
}

FSlateBrush UNewItemwidget::GetIconImage()
{
	FSlateBrush icon;
	if (ItemObj)
	{
		if (ItemObj->GetItemIcon() != nullptr)
		{
			icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->GetItemIcon(), widgetsize.X, widgetsize.Y);
		}
	
		//Item이 지워질때 해당 event에 bind된 함수를 호출한다.6
		//OnRemoved.Broadcast(ItemObj);
	}
	return icon;
}

void UNewItemwidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (BGBorder)
	{
		BGBorder->SetBrushColor(HoverColor);
		
		if (ToolTipWidget)
		{
			ToolTipWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UNewItemwidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (BGBorder)
	{
		BGBorder->SetBrushColor(NormalColor);
		if (ToolTipWidget)
		{
			ToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

FReply UNewItemwidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	//FPointerEvent point;
	//point.
	
	return Reply.NativeReply;
}

FReply UNewItemwidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//FReply ReturnReply;// = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	//FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//InventoryGrid에 Bind된 event broadcast하기.
		if (MotherContainer)
		{
			MotherContainer->OpenAdditionalWidget.Broadcast(ItemObj);
		}


	}

	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UNewItemwidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	//UDragDropOperation

	//DDOper = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
	DDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UCustomDDOperation::StaticClass()));
	
	//Dragwidget을 복제해서 새로 생성해준다.
	UNewItemwidget* DragWidget = Cast<UNewItemwidget>(CreateWidget<UUserWidget>(GetWorld(), this->GetClass()));
	check(DragWidget)

	DragWidget->ItemObj = ItemObj;
	DragWidget->Tilesize = Tilesize;

	//New Version
	/* DragWidget은 현 위젯을 복제한것으로 대체한다. 
	*  즉, OnDrop이 정확히 수행되어야 해당 위젯이 삭제된다.
	*/
	if (ItemObj && DDOper && DragWidget)
	{
		ItemObj->bIsDragging = true;
		
		DDOper->DefaultDragVisual = DragWidget;
		//DDOper->Payload = this;
		DDOper->ItemObj = ItemObj;
		//DDOper->ParentGridWidget = MotherContainer;
		DDOper->DragWidget = DragWidget;

		OutOperation = DDOper;
		//OnDragDetect.Broadcast(ItemObj);
	}

	//Old Version
	/*
	if (ItemObj)
	{
		ItemObj->bIsDragging = true;
		//EquipWidget || NewInventoryGrid에서 Bind함 (Add할때)
		OnRemoved.Broadcast(ItemObj);
		
		
		DDOper->Payload = ItemObj;
		DDOper->DefaultDragVisual = this;
		
		OutOperation = DDOper;
		RemoveFromParent();


		// Drop Widget이 있다면, Drop Widget의 상태를 변환하기 위해 BroadCast를 한다.
		//OnDragDetect.Broadcast(ItemObj);
	}
	*/
	
}

bool UNewItemwidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop func called"));

	/* 같은 Item이고 Stackable이 가능하다면, stack한다.
	 *
	 */
	return bReturn;
}

