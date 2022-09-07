// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/AdditionalWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/DraggInventoryWindow.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


void UAdditionalWidget::BindingOpenWidgetFunction(UNewItemObject* ItemObj)
{
	if (ItemObj == nullptr || ItemObj->ItemInfo.DataAsset->bHasStorage == false)
	{
		return;
	}
	UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ItemObj);
	if(StorageObj)
	{
		OpenAdditionalWidget(StorageObj);
	}
}

void UAdditionalWidget::OpenAdditionalWidget(UItemStorageObject* StorageObj)
{
	if (StorageObj && WStorageWindow)
	{
		StorageWindow = CreateWidget<UDraggInventoryWindow>(this, WStorageWindow);
		if(StorageWindow)
		{
			StorageWindow->GridInventory->StorageObj = StorageObj;
			
			UCanvasPanelSlot* CanvasSlot = ContentCanvas->AddChildToCanvas(StorageWindow);// ->AddChildToOverlay(StorageWindow);
			/*OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);*/

			float MouseX;
			float MouseY;
			if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
			{
				FVector2D CurrentMousePos = FVector2D(MouseX,MouseY);
				CanvasSlot->SetPosition(CurrentMousePos);
			}

			
			StorageWindow->GridInventory->OpenAdditionalWidget.AddDynamic(this, &UAdditionalWidget::BindingOpenWidgetFunction);
		}
	}

}


bool UAdditionalWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp,Warning,TEXT("Additional Widget :: OnDrop"));
	return false;
}
