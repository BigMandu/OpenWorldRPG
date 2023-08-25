// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Intel/IntelPageBaseWidget.h"
#include "Components/CanvasPanelSlot.h"


void UIntelPageBaseWidget::SettingFocus()
{
	
	this->bIsFocusable = true;
	PanningCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SetFocus();
	this->SetKeyboardFocus();
}

FReply UIntelPageBaseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(ListWidget.IsValid() == false) return FReply::Unhandled();

	FKey KeyDown = InKeyEvent.GetKey();

	//오른쪽 화살표 또는 D키 누르면 다음 페이지로.
	if ( KeyDown == EKeys::Right || KeyDown == EKeys::D )
	{
		ListWidget->ToNextIntelPage();
	}
	else if ( KeyDown == EKeys::Left || KeyDown == EKeys::A )
	{
		ListWidget->ToPrevIntelPage();
	}
	
	return FReply::Handled();
}

/* Zoom */
FReply UIntelPageBaseWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//up시 +, down시 -
	float ScrollDelta = InMouseEvent.GetWheelDelta();
	CurrentScaleX += ScrollDelta;
	CurrentScaleY += ScrollDelta;

	CurrentScaleX = FMath::Clamp(CurrentScaleX, 1.f, 3.f);
	CurrentScaleY = FMath::Clamp(CurrentScaleY, 1.f, 3.f);

	FVector2D NewScale = FVector2D(CurrentScaleX, CurrentScaleY);

	SetRenderScale(NewScale);

	return FReply::Handled();
}



FReply UIntelPageBaseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if ( InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp,Warning,TEXT("Left button down"));
		bMouseButtonDown = true;
	}
	return FReply::Handled();
}
FReply UIntelPageBaseWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	if ( InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("Left button up"));
		bMouseButtonDown = false;
	}

	return FReply::Handled();
}

//Intel widget을 panning한다.
FReply UIntelPageBaseWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//zoom한 상태가 아니면 panning을 하지 않는다.
	if ( RenderTransform.Scale == FVector2D::ZeroVector )
	{
		return FReply::Unhandled();
	}
	//LMB down이 아니면 panning을 하지 않는다.
	if ( bMouseButtonDown == false )
	{
		return FReply::Unhandled();
	}

	PanningWidget(InMouseEvent);

	return FReply::Handled();
}


void UIntelPageBaseWidget::PanningWidget(const FPointerEvent& InMouseEvent)
{

	if ( UCanvasPanelSlot* panCanvasSlot = Cast<UCanvasPanelSlot>(PanningCanvas->Slot) )
	{
		FVector2D CurrentPaperPos = panCanvasSlot->GetPosition();
		FVector2D CurrentCursorPos = InMouseEvent.GetCursorDelta();

		FVector2D NewPaperPos(CurrentPaperPos+CurrentCursorPos);

		panCanvasSlot->SetPosition(NewPaperPos);

	}

}