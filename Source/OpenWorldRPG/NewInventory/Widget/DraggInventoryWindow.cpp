// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/DraggInventoryWindow.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalDaDWidget.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"




void UDraggInventoryWindow::NativeConstruct()
{
	Super::NativeConstruct();
	if(GridInventory && GridInventory->StorageObj)
	{
		StorageWidgetName->SetText(GridInventory->StorageObj->ItemInfo.DataAsset->ItemName);

		UE_LOG(LogTemp, Warning, TEXT("DraggingInventoryWindow::Binding ClsoeWindow func"));
		CloseButton->OnClicked.AddDynamic(this,&UDraggInventoryWindow::CloseWindow);
	}
}

FReply UDraggInventoryWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry,InMouseEvent);

	bMouseDown = true;
	/*MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FEventReply DragPressEve = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	FEventReply CaptureMouseEve = UWidgetBlueprintLibrary::CaptureMouse(DragPressEve,this);

	FirstMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());*/
	
	return DetectDragWidget(InMouseEvent,this); //CaptureMouseEve.NativeReply;
}

FReply UDraggInventoryWindow::DetectDragWidget(const FPointerEvent& InMouseEvent, UWidget* DragWidget)
{
	//나중에 TopBorder 의 IsHovered 조건 추가하기.
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) //&& topBorder->IsHoverd())
	{
		FEventReply Reply;
		Reply.NativeReply = FReply::Handled();

		if (DragWidget)
		{
			TSharedPtr<SWidget> SlateDetectedWidget = DragWidget->GetCachedWidget();
			
			if (SlateDetectedWidget.IsValid())
			{
				//DetectDrag가 정상 호출된다면, NativeOnDragDetected가 호출된다.
				Reply.NativeReply = Reply.NativeReply.DetectDrag(SlateDetectedWidget.ToSharedRef(),EKeys::LeftMouseButton);
				
				return Reply.NativeReply;
			}
		}
	}
	return FReply::Unhandled();
}

//이 이후의 루틴은 NewInventory의 NativeOnDrop이 호출된다.
void UDraggInventoryWindow::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry,InMouseEvent,OutOperation);
	
	UAdditionalDaDWidget* DaDOper = NewObject<UAdditionalDaDWidget>();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	//UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::DragDetected, HitTestInvisible"));
	DaDOper->AdditionalWidget = this;
	DaDOper->WidgetDragLocation = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	DaDOper->DefaultDragVisual = this;
	DaDOper->Pivot = EDragPivot::MouseDown;

	OutOperation = DaDOper;
}

void UDraggInventoryWindow::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UAdditionalDaDWidget* DragWidget = Cast<UAdditionalDaDWidget>(InOperation);
	
	if (DragWidget)
	{
		//지금 Dragging하고 있는게 Widget이고,
		//Dragging 중인 Widget과 이 Widget이 다른 위젯이면 이 Widget을 비활성화 한다.

		if (DragWidget->AdditionalWidget->GetName() != GetName())
		{
			//UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::HitTestInvisible"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	
	
}

void UDraggInventoryWindow::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UAdditionalDaDWidget* DragWidget = Cast<UAdditionalDaDWidget>(InOperation);
	//UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::NativeOnDragLeave"));
	if (DragWidget)
	{
		//Dragging중인 Widget이 이 Widget과 같은거라면 이 Widget을 지워준다.
		//DragLeave함수는 DragDetected이후, Dragging할때 호출된다. 복제본의 원본을 삭제하는거임.
		if (DragWidget->AdditionalWidget->GetName() == GetName())
		{
			RemoveFromParent();
		}
	}
	
}

void UDraggInventoryWindow::CloseWindow()
{
	UE_LOG(LogTemp,Warning,TEXT("DraggingInventoryWindow::ClsoeWindow func called"));
	OnCloseWindow.Broadcast(this);
}

/*
FReply UDraggInventoryWindow::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	bMouseDown = false;

	//FirstMousePos = FVector2D(0.f);
	//MousePos = FVector2D(0.f);

	FEventReply DragPressEve = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	FEventReply ReleaseMouseEve = UWidgetBlueprintLibrary::ReleaseMouseCapture(DragPressEve);

	return ReleaseMouseEve.NativeReply;
}
*/


//FReply UDraggInventoryWindow::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	FReply ReturnReply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	//if(bMouseDown == false) return ReturnReply;

	/*
	FVector2D LocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	FVector2D SettingThisVec = LocalPos - FirstMousePos;
	SetRenderTranslation(SettingThisVec);*/
	
	
	/*FVector2D Offset = 
	(InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - InGeometry.AbsoluteToLocal(InMouseEvent.GetLastScreenSpacePosition()));

	FVector2D ThisPos = InGeometry.GetAbsolutePosition();
	SetRenderTranslation(Offset+ThisPos);*/

	//FVector2D LocCalc = GetTickSpaceGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	//SetRenderTranslation(LocCalc);

	/*FVector2D InnerLoc = 
		(GetParent()->GetTickSpaceGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - GetParent()->GetTickSpaceGeometry().GetLocalSize() / 2.0f) - (MousePos - GetDesiredSize() / 2.0f);

	auto Localsize = GetParent()->GetTickSpaceGeometry().GetLocalSize();
	FVector2D Min = ((Localsize / 2.0f) - (GetDesiredSize() / 2.0f)) * -1.0f;
	FVector2D Max = Localsize - ((Min * -1.0f) + GetDesiredSize());
	FVector2D Loc = {FMath::Clamp(InnerLoc.X, Min.X, Max.X), FMath::Clamp(InnerLoc.Y, Min.Y, Max.Y)};

	SetRenderTranslation(Loc);*/
	
//	return ReturnReply;
//}


/*
FReply UBBBuildWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (CanMove)
	{
		UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Canvas->GetChildAt(0)->Slot);

		if (IsValid(Slot))
		{
			FVector2D Offset = (InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - InGeometry.AbsoluteToLocal(InMouseEvent.GetLastScreenSpacePosition())) * UWidgetLayoutLibrary::GetViewportScale(GetWorld()->GetGameViewport());
			FVector2D SlotPosition = Slot->GetPosition();

			Slot->SetPosition(SlotPosition + Offset);
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}
*/