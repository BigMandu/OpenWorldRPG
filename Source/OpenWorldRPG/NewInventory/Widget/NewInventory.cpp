// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipStorageWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/DropWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalDaDWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/DraggInventoryWindow.h"
#include "OpenWorldRPG/NewInventory/Widget/WeaponPartsWidget.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

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

		EquipmentStorageWidget->InitializeInventory(Main);
		EquipmentStorageWidget->EquipInitialize(EquipComp);		
		EquipmentStorageWidget->MainWidget = this;
		
		EquipmentStorageWidget->PocketWidget->OpenAdditionalWidget.AddDynamic(this, &UNewInventory::BindingOpenWidgetFunc);
		EquipmentStorageWidget->SecureBoxWidget->OpenAdditionalWidget.AddDynamic(this, &UNewInventory::BindingOpenWidgetFunc);

		EquipmentWidget->OpenAdditionalWidget_Equip.AddDynamic(this, &UNewInventory::BindingOpenWidgetFunc);
	}

	StatusButton->OnClicked.AddDynamic(this, &UNewInventory::ChangeMainSwitchToStatus);
	InventoryButton->OnClicked.AddDynamic(this, &UNewInventory::ChangeMainSwitchToInventory);
	CraftButton->OnClicked.AddDynamic(this, &UNewInventory::ChangeMainSwitchToCraft);
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
				RightWidgetSwitcher->SetActiveWidgetIndex(0);
			}
			else
			{
				SetRightWidget(nullptr);
				RightWidgetSwitcher->SetActiveWidgetIndex(0);
			}
		}
	}
}

bool UNewInventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//UE_LOG(LogTemp,Warning,TEXT("MAIN WIDGET (NEWINVENTORY WBP) :: Native On drop Event."));
	
	//	UAdditionalDaDWidget* AdditionalDaD = Cast<UAdditionalDaDWidget>(InOperation);
	//if (AdditionalDaD)
	{
		/*FVector2D DragWidgetOffset = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
		FVector2D DragWidgetLocation = DragWidgetOffset - AdditionalDaD->WidgetDragLocation;

		AdditionalDaD->AdditionalWidget->AddToViewport();
		AdditionalDaD->AdditionalWidget->SetVisibility(ESlateVisibility::Visible);
		AdditionalDaD->AdditionalWidget->SetPositionInViewport(DragWidgetLocation, false);*/



		//Canvas에 추가하면, DraggInventoryWindow에서 RemoveFromParent함수가 먹지 않아, AddToViewport로 대체했다.
		// 
		//UDraggInventoryWindow* StorageWindow = Cast<UDraggInventoryWindow>(AdditionalDaD->AdditionalWidget);
		//UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(StorageWindow);
		//StorageWindow->SetVisibility(ESlateVisibility::Visible);
		//CanvasSlot->SetPosition(DragWidgetLocation);

		
	}

	return bReturn;
}

void UNewInventory::BindingOpenWidgetFunc(UNewItemObject* ItemObj)
{	
	if (ItemObj == nullptr) return;
	
	UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
	if (CPDA == nullptr) return;
	if (CPDA->bHasWidget == false) return;
	
	CreateAdditionalWidget(ItemObj);
}


void UNewInventory::CreateAdditionalWidget(UNewItemObject* T_Obj)
{
	//이미 켜있는 Widget인지 확인한다.
	if (CheckAlreadyOpened(T_Obj))
	{
		return;
	}
	UDraggInventoryWindow* AdditionalWindow = CreateWidget<UDraggInventoryWindow>(this, WStorageWindow);
		
	UWidget* ChildWidget = CreateChildWidget(AdditionalWindow,T_Obj);

	if(ChildWidget == nullptr) return;
	
	
	AdditionalWindow->ContentBorder->AddChild(ChildWidget);
	
	
	UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(AdditionalWindow);

	//OpenedWindowArray.Num()
	CanvasSlot->SetZOrder(++HighestZ);

	UE_LOG(LogTemp, Warning, TEXT("dragging inv window parent : %s"), *GetParent()->GetFName().ToString());

	float MouseX;
	float MouseY;
	if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
	{
		
		FVector2D CurrentMousePos = FVector2D(MouseX, MouseY);
		CanvasSlot->SetPosition(CurrentMousePos);
		//AdditionalWindow ->SetPositionInViewport(CurrentMousePos);
	}
			

	//중복제거를 위해 widget을 Array에 담는다.
	OpenedWindowArray.Add(AdditionalWindow);
	

	UE_LOG(LogTemp, Warning, TEXT("NewInventory ::Binding CloseAdditional func"));
	AdditionalWindow->OnCloseWindow.AddDynamic(this, &UNewInventory::CloseAddiionalWidget);
	AdditionalWindow->OnButtonDownWindow.AddDynamic(this, &UNewInventory::MoveZOrderTop);

	AdditionalWindow->OnDraggingWindow.AddDynamic(this, &UNewInventory::DraggingWindow);
	//AdditionalWindow ->OnReleaseWindow.AddDynamic(this, &UNewInventory::ReleaseWindow);

}


bool UNewInventory::CheckAlreadyOpened(UNewItemObject* T_Obj)
{
	UItemStorageObject* StorageObj = Cast<UItemStorageObject>(T_Obj);

		//이미 켜있는 Widget인지 확인한다.
		/* Array 분할을 생각해 봤지만, ZOrder 문제로 인해 컨테이너를 합쳤다.
		*/
		for (UDraggInventoryWindow* OpenedWindow : OpenedWindowArray)
		{
			if(OpenedWindow)
			{
				TArray<UWidget*> ChildWidget = OpenedWindow->ContentBorder->GetAllChildren();
				for(auto Child : ChildWidget)
				{
					//StorageType인 경우
					if (StorageObj)
					{
						UNewInventoryGrid* Gridwidget = Cast< UNewInventoryGrid>(Child);
						if (Gridwidget)
						{
							if (Gridwidget->StorageObj == StorageObj)
							{
								return true;
							}
							continue;
						}
					}
					// 그외 나머지 Type
					else
					{
						if (UWeaponPartsWidget* PartsWidget = Cast<UWeaponPartsWidget>(Child))
						{
							if (PartsWidget->WeaponObj == T_Obj)
							{
								return true;
							}
							continue;
						}
					}
					
				}

			}
		}
	return false;
}

UWidget* UNewInventory::CreateChildWidget(UDraggInventoryWindow* AdditionalWindow, UNewItemObject* Obj)
{
	UWidget* ReturnSubWidget = nullptr;
	if (AdditionalWindow == nullptr) return ReturnSubWidget;
	UItemStorageObject* StorageObj = Cast<UItemStorageObject>(Obj);
	
	if (StorageObj)
	{
		//초기화가 된적 없는 Storage면 초기화를 해준다.
		if (StorageObj->Inventory.IsValidIndex(0) == false)
		{
			UCustomPDA* CPDA = Cast<UCustomPDA>(StorageObj->ItemInfo.DataAsset);
			StorageObj->InitializeStorage(CPDA->StorageX, CPDA->StorageY, CPDA->TileSize);
		}

		UNewInventoryGrid* GridWidget = CreateWidget<UNewInventoryGrid>(this, AdditionalWindow->WGridWidget);
		if (GridWidget == nullptr) return ReturnSubWidget;

		GridWidget->StorageObj = StorageObj;
		GridWidget->GridInit();
		ReturnSubWidget = GridWidget;

		//가가가방을 위해 Bind해준다.
		GridWidget->OpenAdditionalWidget.AddDynamic(this, &UNewInventory::BindingOpenWidgetFunc);
	}
	else
	{
		UWeaponPartsWidget* PartsWidget = CreateWidget<UWeaponPartsWidget>(this, AdditionalWindow->WPartsWidget);
		if (PartsWidget == nullptr) return ReturnSubWidget;

		
		PartsWidget->Initialize();
		PartsWidget->WidgetInit(Obj);
		PartsWidget->RefreshWidget();
		ReturnSubWidget = PartsWidget;
		//PartsWidget->AddToViewport();
		
	}

	return ReturnSubWidget;
}

void UNewInventory::CloseAddiionalWidget(UDraggInventoryWindow* WindowWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("NewInventory ::CloseAdditional func called"));
	OpenedWindowArray.Remove(WindowWidget);
	WindowWidget->RemoveFromParent();
	//WindowWidget->BeginDestroy();
	
}


void UNewInventory::MoveZOrderTop(UDraggInventoryWindow* Window)
{
	UE_LOG(LogTemp, Warning, TEXT("NewInventory ::MoveZOrder To Top function called"));

	
	Cast<UCanvasPanelSlot>(Window->Slot)->SetZOrder(++HighestZ);
}


void UNewInventory::DraggingWindow(UDraggInventoryWindow* DraggingWindow, const FPointerEvent InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NewInventory ::Dragging Window func called"));
	if(DraggingWindow == nullptr) return;
	bCanMoveWindow = true;
	CurrentHoldingWidget = DraggingWindow;

	LastWindowPos = Cast<UCanvasPanelSlot>(DraggingWindow->Slot)->GetPosition();
	LastMousePos = GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetLastScreenSpacePosition());
	//FVector2D MouseScreenPos = GetParent()->GetTickSpaceGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	//Cast<UCanvasPanelSlot>(DraggingWindow->Slot)->SetPosition(MouseScreenPos);

}
//
//void UNewInventory::ReleaseWindow()
//{
//	bCanMoveWindow = false;
//	CurrentHoldingWidget = nullptr;
//}


FReply UNewInventory::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry,InMouseEvent);

	FReply ReturnReply = FReply::Unhandled();
	if (bCanMoveWindow && CurrentHoldingWidget.IsValid())
	{
		auto PanelSlot = Cast<UCanvasPanelSlot>(CurrentHoldingWidget->Slot);
		if (IsValid(PanelSlot))
		{
			FVector2D CurrentMousePoint = GetTickSpaceGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			//FVector2D LastMousePoint = InGeometry.AbsoluteToLocal(InMouseEvent.LastScreenSpacePosition());

			FVector2D Offset = LastWindowPos - LastMousePos;

			PanelSlot->SetPosition(CurrentMousePoint+Offset);

			ReturnReply = FReply::Handled();
		}
		
	}
	return ReturnReply;
}

FReply UNewInventory::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply ReturnReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	ReturnReply = UWidgetBlueprintLibrary::ReleaseMouseCapture(ReturnReply);

	bCanMoveWindow = false;
	CurrentHoldingWidget = nullptr;

	return ReturnReply.NativeReply;
}

FReply UNewInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply ReturnReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	ReturnReply = UWidgetBlueprintLibrary::CaptureMouse(ReturnReply, this);
	return ReturnReply.NativeReply;
}

/** Inventory 창을 닫을때 열려있는 모든 AdditionalWidget을 닫는다.
  * @ArrayNum - Tabkey Down에서 넘겨준다. (OpendedWindowArray.Num() 값임)
  */
void UNewInventory::CloseAllAdditionalWidget(int32 ArrayNum)
{
	//for (int32 ind = ArrayNum-1; ind >= 0; --ind)
	//{
	//	UDraggInventoryWindow* AddedWindow = OpenedWindowArray[ind];
	//	if(AddedWindow != nullptr)
	//	{
	//		
	//		OpenedWindowArray.Remove(AddedWindow);
	//		AddedWindow->RemoveFromParent();
	//	}
	//}

	//other version
	//window가 nullptr이 아니면 삭제한다.
	OpenedWindowArray.RemoveAll([](UDraggInventoryWindow* AddedWindow)
		{
			AddedWindow->RemoveFromParent();
			return AddedWindow != nullptr;
		}
	);

}

void UNewInventory::ChangeMainSwitchToStatus()
{
	MainInventorySwitcher->SetActiveWidgetIndex(0);
	if (ResourceStatusWidget)
	{
		ResourceStatusWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UNewInventory::ChangeMainSwitchToInventory()
{
	MainInventorySwitcher->SetActiveWidgetIndex(1);
	RightWidgetSwitcher->SetActiveWidgetIndex(0);
	if (ResourceStatusWidget)
	{
		ResourceStatusWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
void UNewInventory::ChangeMainSwitchToCraft()
{
	MainInventorySwitcher->SetActiveWidgetIndex(1);
	RightWidgetSwitcher->SetActiveWidgetIndex(1);
	if (ResourceStatusWidget)
	{
		ResourceStatusWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}