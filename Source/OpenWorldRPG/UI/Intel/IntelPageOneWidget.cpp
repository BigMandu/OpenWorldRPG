// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Intel/IntelPageOneWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"


void UIntelPageOneWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ForceLayoutPrepass();
	FVector2D CLSize = Classification_TOP->GetDesiredSize();
	FVector2D ADSize = Additional_TOP->GetDesiredSize();

	//UE_LOG(LogTemp, Warning, TEXT("classiTop size : %s"), *CLSize.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("AD size : %s"), *ADSize.ToString());
}

void UIntelPageOneWidget::SettingPage(const FIntelligenceDataTable& Data, const int32& pageindex)
{
	FirstTitleItem->SetText(Data.From);
	SecondTitleItem->SetText(Data.To);
	
	//Reference나 Through가 있다면
	if ( Data.ReferenceORThrough.IsEmpty() == false )
	{
		ThirdText = NewObject<UTextBlock>(this,FName("TheThirdTextBlock"));
		PageCanvas->AddChildToCanvas(ThirdText);
		ThirdText->SetText(Data.ReferenceORThrough);
		ThirdText->SetFont(Data.ContentFont);
		ThirdText->SetColorAndOpacity(Data.TextColor);
		
	}

	FourthTitleItem->SetText(Data.Subject);


	DateText->SetText(Data.Date);

	Classification_TOP->SetText(Data.ClassificationStamp);
	Classification_BOT->SetText(Data.ClassificationStamp);
	Additional_TOP->SetText(Data.AdditionalStamp);
	Additional_BOT->SetText(Data.AdditionalStamp);

	if ( Data.Context.IsValidIndex(pageindex) )
	{
		ContentText->SetText(Data.Context[pageindex]);
	}
	else
	{
		ContentText->SetText(FText::FromString(""));
	}
	

	if ( Data.IntelLogo )
	{
		LogoImage->SetBrushFromTexture(Data.IntelLogo);
		LogoImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		LogoImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	//ForceLayoutPrepass();
	//AdjustWidgetLocation();
}

//Classification, Additional의 textblock위젯을 중앙으로 오게끔 한다.
void UIntelPageOneWidget::AdjustWidgetLocation()
{
	//GetDesiredSize를 하기전에 Prepass함수를 이용해 다음 프레임을 넘기는것처럼 사전패스를 이용한다.
	ForceLayoutPrepass();

	//Classification과 Additional의 Size를 얻어온다. BOT은 어차피 위의 텍스트와 동일한 크기를 가지니
	//각각 하나만 크기만 가져와 동일한 계산의 X값을 넣어줌
	FVector2D CLSize = Classification_TOP->GetDesiredSize();
	FVector2D ADSize = Additional_TOP->GetDesiredSize();

	//size의 크기를 반으로 나눠절반 만큼만 - 방향으로 가게끔 한다.
	float ClassificationXSize = FVector2D(CLSize/2).X;
	float AdditionalXSize = FVector2D(ADSize/2).X;

	
	FVector2D BeforePosition = GetUWidgetPosition(Classification_TOP);
	FVector2D AdjustPosition = FVector2D(ClassificationXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Classification_TOP, AdjustPosition);

	BeforePosition = GetUWidgetPosition(Classification_BOT);
	AdjustPosition = FVector2D(ClassificationXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Classification_BOT, AdjustPosition);


	BeforePosition = GetUWidgetPosition(Additional_TOP);
	AdjustPosition = FVector2D(AdditionalXSize* -1, BeforePosition.Y);
	SetUWidgetPosition(Additional_TOP, AdjustPosition);

	BeforePosition = GetUWidgetPosition(Additional_BOT);
	AdjustPosition = FVector2D(AdditionalXSize * -1, BeforePosition.Y);
	SetUWidgetPosition(Additional_BOT, AdjustPosition);


	//Third Title의 position을 adjust한다.
	if ( ThirdText != nullptr )
	{
		UCanvasPanelSlot* Pslot = Cast<UCanvasPanelSlot>(ThirdText->Slot);
		//상단 중앙으로 Anchor를 한다.
		FAnchors anch = FAnchors(0.5,0.f,0.5f,0.f);
		Pslot->SetAnchors(anch);

		//Pslot->SetAlignment();
		FVector2D UpperWidgetPos = GetUWidgetPosition(SecondTitleItem);
		FVector2D NewPosition = FVector2D(UpperWidgetPos.X, UpperWidgetPos.Y + 30.f);
		SetUWidgetPosition(ThirdText, NewPosition);
	}
	

	//FourthTitle의 position을 adjust한다.
	//ThirdText가 있는지 없는지에 따라 FourthTitle위치를 조정한다.
	FVector2D AdjustFourthPos = FVector2D::ZeroVector;
	if ( ThirdText == nullptr )
	{
		FVector2D UpperWidgetPos = GetUWidgetPosition(SecondTitleItem);
		AdjustFourthPos = FVector2D(UpperWidgetPos.X, UpperWidgetPos.Y + 30.f);
	}
	else
	{
		FVector2D UpperWidgetPos = GetUWidgetPosition(ThirdText);
		AdjustFourthPos = FVector2D(UpperWidgetPos.X, UpperWidgetPos.Y + 30.f);
	}
	SetUWidgetPosition(FourthTitleItem, AdjustFourthPos);


	//for debug
	/*UE_LOG(LogTemp,Warning,TEXT("classiTop size : %s"), *CLSize.ToString());	
	UE_LOG(LogTemp,Warning,TEXT("AD size : %s"), *ADSize.ToString());*/
}



/* 아래는 자주 사용하는 함수 */

FVector2D UIntelPageOneWidget::GetUWidgetPosition(UWidget* WidgetElement)
{
	FVector2D ReturnVector = FVector2D::ZeroVector;
	if ( UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(WidgetElement->Slot) )
	{
		ReturnVector = PanelSlot->GetPosition();
	}
	return ReturnVector;
}

void UIntelPageOneWidget::SetUWidgetPosition(UWidget* WidgetElement, FVector2D Position)
{
	if(WidgetElement == nullptr ) return;

	if ( UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(WidgetElement->Slot) )
	{
		PanelSlot->SetAutoSize(true);
		PanelSlot->SetPosition(Position);
	}
}