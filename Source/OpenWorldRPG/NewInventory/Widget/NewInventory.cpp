// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/DropWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalDaDWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/DraggInventoryWindow.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
//#include "OpenWorldRPG/NewInventory/ContainerWidget.h"

#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"


#include "Blueprint/DragDropOperation.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"


UNewInventory::UNewInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//InventoryWindowArray.Init()
}

void UNewInventory::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	if (EquipComp)
	{
		EquipmentWidget->EquipInitialize(EquipComp);
		EquipmentWidget->MainWidget = this;
		
	}

	StatusButton->OnClicked.AddDynamic(this, &UNewInventory::ChangeMainSwitchToStatus);
	InventoryButton->OnClicked.AddDynamic(this, &UNewInventory::ChangeMainSwitchToInventory);

	//MainCharacter의 Beginplay에서 호출하는것으로 변경함.
	//if (CharInvWidget)
	//{
	//	CharInvWidget->InitializeInventory(Main);
	//}
}

/* Navive Construct 보다 먼저 실행됨.*/
bool UNewInventory::Initialize()
{
	bool bResult = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;
	if (Main)
	{
		EquipComp = Main->Equipment;
	}
	return bResult;
}

void UNewInventory::SetRightWidget(UUserWidget* Widget)
{
	ContentBorder->ClearChildren();
	if (Widget)
	{
		ContentBorder->AddChild(Widget);
		Widget->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("Inventory::SetRightWidget, Widget name : %s"), *Widget->GetFName().ToString());
	}
}

void UNewInventory::ChangeRightSwitcher()
{
	if (Main)
	{
		AMainController* MainCon = Main->MainController;
		if (MainCon)
		{
			if (MainCon->bIsInteractLootBox || MainCon->bIsInteractCharacterLoot)
			{
				RightWidgetSwitcher->SetActiveWidgetIndex(2);
			}
			else
			{
				SetRightWidget(nullptr);
				RightWidgetSwitcher->SetActiveWidgetIndex(1);
			}
		}
	}
}

bool UNewInventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//UE_LOG(LogTemp,Warning,TEXT("MAIN WIDGET (NEWINVENTORY WBP) :: Native On drop Event."));
	
	UAdditionalDaDWidget* AdditionalDaD = Cast<UAdditionalDaDWidget>(InOperation);
	if (AdditionalDaD)
	{
		FVector2D DragWidgetOffset = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
		FVector2D DragWidgetLocation = DragWidgetOffset - AdditionalDaD->WidgetDragLocation;

		AdditionalDaD->AdditionalWidget->AddToViewport();
		AdditionalDaD->AdditionalWidget->SetVisibility(ESlateVisibility::Visible);
		AdditionalDaD->AdditionalWidget->SetPositionInViewport(DragWidgetLocation, false);

		//Canvas에 추가하면, DraggInventoryWindow에서 RemoveFromParent함수가 먹지 않아, AddToViewport로 대체했다.
		//UDraggInventoryWindow* StorageWindow = Cast<UDraggInventoryWindow>(AdditionalDaD->AdditionalWidget);
		//UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(StorageWindow);
		//StorageWindow->SetVisibility(ESlateVisibility::Visible);
		//CanvasSlot->SetPosition(DragWidgetLocation);

		
	}

	return bReturn;
}

void UNewInventory::BindingOpenWidgetFunc(UNewItemObject* ItemObj)
{
	
	if (ItemObj == nullptr)
	{
		UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
		if (!CPDA || (CPDA && CPDA->bHasStorage == false))
		{
			return;
		}
	}

	UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ItemObj);
	if (StorageObj)
	{
		OpenAdditionalWidget(StorageObj);
	}
}

void UNewInventory::OpenAdditionalWidget(UItemStorageObject* StorageObj)
{
	if (StorageObj && WStorageWindow)
	{
		//이미 켜있는 Widget인지 확인한다.
		for(auto OpenedWindow : InventoryWindowArray)
		{
			if (OpenedWindow->GridInventory->StorageObj == StorageObj)
			{
				return;
			}
		}

		UDraggInventoryWindow* StorageWindow = CreateWidget<UDraggInventoryWindow>(this, WStorageWindow);
		if (StorageWindow)
		{
			//초기화가 된적 없는 Storage면 초기화를 해준다.
			if (StorageObj->Inventory.IsValidIndex(0) == false)
			{
				UCustomPDA* CPDA = Cast<UCustomPDA>(StorageObj->ItemInfo.DataAsset);
				StorageObj->InitializeStorage(CPDA->StorageX, CPDA->StorageY, CPDA->TileSize);
			}


			StorageWindow->GridInventory->StorageObj = StorageObj;
			StorageWindow->GridInventory->GridInit();
			StorageWindow->AddToViewport();
			
			

			UE_LOG(LogTemp,Warning,TEXT("dragging inv window parent : %s"), *GetParent()->GetFName().ToString());
			
			//UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(StorageWindow);// ->AddChildToOverlay(StorageWindow);
			float MouseX;
			float MouseY;
			if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
			{
				FVector2D CurrentMousePos = FVector2D(MouseX, MouseY);
				StorageWindow->SetPositionInViewport(CurrentMousePos);
				//CanvasSlot->SetPosition(CurrentMousePos);
			}
			/*OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);*/

			//중복제거를 위해 widget을 Array에 담는다.
			InventoryWindowArray.Add(StorageWindow);

			UE_LOG(LogTemp, Warning, TEXT("NewInventory ::Binding CloseAdditional func"));
			StorageWindow->OnCloseWindow.AddDynamic(this,&UNewInventory::CloseAddiionalWidget);
			//가가방 가가가방을 위해 역시Binding을 해준다.
			StorageWindow->GridInventory->OpenAdditionalWidget.AddDynamic(this, &UNewInventory::BindingOpenWidgetFunc);
		}
	}
}

void UNewInventory::CloseAddiionalWidget(UDraggInventoryWindow* WindowWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("NewInventory ::CloseAdditional func called"));
	InventoryWindowArray.Remove(WindowWidget);
	WindowWidget->RemoveFromParent();
}

void UNewInventory::ChangeMainSwitchToStatus()
{
	MainInventorySwitcher->SetActiveWidgetIndex(0);
}

void UNewInventory::ChangeMainSwitchToInventory()
{
	MainInventorySwitcher->SetActiveWidgetIndex(1);
}