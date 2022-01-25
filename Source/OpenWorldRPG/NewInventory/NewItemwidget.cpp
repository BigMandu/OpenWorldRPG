// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
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

	//Refresh(); //error
}

void UNewItemwidget::Refresh()
{
	if (ItemObj && BackgroundSizeBox)
	{
		FIntPoint Itemsize = ItemObj->GetItemSize();
		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);

		BackgroundSizeBox->SetWidthOverride(widgetsize.X);
		BackgroundSizeBox->SetHeightOverride(widgetsize.Y); //error

		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetSize(widgetsize);
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
	}
}

FSlateBrush UNewItemwidget::GetIconImage()
{
	FSlateBrush icon;
	if (ItemObj)
	{

		/* Item obj에서 rotate bool에 따른 icon리턴하기 (icon, iconroated 중에서 하나)*/
		icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->icon, widgetsize.X, widgetsize.Y);

		//ItemObj->icon
		//ReturnIcon = FSlateBrush(icon);
		/*
		https://youtu.be/pG7KFaCwvJw?t=1237 여기임.
		에러 처리 해야됨 -> build.cs에서 slate 부분 주석 풀어주면 해결.

		https://youtu.be/pG7KFaCwvJw?t=1351 여기임.
		Event추가해야됨. 

		Item이 Inventory에서 지워질때 Event를 발생시킨다.
		*/

		OnRemoved.Broadcast(ItemObj);
	}
	return icon;
}