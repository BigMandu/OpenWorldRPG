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

			//Item�� ����� �����ͼ� TileSize��ŭ ���� Widget�� ����� �����Ѵ�.
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

			//�׸��ֱ�
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
		
		
		

		//Item�� �������� �ش� event�� bind�� �Լ��� ȣ���Ѵ�.6
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

		//this�� �ھƹ����� size�� ũ�� Grid�� ���������� ������ �߻���.
		DDOper->DefaultDragVisual = this;
		


		/*OutOperation->Payload = ItemObj;
		OutOperation->DefaultDragVisual = this;*/
		
		OutOperation = DDOper;
		

		ItemObj->bIsDragging = true;
		
		//EquipWidget || NewInventoryGrid���� Bind�� (Add�Ҷ�)
		OnRemoved.Broadcast(ItemObj);
		RemoveFromParent();


		/* Drop Widget�� �ִٸ�, Drop Widget�� ���¸� ��ȯ�ϱ� ���� BroadCast�� �Ѵ�.*/
		//OnDragDetect.Broadcast(ItemObj);

		/* ���߿� �����ؾ��Ұ�. 
		* Drag�� Detection�Ǹ� �ٷ� ����°� �ƴ϶� , 
		* �� Class�� Drop�̺�Ʈ���� 
		*	mother container�� ������, �ش� Container�� Drop()�� ȣ���ϴ� ������ ��������.
		*  �̷��� �Ǹ� Drop()���� �̺�Ʈ�� �߻���Ű��  DropWidget���� �����Ҽ� �ְ� �ȴ�. �غ��� 
		*/
		
		
	}
}

bool UNewItemwidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop func called"));

	/* ���� Item�̰� Stackable�� �����ϴٸ�, stack�Ѵ�.
	 *
	 */


	/*
		���⼭, Operation�� Payload���� �ִ� MotherContainer��  ���� �̳��� MotherContainer�� ��.

		������ , �����̳ʰ� �ٲ��� ����. index(��ġ)�� �����ϸ��.
		���� Top-Left index�� �̿�, �̳��� Mothercontainer���� �Լ��� ȣ��, index�� �����Ѵ�.


		�ٸ���, �����̳ʰ� �����.
		�̳��� MotherContainer��  payload���� MotherContainer�� �Ķ���ͷ� �޴� �Լ��� ȣ��,
		�̳��� MotherContainer���� item����, payload���� MotherContainer�� item add�Ѵ�.

	*/

	/*
	UNewItemObject* InOperItemObj = Cast<UNewItemObject>(InOperation->Payload);
	if (InOperItemObj && InOperItemObj == ItemObj)
	{
		InOperItemObj = nullptr;
		//ItemWidget�� ItemObj�� ����� MotherContainer�� ��ġ�Ѵٸ� ���� �����̳ʿ��� �̵�.
		if (MotherContainer == ItemObj->GetMotherContainer())
		{
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop(), Call MoveItemInSameContainer"));
			MotherContainer->MoveItemInSameContainer(ItemObj);
		}
		//��ġ ���� �ʴ´ٸ� �ٸ� �����̳ʷ� �̵���. (ItemWidget�� MotherContainer��)
		else if (MotherContainer != ItemObj->GetMotherContainer() && MotherContainer->bCanDrop)
		{
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget::OnDrop(), Move other container"));
			OnRemoved.Broadcast(ItemObj);
			//RemoveFromParent(); //���⼭ ���� ������
			return true;
		}
		//�ƹ��͵� �ƴϸ� ���з� Rollback�Ѵ�.
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

