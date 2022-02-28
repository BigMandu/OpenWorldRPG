// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/TooltipWidget.h"
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
	//SynchronizeProperties();
	Refresh();
	CreateTooltip();

	/*BGSizeBox = NewObject<USizeBox>(this, USizeBox::StaticClass());
	ItemIcon = NewObject<UImage>(this, UImage::StaticClass());*/
	/*if (testsizebox)
	{
		BGSizeBox = testsizebox;
	}*/
	//FObjectProperty* Obj = FindFProperty<FObjectProperty>(this->GetClass(), FName("BGSizeBox"));
	//if(Obj)
	//{
	//	//testsizebox = Cast<USizeBox>(Obj);
	//	
	//}
	//Refresh(); //error
}


//void UNewItemwidget::SynchronizeProperties()
//{
//	Super::SynchronizeProperties();
//	if (ItemObj && BGSizeBox)
//	{
//		FIntPoint Itemsize = ItemObj->GetItemSize();
//
//		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);
//
//		BGSizeBox->SetWidthOverride(widgetsize.X);
//		BGSizeBox->SetHeightOverride(widgetsize.Y); //error
//
//		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
//		if (CanvasSlot)
//		{
//			CanvasSlot->SetSize(widgetsize);
//		}
//
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
//	}
//}
void UNewItemwidget::CreateTooltip()
{
	Tooltip = CreateWidget<UTooltipWidget>(this, WTooltipWidget);
	if (Tooltip && ItemObj)
	{
		Tooltip->SetData(ItemObj, Tilesize);
		SetToolTip(Tooltip);
	}
}

void UNewItemwidget::Refresh()
{
	
	if (ItemObj && BGSizeBox && ItemIcon && BGBorder)
	{
		FIntPoint Itemsize = ItemObj->GetItemSize();

		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);

		BGSizeBox->SetWidthOverride(widgetsize.X);
		BGSizeBox->SetHeightOverride(widgetsize.Y); //error
		
		BGBorder->SetBrushColor(NormalColor);
		
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetSize(widgetsize);
			ItemIcon->SetBrush(GetIconImage());
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
	}
	
}

FSlateBrush UNewItemwidget::GetIconImage()
{
	FSlateBrush icon;
	if (ItemObj)
	{

		/* Item obj에서 함수를 하나 만든뒤 rotate bool에 따른 icon리턴하기 (icon, iconroated 중에서 하나)
		* 아래는 임시 코드임.
		*/
		icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->icon, widgetsize.X, widgetsize.Y);

		//Item이 지워질때 해당 event에 bind된 함수를 호출한다.
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

void UNewItemwidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	//UDragDropOperation
	UDragDropOperation* DDOper = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
	
	DDOper->Payload = ItemObj;
	DDOper->DefaultDragVisual = this;

	/*OutOperation->Payload = ItemObj;
	OutOperation->DefaultDragVisual = this;*/
	
	OutOperation = DDOper;
	/* Drag가 감지되면 해당 item을 Inventory에서 삭제한다.*/
	OnRemoved.Broadcast(ItemObj);	
	RemoveFromParent();
	
}