// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

UItem::UItem()
{
	//ItemDisplayName = FText(TEXT("Item"));
	ItemDisplayName = FText::FromString("Item");
	UseActionText = FText::FromString("Use"); //FText(TEXT("Use"));
}


//void UItem::Use_Implementation(class AMainCharacter* Character)
//{
//
//}