// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/TooltipWidget.h"
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
		//FIntPoint size;
		//FIntPoint Tempsize = Obj->GetItemSize();
		FIntPoint size = Obj->GetItemSize();

		/*if (Obj->bRotated)
		{
			size.X = Tempsize.Y;
			size.Y = Tempsize.X;
		}
		else
		{
			size = Tempsize;
		}*/
		if (Obj->ItemInfo.DataAsset)
		{
			ItemIcon->SetBrushSize(FVector2D(size.X * tilesize, size.Y * tilesize));
			ItemIcon->SetBrushFromMaterial(Obj->ItemInfo.DataAsset->Icon);
			ItemName->SetText(Obj->ItemInfo.DataAsset->ItemName);
			ItemDesc->SetText(Obj->ItemInfo.DataAsset->ItemDescription);
		}
		
		
	}
}