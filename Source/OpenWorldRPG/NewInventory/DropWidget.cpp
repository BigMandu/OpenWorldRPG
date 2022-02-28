// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/DropWidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "DrawDebugHelpers.h"

bool UDropWidget::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UE_LOG(LogTemp, Warning, TEXT("Drop detected"));
	UNewItemObject* Obj = Cast<UNewItemObject>(InOperation->Payload);
	if (Obj)
	{
		bReturnNormal = true;

		UWorld* World = GetWorld();
		FTransform form;

		FVector OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
		FVector OwnerForwardLo = GetOwningPlayerPawn()->GetActorForwardVector();
		const FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;
		
		form.SetLocation(DropLocation);
		DrawDebugSphere(World, DropLocation, 10.f, 6, FColor::Green, false, 3.f,(uint8)0U, 5.f);

		
		AActor* DropItem = World->SpawnActor<AActor>(Obj->GetItemClass(), form);
		//World->SpawnActor(Obj->GetItemClass(), DropLocation);
		
		ChangeState();
		UE_LOG(LogTemp, Warning, TEXT("Drop item name : %s"), *DropItem->GetFName().ToString());
		return true;
	}
	return false;
}

void UDropWidget::ChangeState()
{
	if (bReturnNormal)
	{
		DropBorder->SetBrushColor(FLinearColor(0.0, 0.0, 0.0, 0.0));
		DropText->SetText(NormalText);
		
		bReturnNormal = false;
		/*FText Droptext = FText("Drop Item Here");
		DropText->SetText(FText::FromString(" Drop item Here"));
		*/
		
	}
	else
	{
		DropBorder->SetBrushColor(FLinearColor(0.5, 0.5, 0.5, 0.2));
		DropText->SetText(DragDetectText);
	}
}
