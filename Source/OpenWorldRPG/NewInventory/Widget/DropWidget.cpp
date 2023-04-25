// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/DropWidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "DrawDebugHelpers.h"
#include <OpenWorldRPG/NewInventory/CustomDDOperation.h>
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include <OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h>

bool UDropWidget::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UE_LOG(LogTemp, Warning, TEXT("UDropWidget::NativeOnDrop // Drop detected"));


	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	UNewInventoryComponent* BaseInvComp = Cast<UNewInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UNewInventoryComponent::StaticClass()));

	if(!DDOper || !BaseInvComp ) return false;

	UWorld* World = GetWorld();
	if ( DDOper->ItemObj->bIsDragging && World )
	{
		FTransform form;

		FVector OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
		FVector OwnerForwardLo = GetOwningPlayerPawn()->GetActorForwardVector();
		const FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;
		
		form.SetLocation(DropLocation);
		DrawDebugSphere(World, DropLocation, 10.f, 6, FColor::Green, false, 3.f,(uint8)0U, 5.f);

		AActor* DropItem = nullptr;
		
		if ( UCustomPDA* EquipPDA = Cast<UCustomPDA>(DDOper->ItemObj->ItemInfo.DataAsset) )
		{
			DropItem = UCustomInventoryLibrary::SpawnEquipment(World,DDOper->ItemObj);
			
		}
		else if ( UBasePDA* BasePDA = Cast<UBasePDA>(DDOper->ItemObj->ItemInfo.DataAsset) )
		{
			DropItem = UCustomInventoryLibrary::SpawnItem(World, DDOper->ItemObj);
		}

		UCustomInventoryLibrary::RemoveFromPreviousMotherContainer(DDOper->ItemObj);
		if(!DropItem ) return false;
		
		//World->SpawnActor<AActor>(DropItem->GetClass(), form);
		DropItem->SetActorLocation(DropLocation);


		ChangeState();
		return true;
	}
	return false;
	//UNewItemObject* Obj = Cast<UNewItemObject>(InOperation->Payload);
	//if (Obj)
	//{
	//	bReturnNormal = true;

	//	UWorld* World = GetWorld();
	//	FTransform form;

	//	FVector OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
	//	FVector OwnerForwardLo = GetOwningPlayerPawn()->GetActorForwardVector();
	//	const FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;
	//	
	//	form.SetLocation(DropLocation);
	//	DrawDebugSphere(World, DropLocation, 10.f, 6, FColor::Green, false, 3.f,(uint8)0U, 5.f);

	//	
	//	AActor* DropItem = World->SpawnActor<AActor>(Obj->GetItemClass(), form);
	//	
	//	//World->SpawnActor(Obj->GetItemClass(), DropLocation);
	//	
	//	ChangeState();
	//	UE_LOG(LogTemp, Warning, TEXT("Drop item name : %s"), *DropItem->GetFName().ToString());
	//	return true;
	//}
	//return false;
}

void UDropWidget::ChangeState()
{

	if (bReturnNormal)
	{
		DropBorder->SetBrushColor(FLinearColor(0.0, 0.0, 0.0, 0.0));
		DropText->SetText(NormalText);
		
		bReturnNormal = false;
		
	}
	else
	{
		DropBorder->SetBrushColor(FLinearColor(0.5, 0.5, 0.5, 0.2));
		DropText->SetText(DragDetectText);
	}
}
