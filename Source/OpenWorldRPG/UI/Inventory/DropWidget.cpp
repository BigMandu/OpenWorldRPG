// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/DropWidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "DrawDebugHelpers.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include <OpenWorldRPG/NewInventory/CustomDDOperation.h>
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include <OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h>

#define DROPWIDGETDEBUG 0

bool UDropWidget::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
#if DROPWIDGETDEBUG
	UE_LOG(LogTemp, Warning, TEXT("UDropWidget::NativeOnDrop // Drop detected"));
#endif
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	UNewInventoryComponent* BaseInvComp = Cast<UNewInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UNewInventoryComponent::StaticClass()));
	UWorld* World = GetWorld();

	if(!DDOper || !BaseInvComp || !World) return false;
	

	if (DDOper->ItemObj->bIsDragging)
	{
		FVector OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
		FVector OwnerForwardLo = GetOwningPlayerPawn()->GetActorForwardVector();
		const FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;

		FTransform form;
		form.SetLocation(DropLocation);
#if DROPWIDGETDEBUG
		DrawDebugSphere(World, DropLocation, 10.f, 6, FColor::Green, false, 3.f,(uint8)0U, 5.f);
#endif
		bool bIsNeedToUpdateWeaponParts = false;
		AItem* DropItem = nullptr;
		
		if ( UCustomPDA* EquipPDA = Cast<UCustomPDA>(DDOper->ItemObj->ItemInfo.DataAsset) )
		{
			DropItem = UCustomInventoryLibrary::SpawnEquipment(World,DDOper->ItemObj);
			if(UWeaponPDA* WeapPDA = Cast<UWeaponPDA>(EquipPDA) )
			{
				bIsNeedToUpdateWeaponParts = true;
			}			
		}
		else if ( UBasePDA* BasePDA = Cast<UBasePDA>(DDOper->ItemObj->ItemInfo.DataAsset) )
		{
			DropItem = UCustomInventoryLibrary::SpawnItem(World, DDOper->ItemObj);
		}

		UCustomInventoryLibrary::RemoveFromPreviousMotherContainer(DDOper->ItemObj);

		if(!DropItem ) return false;

		//WeaponType이라면 WPM을 update해준다.
		if (bIsNeedToUpdateWeaponParts)
		{
			AWeapon* Weapon = Cast<AWeapon>(DropItem);
			Weapon->WeaponDataAsset = Cast<UWeaponPDA>(DDOper->ItemObj->ItemInfo.DataAsset);

			//아래는 WPM을 무조건 생성하는건데, 버그를 유발하는거 같아 주석처리함 //WPMO bug fix
			//사실, 위에서 SpawnEquipment 함수내부에서 이미 WPM을 생성한다. 즉, 중복 생성 하고 있었던것..
			//Weapon->SettingWeaponPartsManager();
			UCustomInventoryLibrary::SetWeaponPartsManager(DDOper->ItemObj, Weapon);

			//Parts를 Update만 해준다.
			Weapon->UpdateWeaponParts();
		}
		DropItem->SetActorLocation(DropLocation);

		//버리면 DDOper의 Itemobj는 알아서 destory된다. conditionbegindestory인가가 호출됨.

		
		ChangeState(true);

		return true;
	}
	return false;
}


void UDropWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	ChangeState(false);
}
void UDropWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	
	ChangeState(true);

}


void UDropWidget::ChangeState(bool bNormalState)
{

	if (bNormalState)
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
