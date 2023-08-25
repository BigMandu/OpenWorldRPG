// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Intel/IntelPageOtherWidget.h"

void UIntelPageOtherWidget::NativeConstruct()
{
	ForceLayoutPrepass();
	FVector2D CLSize = Classification_TOP->GetDesiredSize();
	FVector2D ADSize = Additional_TOP->GetDesiredSize();

	/*UE_LOG(LogTemp, Warning, TEXT("classiTop size : %s"), *CLSize.ToString());
	UE_LOG(LogTemp, Warning, TEXT("AD size : %s"), *ADSize.ToString());*/
}


void UIntelPageOtherWidget::AdjustWidgetLocation()
{
	ForceLayoutPrepass();

	FVector2D CLSize = Classification_TOP->GetDesiredSize();
	FVector2D ADSize = Additional_TOP->GetDesiredSize();

	float ClassificationXSize = FVector2D(CLSize / 2).X;
	float AdditionalXSize = FVector2D(ADSize / 2).X;

	FVector2D BeforePosition = GetUWidgetPosition(Classification_TOP);
	FVector2D AdjustPosition = FVector2D(ClassificationXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Classification_TOP, AdjustPosition);

	BeforePosition = GetUWidgetPosition(Classification_BOT);
	AdjustPosition = FVector2D(ClassificationXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Classification_BOT, AdjustPosition);


	BeforePosition = GetUWidgetPosition(Additional_TOP);
	AdjustPosition = FVector2D(AdditionalXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Additional_TOP, AdjustPosition);

	BeforePosition = GetUWidgetPosition(Additional_BOT);
	AdjustPosition = FVector2D(AdditionalXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Additional_BOT, AdjustPosition);


	//Page number textblock의 position adjust (중앙으로 오게 한다.)
	FVector2D PageNumSize = PageNumber->GetDesiredSize();
	float NumXSize = FVector2D(PageNumSize / 2).X;

	BeforePosition = GetUWidgetPosition(PageNumber);//->GetPosition();
	AdjustPosition = FVector2D(NumXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(PageNumber, AdjustPosition);
	

	//if ( UCanvasPanelSlot* TopPanelSlot = Cast<UCanvasPanelSlot>(Classification_TOP->Slot) )
	//{
	//	FVector2D TopOriginPos = TopPanelSlot->GetPosition();
	//	FVector2D TopAdjustPos = FVector2D(ClassificationXSize * -1, TopOriginPos.Y);

	//	TopPanelSlot->SetPosition(TopAdjustPos);
	//}

	//if ( UCanvasPanelSlot* BotPanelSlot = Cast<UCanvasPanelSlot>(Classification_BOT->Slot) )
	//{
	//	FVector2D BotOriginPos = BotPanelSlot->GetPosition();
	//	FVector2D BotAdjustPos = FVector2D(ClassificationXSize * -1, BotOriginPos.Y);

	//	BotPanelSlot->SetPosition(BotAdjustPos);
	//}

	//if ( UCanvasPanelSlot* TopAdPanelSlot = Cast<UCanvasPanelSlot>(Additional_TOP->Slot) )
	//{
	//	FVector2D TopOriginPos = TopAdPanelSlot->GetPosition();
	//	FVector2D TopAdjustPos = FVector2D(ClassificationXSize * -1, TopOriginPos.Y);

	//	TopAdPanelSlot->SetPosition(TopAdjustPos);
	//}

	//if ( UCanvasPanelSlot* BotAdPanelSlot = Cast<UCanvasPanelSlot>(Additional_BOT->Slot) )
	//{
	//	FVector2D BotOriginPos = BotAdPanelSlot->GetPosition();
	//	FVector2D BotAdjustPos = FVector2D(ClassificationXSize * -1, BotOriginPos.Y);

	//	BotAdPanelSlot->SetPosition(BotAdjustPos);
	//}

	//PageNumber Text의 위치 조정
	/*if ( UCanvasPanelSlot* PageNumPanelSlot = Cast<UCanvasPanelSlot>(PageNumber->Slot) )
	{
		FVector2D PageNumSize = PageNumber->GetDesiredSize();
		float NumXSize = FVector2D(PageNumSize/2).X;

		FVector2D PageNumOriginPos = PageNumPanelSlot->GetPosition();
		FVector2D NumAdjustLo = FVector2D(NumXSize * -1, PageNumOriginPos.Y);

		PageNumPanelSlot->SetPosition(NumAdjustLo);
	}*/
}


FVector2D UIntelPageOtherWidget::GetUWidgetPosition(UWidget* WidgetElement)
{
	FVector2D ReturnVector = FVector2D::ZeroVector;
	if ( UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(WidgetElement->Slot) )
	{
		ReturnVector = PanelSlot->GetPosition();
	}
	return ReturnVector;
}

void UIntelPageOtherWidget::SetUWidgetPosition(UWidget* WidgetElement, FVector2D Position)
{
	if ( WidgetElement == nullptr ) return;

	if ( UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(WidgetElement->Slot) )
	{
		PanelSlot->SetAutoSize(true);
		PanelSlot->SetPosition(Position);
	}
}