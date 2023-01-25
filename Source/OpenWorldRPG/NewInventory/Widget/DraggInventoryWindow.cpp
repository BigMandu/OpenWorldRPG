// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/DraggInventoryWindow.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalDaDWidget.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"




void UDraggInventoryWindow::NativeConstruct()
{
	Super::NativeConstruct();
	/*if(GridInventory && GridInventory->StorageObj)
	{
		StorageWidgetName->SetText(GridInventory->StorageObj->ItemInfo.DataAsset->ItemName);*/

		UE_LOG(LogTemp, Warning, TEXT("DraggingInventoryWindow::Binding ClsoeWindow func"));
		CloseButton->OnClicked.AddDynamic(this,&UDraggInventoryWindow::CloseWindow);
	//}
}

FReply UDraggInventoryWindow::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry,InMouseEvent);
	bMouseDown = false;

	FEventReply ReturnReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	ReturnReply = UWidgetBlueprintLibrary::ReleaseMouseCapture(ReturnReply);

	OnReleaseWindow.Broadcast();

	return FReply::Unhandled();//ReturnReply.NativeReply;
}


FReply UDraggInventoryWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry,InMouseEvent);
	
	//FEventReply ReturnReply;
	
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		/*bMouseDown = true;

		FirstMousePos = GetParent()->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		UE_LOG(LogTemp,Warning,TEXT("DraggInv : OnMouseButtonDown : FirstMousePos = %s "),*FirstMousePos.ToString());

		ReturnReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		ReturnReply = UWidgetBlueprintLibrary::CaptureMouse(ReturnReply,this);*/

		DetectDragWidget(InMouseEvent,this);
		//return  FReply::Unhandled(); //ReturnReply.NativeReply;
	}
	return FReply::Unhandled(); //ReturnReply.NativeReply;
}

/*
FReply UDraggInventoryWindow::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply ReturnReply = Super::NativeOnMouseMove(InGeometry,InMouseEvent);

	if (bMouseDown) // && HeaderBorder->IsHovered())
	{		
			
		FVector2D Offset = (GetParent()->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - GetParent()->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetLastScreenSpacePosition()))
		* UWidgetLayoutLibrary::GetViewportScale(GetWorld()->GetGameViewport());
		//FVector2D CurrentPosition =  //this-> Slot->GetPosition();

		
		//Slot->SetPosition(SlotPosition + Offset);



		FVector2D MouseScreenPos = GetParent()->GetTickSpaceGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		const FVector2D ClickLoc = MouseScreenPos+Offset;//MouseScreenPos - FirstMousePos;

		UE_LOG(LogTemp, Warning, TEXT("DraggInv : NativeOnMouseMove : Offset = %s "), *Offset.ToString());
		UE_LOG(LogTemp, Warning, TEXT("DraggInv : NativeOnMouseMove : FirstMousePos = %s "), *FirstMousePos.ToString());
		UE_LOG(LogTemp, Warning, TEXT("DraggInv : NativeOnMouseMove : MouseScreenPos = %s "), *MouseScreenPos.ToString());
		UE_LOG(LogTemp, Warning, TEXT("DraggInv : NativeOnMouseMove : ClickLoc = %s "), *ClickLoc.ToString());

		SetRenderTranslation(ClickLoc);
	}
	return ReturnReply;
}
*/


FReply UDraggInventoryWindow::DetectDragWidget(const FPointerEvent& InMouseEvent, UWidget* DragWidget)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnDraggingWindow.Broadcast(this, InMouseEvent);
		OnButtonDownWindow.Broadcast(this);
		/*
		if(HeaderBorder->IsHovered())
		{

			OnDraggingWindow.Broadcast(this, InMouseEvent);
			
			//FEventReply Reply;
			//Reply.NativeReply = FReply::Handled();

			//if (DragWidget)
			//{
			//	TSharedPtr<SWidget> SlateDetectedWidget = DragWidget->GetCachedWidget();
			//

			//	if (SlateDetectedWidget.IsValid())
			//	{
			//		//DetectDrag�� ���� ȣ��ȴٸ�, NativeOnDragDetected�� ȣ��ȴ�.
			//		Reply.NativeReply = Reply.NativeReply.DetectDrag(SlateDetectedWidget.ToSharedRef(),EKeys::LeftMouseButton);
			//	
			//		return Reply.NativeReply;
			//	}
			//}
			return FReply::Handled();
		}
		else
		{
			OnButtonDownWindow.Broadcast(this);
		}
	*/
	}
	return FReply::Unhandled();
}

//�� ������ ��ƾ�� NewInventory�� NativeOnDrop�� ȣ��ȴ�.
/*
void UDraggInventoryWindow::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry,InMouseEvent,OutOperation);
	
	UAdditionalDaDWidget* DaDOper = NewObject<UAdditionalDaDWidget>();

	//SetVisibility(ESlateVisibility::HitTestInvisible);
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
		���� Dragging�ϰ� �ִ°� Widget�̰�,
		Dragging ���� Widget�� �� Widget�� �ٸ� �����̸� �� Widget�� ��Ȱ��ȭ �Ѵ�.
		
		if (DragWidget->AdditionalWidget->GetName() != GetName())
		{
			UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::HitTestInvisible"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	
	
}

void UDraggInventoryWindow::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{	
	UAdditionalDaDWidget* DragWidget = Cast<UAdditionalDaDWidget>(InOperation);
	UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::NativeOnDragLeave"));
	if (DragWidget)
	{
		//Dragging���� Widget�� �� Widget�� �����Ŷ�� �� Widget�� �����ش�.
		//DragLeave�Լ��� DragDetected����, Dragging�Ҷ� ȣ��ȴ�. �������� ������ �����ϴ°���.
		//UDraggInventoryWindow* InvWindow = Cast<UDraggInventoryWindow>(DragWidget->AdditionalWidget);
		if (DragWidget->AdditionalWidget->GetName() == GetName())
		{
			UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::NativeOnDragLeave / if statement success"));
			RemoveFromParent();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DraggingInvWidget::NativeOnDragLeave / if statement fail"));
		}
	}
	
}
*/
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