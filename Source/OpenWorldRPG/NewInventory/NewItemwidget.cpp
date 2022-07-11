// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/TooltipWidget.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
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

	//Refresh();
	CreateTooltip();
}

void UNewItemwidget::CreateTooltip()
{
	Tooltip = CreateWidget<UTooltipWidget>(this, WTooltipWidget);
	if (Tooltip && ItemObj)
	{
		Tooltip->SetData(ItemObj, Tilesize);
		SetToolTip(Tooltip);
	}
}

void UNewItemwidget::Refresh()// float var_tilesize)
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

			//BGSizeBox->SetWidthOverride(200.f);
			//BGSizeBox->SetHeightOverride(200.f);
			BGSizeBox->SetWidthOverride(widgetsize.X);
			BGSizeBox->SetHeightOverride(widgetsize.Y);

			BGBorder->SetBrushColor(NormalColor);
			//ItemIcon->SetBrush(GetIconImage());

			UE_LOG(LogTemp, Warning, TEXT("Widget size : %s"), *widgetsize.ToString());
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

void UNewItemwidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	//UDragDropOperation
	DDOper = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
	
	if (ItemObj)
	{
		
		DDOper->Payload = ItemObj;

		//this로 박아버리니 size가 크면 Grid를 가려버리는 문제가 발생함.
		DDOper->DefaultDragVisual = this;
		


		/*OutOperation->Payload = ItemObj;
		OutOperation->DefaultDragVisual = this;*/
		
		OutOperation = DDOper;
		

		ItemObj->bIsDragging = true;
		
		//EquipWidget || NewInventoryGrid에서 Bind함 (Add할때)
		OnRemoved.Broadcast(ItemObj);
		RemoveFromParent();


		/* Drop Widget이 있다면, Drop Widget의 상태를 변환하기 위해 BroadCast를 한다.*/
		//OnDragDetect.Broadcast(ItemObj);

		/* 나중에 수정해야할것. 
		* Drag가 Detection되면 바로 지우는게 아니라 , 
		* 이 Class의 Drop이벤트에서 
		*	mother container가 있으면, 해당 Container의 Drop()을 호출하는 식으로 변경하자.
		*  이렇게 되면 Drop()에서 이벤트를 발생시키고  DropWidget에서 변경할수 있게 된다. 해보자 
		*/
		
		
	}
}

bool UNewItemwidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop func called"));

	/* 같은 Item이고 Stackable이 가능하다면, stack한다.
	 *
	 */


	/*
		여기서, Operation의 Payload내에 있는 MotherContainer와  지금 이놈의 MotherContainer를 비교.

		같으면 , 컨테이너가 바뀌지 않음. index(위치)만 변경하면됨.
		얘의 Top-Left index를 이용, 이놈의 Mothercontainer내에 함수를 호출, index를 변경한다.


		다르면, 컨테이너가 변경됨.
		이놈의 MotherContainer와  payload내의 MotherContainer를 파라미터로 받는 함수를 호출,
		이놈의 MotherContainer에서 item삭제, payload내의 MotherContainer에 item add한다.

	*/

	/*
	UNewItemObject* InOperItemObj = Cast<UNewItemObject>(InOperation->Payload);
	if (InOperItemObj && InOperItemObj == ItemObj)
	{
		InOperItemObj = nullptr;
		//ItemWidget과 ItemObj에 저장된 MotherContainer가 일치한다면 같은 컨테이너에서 이동.
		if (MotherContainer == ItemObj->GetMotherContainer())
		{
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop(), Call MoveItemInSameContainer"));
			MotherContainer->MoveItemInSameContainer(ItemObj);
		}
		//일치 하지 않는다면 다른 컨테이너로 이동함. (ItemWidget의 MotherContainer로)
		else if (MotherContainer != ItemObj->GetMotherContainer() && MotherContainer->bCanDrop)
		{
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop(), Move other container"));
			OnRemoved.Broadcast(ItemObj);
			//RemoveFromParent(); //여기서 가끔 에러뜸
			return true;
		}
		//아무것도 아니면 실패로 Rollback한다.
		else if(MotherContainer->bCanDrop == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop(), Wrong area"));
			//MotherContainer
		}


		//
		//
		//}
	}
	*/
	return bReturn;
}

