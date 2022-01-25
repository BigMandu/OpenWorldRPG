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

		/* Item obj���� rotate bool�� ���� icon�����ϱ� (icon, iconroated �߿��� �ϳ�)*/
		icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->icon, widgetsize.X, widgetsize.Y);

		//ItemObj->icon
		//ReturnIcon = FSlateBrush(icon);
		/*
		https://youtu.be/pG7KFaCwvJw?t=1237 ������.
		���� ó�� �ؾߵ� -> build.cs���� slate �κ� �ּ� Ǯ���ָ� �ذ�.

		https://youtu.be/pG7KFaCwvJw?t=1351 ������.
		Event�߰��ؾߵ�. 

		Item�� Inventory���� �������� Event�� �߻���Ų��.
		*/

		OnRemoved.Broadcast(ItemObj);
	}
	return icon;
}