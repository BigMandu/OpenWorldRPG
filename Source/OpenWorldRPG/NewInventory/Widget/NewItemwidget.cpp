// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/Widget/TooltipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"

#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"


#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

	bIsFocusable = true;
	
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

void UNewItemwidget::Refresh(bool bIsTempRotate)
{	
	if (ItemObj && BGSizeBox && ItemIcon && BGBorder)
	{
		//Tilesize = var_tilesize;
		if (ItemObj->GetItemSize() != FIntPoint(0))
		{
			FIntPoint Itemsize;
			if (bIsTempRotate)
			{
				Itemsize = ItemObj->GetTempItemSize();
			}
			else
			{
				Itemsize = ItemObj->GetItemSize();
			}

			//Item의 사이즈를 가져와서 TileSize만큼 곱해 Widget의 사이즈를 결정한다.
			widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);
			//UE_LOG(LogTemp, Warning, TEXT("UNewItemwidget::Refresh / TileSize : %f"), Tilesize);
			//UE_LOG(LogTemp, Warning, TEXT("UNewItemwidget::Refresh / itemsize.x : %f, itemsize.y :f"), Itemsize.X, Itemsize.Y);

			BGSizeBox->SetWidthOverride(widgetsize.X);
			BGSizeBox->SetHeightOverride(widgetsize.Y);

			BGBorder->SetBrushColor(NormalColor);


			//그림넣기
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
			if (CanvasSlot)
			{
				//CanvasSlot->SetSize(FVector2D(200.f));
				CanvasSlot->SetSize(widgetsize);
				ItemIcon->SetBrush(GetIconImage(bIsTempRotate));
			}
			//Item 개수 넣기
			if(ItemObj->ItemInfo.DataAsset->bCanStack)
			{
				const FString CountStr = FString::FromInt(ItemObj->ItemInfo.Count);
				ItemCount->SetText(FText::FromString(CountStr));
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
	}
	
}

FSlateBrush UNewItemwidget::GetIconImage(bool bIsTempRotate)
{
	FSlateBrush icon;
	if (ItemObj)
	{
		if (bIsTempRotate)
		{
			icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->GetTempItemIcon(), widgetsize.X, widgetsize.Y);
		}
		else
		{
			icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->GetItemIcon(), widgetsize.X, widgetsize.Y);
		}
	
	}
	return icon;
}

void UNewItemwidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (BGBorder)
	{
		// Set Focus
		SetFocus();
		UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnMouseEnter / SetFocus Name : %s"), *GetFName().ToString());

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

	//Remove Focus
	if (!bIsDragWidget && HasAnyUserFocus())
	{
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		if (LocalPlayer)
		{
			FReply& DelayedSlateOperations = LocalPlayer->GetSlateOperations();
			DelayedSlateOperations.ClearUserFocus();
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnMouse Leave / Remove Focus Name : %s"), *GetFName().ToString());
		}
	}

	//Set Focus
	/*TSharedPtr<SWidget> SafeWidget = GetCachedWidget();
	if (SafeWidget.IsValid())
	{
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		if(LocalPlayer)
		{
			FReply& DelayedSlateOperations = LocalPlayer->GetSlateOperations();
			DelayedSlateOperations.ClearUserFocus(SafeWidget.ToSharedRef());
		}

	}*/
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
	FEventReply Reply;

	//기존
	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	//개선, LeftButton, Right Button을 구분하기 위해 if문으로 분기한다.
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this, EKeys::LeftMouseButton);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// open menu widget
	}
	
	
	return Reply.NativeReply;
}

void UNewItemwidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	DDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UCustomDDOperation::StaticClass()));
	
	//Dragwidget을 복제해서 새로 생성해준다.
	UNewItemwidget* DragWidget = CreateWidget<UNewItemwidget>(GetWorld(), this->GetClass());//, FName(TEXT("DragWidget")));//Cast<UNewItemwidget>(CreateWidget<UUserWidget>(GetWorld(), this->GetClass()));
	check(DragWidget)

	DragWidget->bIsDragWidget = true;
	DragWidget->ItemObj = ItemObj;
	DragWidget->Tilesize = Tilesize;
	DragWidget->SetFocus();

	//New Version
	/* DragWidget은 현 위젯을 복제한것으로 대체한다. 
	*  즉, OnDrop이 정확히 수행되어야 해당 위젯이 삭제된다.
	*/
	if (ItemObj && DDOper && DragWidget)
	{
		ItemObj->bIsDragging = true;
		

		DDOper->DefaultDragVisual = DragWidget;
		DDOper->DragWidget = DragWidget;
		DDOper->ItemObj = ItemObj;
		DDOper->Pivot = EDragPivot::CenterCenter;
		
		

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

	/* 같은 Item이고 Stackable이 가능하다면, stack한다.  */
	 
	
	UCustomDDOperation* CusDDOper = Cast<UCustomDDOperation>(InOperation);
	//DDOper가 없거나, 같은 Widget이면 return한다.
	 if(CusDDOper == nullptr) return false;
	 if (ItemObj == CusDDOper->ItemObj) return false;

	UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
	if(CPDA && CPDA->bHasStorage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemWidget::OnDrop / Has Storage, Try add item"));
		Cast<UItemStorageObject>(ItemObj)->TryAddItem(CusDDOper->ItemObj);
		
	}
	else if(ItemObj->ItemInfo.DataAsset->bCanStack && ItemObj->ItemInfo.DataAsset == CusDDOper->ItemObj->ItemInfo.DataAsset)
	{
		MotherContainer->MoveItemToItem(CusDDOper->ItemObj,ItemObj);
	}

	return bReturn;
}

FReply UNewItemwidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//FReply ReturnReply;// = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	//FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		/* EquipWidget, InvWidget에서 doubleclick event bind, (실제 bind는 NewInventory class에서 진행한다.) */
		//InventoryGrid에 Bind된 event broadcast하기.
		if (MotherContainer)
		{
			MotherContainer->OpenAdditionalWidget.Broadcast(ItemObj);
		}
		else if (MotherEquipWidget)
		{
			MotherEquipWidget->OpenAdditionalWidget_Equip.Broadcast(ItemObj);
		}
	}

	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

/* If player input 4,5,6,...
*  Register this ItemObj in QuickSlot.
*/
FReply UNewItemwidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("UNewItemwidget::NativeOnKeyDown"));

	FKey key = InKeyEvent.GetKey();
	
	/* Rotate Item */
	//if (DDOper)
	//{
	//	bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();
	//	if (key == EKeys::R) //&& bDragging)
	//	{
	//		if (ItemObj && ItemObj->ItemInfo.DataAsset->bCanRotate)
	//		{
	//			DDOper->ItemObj->ItemRotate();

	//			DDOper->DragWidget->ItemIcon->SetBrush(GetIconImage());
	//			return FReply::Handled();
	//		}
	//	}
	//}


	AMainController* PlayerCon = Cast<AMainController>(GetOwningPlayer());
	check(PlayerCon);

	UQuickSlotWidget* QuickSlot = PlayerCon->MainHud->QuickSlot;
	check(QuickSlot);
	
	if (key == EKeys::Four || key == EKeys::NumPadFour)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N4,ItemObj,true);
		return FReply::Handled();
	}
	else if (key == EKeys::Five || key == EKeys::NumPadFive)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N5, ItemObj, true);
		return FReply::Handled();
	}
	else if (key == EKeys::Six || key == EKeys::NumPadSix)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N6, ItemObj, true);
		return FReply::Handled();
	}
	else if (key == EKeys::Seven || key == EKeys::NumPadSeven)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N7, ItemObj, true);
		return FReply::Handled();
	}
	else if (key == EKeys::Eight || key == EKeys::NumPadEight)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N8, ItemObj, true);
		return FReply::Handled();
	}
	else if (key == EKeys::Nine || key == EKeys::NumPadNine)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N9, ItemObj, true);
		return FReply::Handled();
	}
	


	return FReply::Unhandled();
}


