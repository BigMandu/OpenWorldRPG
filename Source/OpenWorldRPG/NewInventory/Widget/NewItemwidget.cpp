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
	bIsFocusable = true;
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

			//Item�� ����� �����ͼ� TileSize��ŭ ���� Widget�� ����� �����Ѵ�.
			widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);
			//UE_LOG(LogTemp, Warning, TEXT("UNewItemwidget::Refresh / TileSize : %f"), Tilesize);
			//UE_LOG(LogTemp, Warning, TEXT("UNewItemwidget::Refresh / itemsize.x : %f, itemsize.y :f"), Itemsize.X, Itemsize.Y);

			BGSizeBox->SetWidthOverride(widgetsize.X);
			BGSizeBox->SetHeightOverride(widgetsize.Y);

			BGBorder->SetBrushColor(NormalColor);
			//ItemIcon->SetBrush(GetIconImage());


			//�׸��ֱ�
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
			if (CanvasSlot)
			{
				//CanvasSlot->SetSize(FVector2D(200.f));
				CanvasSlot->SetSize(widgetsize);
				ItemIcon->SetBrush(GetIconImage());
			}
			//Item ���� �ֱ�
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

FSlateBrush UNewItemwidget::GetIconImage()
{
	FSlateBrush icon;
	if (ItemObj)
	{
		if (ItemObj->GetItemIcon() != nullptr)
		{
			icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->GetItemIcon(), widgetsize.X, widgetsize.Y);
		}
	
		//Item�� �������� �ش� event�� bind�� �Լ��� ȣ���Ѵ�.
		//OnRemoved.Broadcast(ItemObj);
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
		//Remove Focus
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		if (LocalPlayer)
		{
			FReply& DelayedSlateOperations = LocalPlayer->GetSlateOperations();
			DelayedSlateOperations.ClearUserFocus();
		}

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

	//DDOper = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UCustomDDOperation::StaticClass()));
	
	//Dragwidget�� �����ؼ� ���� �������ش�.
	UNewItemwidget* DragWidget = Cast<UNewItemwidget>(CreateWidget<UUserWidget>(GetWorld(), this->GetClass()));
	check(DragWidget)

	DragWidget->ItemObj = ItemObj;
	DragWidget->Tilesize = Tilesize;

	//New Version
	/* DragWidget�� �� ������ �����Ѱ����� ��ü�Ѵ�. 
	*  ��, OnDrop�� ��Ȯ�� ����Ǿ�� �ش� ������ �����ȴ�.
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
		//EquipWidget || NewInventoryGrid���� Bind�� (Add�Ҷ�)
		OnRemoved.Broadcast(ItemObj);
		
		
		DDOper->Payload = ItemObj;
		DDOper->DefaultDragVisual = this;
		
		OutOperation = DDOper;
		RemoveFromParent();


		// Drop Widget�� �ִٸ�, Drop Widget�� ���¸� ��ȯ�ϱ� ���� BroadCast�� �Ѵ�.
		//OnDragDetect.Broadcast(ItemObj);
	}
	*/
	
}

bool UNewItemwidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop func called"));

	/* ���� Item�̰� Stackable�� �����ϴٸ�, stack�Ѵ�.
	 *
	 */
	 //auto DroppedItemObj = Cast< UCustomDDOperation
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	//DDOper�� ���ų�, ���� Widget�̸� return�Ѵ�.
	 if(DDOper == nullptr) return false;
	 if (ItemObj == DDOper->ItemObj) return false;

	UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
	if(CPDA && CPDA->bHasStorage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemWidget::OnDrop / Has Storage, Try add item"));
		Cast<UItemStorageObject>(ItemObj)->TryAddItem(DDOper->ItemObj);
		
	}
	else if(ItemObj->ItemInfo.DataAsset->bCanStack && ItemObj->ItemInfo.DataAsset == DDOper->ItemObj->ItemInfo.DataAsset)
	{
		MotherContainer->MoveItemToItem(DDOper->ItemObj,ItemObj);		
	}

	return bReturn;
}

FReply UNewItemwidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//FReply ReturnReply;// = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	//FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//InventoryGrid�� Bind�� event broadcast�ϱ�.
		if (MotherContainer)
		{
			MotherContainer->OpenAdditionalWidget.Broadcast(ItemObj);
		}


	}

	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

/* If player input 4,5,6,...
*  Register this ItemObj in QuickSlot.
*/
FReply UNewItemwidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//FReply Rep = Super::NativeOnKeyDown(InGeometry,InKeyEvent);
	//Super::NativeOnPreviewKeyDown
	FKey key = InKeyEvent.GetKey();

	AMainController* PlayerCon = Cast<AMainController>(GetOwningPlayer());
	check(PlayerCon);
	UQuickSlotWidget* QuickSlot = PlayerCon->MainHud->QuickSlot;
	check(QuickSlot);
	
	if (key == EKeys::Four || key == EKeys::NumPadFour)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N4,ItemObj);
		return FReply::Handled();
	}
	else if (key == EKeys::Five || key == EKeys::NumPadFive)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N5, ItemObj);
		return FReply::Handled();
	}
	else if (key == EKeys::Six || key == EKeys::NumPadSix)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N6, ItemObj);
		return FReply::Handled();
	}
	else if (key == EKeys::Seven || key == EKeys::NumPadSeven)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N7, ItemObj);
		return FReply::Handled();
	}
	else if (key == EKeys::Eight || key == EKeys::NumPadEight)
	{
		QuickSlot->SetItemInQuickSlot(EQuickSlotNumber::EQSN_N8, ItemObj);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}
