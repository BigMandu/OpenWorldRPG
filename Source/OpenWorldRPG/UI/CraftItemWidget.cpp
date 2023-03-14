// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CraftItemWidget.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"

void UCraftItemWidget::NativeConstruct()
{
	
}

void UCraftItemWidget::InitCraftItemWidget(UBasePDA* CraftableItemPDA, bool bIsCraftItem, FName MotherName)
{	
	if (CraftableItemPDA != nullptr)
	{		
		CraftItemDA = CraftableItemPDA;
		Thumbnail->SetBrushFromMaterial(CraftItemDA->Icon);
		
		ItemBorder->SetBrushColor(FLinearColor::White);

		Name->SetVisibility(ESlateVisibility::Collapsed);
		CountBox->SetVisibility(ESlateVisibility::Collapsed);
		bCraftItem = bIsCraftItem;

		//CraftItem이 아닌 IngredientWidget이면 Name을 보이게 한다.
		if(bCraftItem == false)
		{
			CountBox->SetVisibility(ESlateVisibility::Visible);
			Name->SetVisibility(ESlateVisibility::Visible);
			Name->SetText(CraftItemDA->ItemName);
			MotherItemName = MotherName;
		}
	}
}

void UCraftItemWidget::SetBorderColor(FLinearColor Color)
{
	if (bCraftItem)
	{
		ItemBorder->SetBrushColor(Color);
	}
}

void UCraftItemWidget::SetCurrentCountTextBox(int32 NumberofCount)
{
	const FString CountStr = FString::FromInt(NumberofCount);
	CurrentCount->SetText(FText::FromString(CountStr));

	CurrentCount_num = NumberofCount;
}

void UCraftItemWidget::SetNeedCountTextBox(int32 NumberofCount)
{
	const FString CountStr = FString::FromInt(NumberofCount);
	NeedCount->SetText(FText::FromString(CountStr));

	NeedCount_num = NumberofCount;
}


//For Craft Item
FReply UCraftItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply ReturnReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	FEventReply Reply;
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if(bCraftItem && CraftItemDA.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("UCraftItemWidget::LeftMousebutton Down"));			

			//UCraftSystemWidget::SettingRecipe와 Bind함
			OnCraftWidgetClicked.Broadcast(this);
		}
	}
	return ReturnReply;
}


//For Ingrident Item
bool UCraftItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UCustomDDOperation* CusDDOper = Cast<UCustomDDOperation>(InOperation);
	if (CusDDOper)
	{
		//DragItem과 thisWidget의 DA가 같은경우 Drop가능
		if (CusDDOper->ItemObj->ItemInfo.DataAsset == CraftItemDA)
		{
			int32 DroppedItemCount = CusDDOper->ItemObj->ItemInfo.Count;
			int32 RemainCount = NeedCount_num - CurrentCount_num;

			//Drop하는 ItemCount가 NeedCount보다 큰 경우
			if (DroppedItemCount > RemainCount)
			{
				CusDDOper->ItemObj->RemoveCount(RemainCount);
				CurrentCount_num = NeedCount_num;
			}
			//Drop하는 Item개수가 NeedCount와 같거나 작은경우 (옮긴후 없어져야함)
			else
			{
				AMainCharacter * Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
				if (Player)
				{
					Player->BaseInventoryComp->RemoveItemCount(CusDDOper->ItemObj, DroppedItemCount);
					CurrentCount_num += DroppedItemCount;
				}
				/*CusDDOper->ItemObj->RemoveCount(DroppedItemCount);
				CusDDOper->ItemObj->MotherStorage->RemoveItem(CusDDOper->ItemObj);
				CurrentCount_num += DroppedItemCount;*/
			}

			SetCurrentCountTextBox(CurrentCount_num);


			OnDropCraftWidget.Broadcast(this,CusDDOper);

		}
	}
	
	return bReturn;
}
