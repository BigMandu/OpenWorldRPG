// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"

bool UNewItemwidget::Initialize() 
{
	bool bReturn = Super::Initialize();

	return bReturn;
}

void UNewItemwidget::NativeConstruct()
{
	Super::NativeConstruct();

	Refresh();
}

void UNewItemwidget::Refresh()
{
	if(ItemObj)
	{
		FIntPoint Itemsize = ItemObj->GetItemSize();
		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);

		BackgroundSizeBox->SetWidthOverride(widgetsize.X);
		BackgroundSizeBox->SetHeightOverride(widgetsize.Y);

		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetSize(widgetsize);
		}
		
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
		에러 처리 해야됨
		*/
	}
	return icon;
}