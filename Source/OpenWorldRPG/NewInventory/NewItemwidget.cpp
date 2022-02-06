// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Styling/SlateBrush.h"

//BindWidget nullptr issue solution 1 -> add runtime headerfile


bool UNewItemwidget::Initialize() 
{
	bool bReturn = Super::Initialize();

	return bReturn;
}

void UNewItemwidget::NativeConstruct()
{
	Super::NativeConstruct();
	//SynchronizeProperties();
	Refresh();
	

	/*BGSizeBox = NewObject<USizeBox>(this, USizeBox::StaticClass());
	ItemIcon = NewObject<UImage>(this, UImage::StaticClass());*/
	/*if (testsizebox)
	{
		BGSizeBox = testsizebox;
	}*/
	//FObjectProperty* Obj = FindFProperty<FObjectProperty>(this->GetClass(), FName("BGSizeBox"));
	//if(Obj)
	//{
	//	//testsizebox = Cast<USizeBox>(Obj);
	//	
	//}
	//Refresh(); //error
}


//void UNewItemwidget::SynchronizeProperties()
//{
//	Super::SynchronizeProperties();
//	if (ItemObj && BGSizeBox)
//	{
//		FIntPoint Itemsize = ItemObj->GetItemSize();
//
//		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);
//
//		BGSizeBox->SetWidthOverride(widgetsize.X);
//		BGSizeBox->SetHeightOverride(widgetsize.Y); //error
//
//		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
//		if (CanvasSlot)
//		{
//			CanvasSlot->SetSize(widgetsize);
//		}
//
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("sizebox is invalid"));
//	}
//}

void UNewItemwidget::Refresh()//_Implementation()
{
	
	if (ItemObj && BGSizeBox)
	{
		FIntPoint Itemsize = ItemObj->GetItemSize();

		widgetsize = FVector2D(Itemsize.X * Tilesize, Itemsize.Y * Tilesize);

		BGSizeBox->SetWidthOverride(widgetsize.X);
		BGSizeBox->SetHeightOverride(widgetsize.Y); //error

		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetSize(widgetsize);
			ItemIcon->SetBrush(GetIconImage());
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

		/* Item obj���� �Լ��� �ϳ� ����� rotate bool�� ���� icon�����ϱ� (icon, iconroated �߿��� �ϳ�)
		* �Ʒ��� �ӽ� �ڵ���.
		*/
		icon = UWidgetBlueprintLibrary::MakeBrushFromMaterial(ItemObj->icon, widgetsize.X, widgetsize.Y);

		//Item�� �������� �ش� event�� bind�� �Լ��� ȣ���Ѵ�.
		OnRemoved.Broadcast(ItemObj);
	}
	return icon;
}