// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/TooltipWidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


//void UTooltipWidget::NativeConstruct()
//{
//	Super::NativeConstruct();
//
//}

void UTooltipWidget::SetData(UNewItemObject* Obj, float tilesize)
{
	if (Obj)
	{
		FIntPoint size;
		FIntPoint Tempsize = Obj->GetItemSize();
		if (Obj->bRotated)
		{
			size.X = Tempsize.Y;
			size.Y = Tempsize.X;
		}
		else
		{
			size = Tempsize;
		}
		

		ItemIcon->SetBrushSize(FVector2D(size.X * tilesize, size.Y * tilesize));
		if (Obj->icon)
		{
			ItemIcon->SetBrushFromMaterial(Obj->icon);
			ItemName->SetText(Obj->itemName);
			ItemDesc->SetText(Obj->itemDesc);
		}
		
		
	}
}