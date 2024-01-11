// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"
#include "OpenWorldRPG/UI/Inventory/NewItemwidget.h"
#include "OpenWorldRPG/UI/Inventory/DropWidget.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"

#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/MainController.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/LineBatchComponent.h"

#include "GameFramework/HUD.h"
#include "Slate/SlateBrushAsset.h"
#include "Sound/SoundCue.h"
#include <Kismet/GameplayStatics.h>


UNewInventoryGrid::UNewInventoryGrid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USlateBrushAsset> BrushAsset(TEXT("/Game/UI/Inventory/SB_DropLocation.SB_DropLocation"));

	if (BrushAsset.Succeeded())
	{
		Brush = BrushAsset.Object;
		//UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::BrushAsset load success."));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::BrushAsset load failed."));
	}

}

void UNewInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;
	GridInit();
}

void UNewInventoryGrid::GridInit()
{
	if (StorageObj == nullptr) return;

	int32 tempRows = 0;
	int32 tempColumns = 0;
	if (StorageObj->Columns == 0 || StorageObj->Rows == 0)
	{
		//tempRows = Rows;
		//tempColumns = Columns;
	}
	else
	{
		tempRows = StorageObj->Rows;
		tempColumns = StorageObj->Columns;
	}



	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
	if (CanvasSlot)
	{
		FVector2D Size = FVector2D(tempColumns * StorageObj->TileSize, tempRows * StorageObj->TileSize);
		CanvasSlot->SetSize(Size);

		StorageObj->OnInventoryAdd.Clear(); //새로 추가함
		StorageObj->OnInventoryAdd.AddUFunction(this, FName("RefreshInventory"));

		CreateLineSegments(tempRows, tempColumns);
		RefreshInventory();
	}

}


int32 UNewInventoryGrid::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 bReturn = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FGeometry BorderGeo = GridBorder->GetCachedGeometry();
	FVector2D BorderLo = USlateBlueprintLibrary::GetLocalTopLeft(BorderGeo);
	/*
	* border의 Top-Left부터 그려나간다.
	*/
	for (FLine line : Lines)
	{
		FVector2D posA = FVector2D(line.Start + BorderLo);
		FVector2D posB = FVector2D(line.End + BorderLo);


		//DrawLine의 Tint = RGB all 0.5f, Alpha is 0.5f;
		FLinearColor LineColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, posA, posB, LineColor);
	}

	bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();

	if (bNeedDropLocation && bDragging)
	{
		DrawDropLocation(Context);
	}	

	return bReturn;
}

void UNewInventoryGrid::CreateLineSegments(int32 VarRows, int32 VarColumns)
{
	/* create vertical line */
	/* 0열부터 끝열까지 순차적으로 그리기 위해 라인을 만들어 넣어줌*/
	for (int32 iter = 0; iter <= VarColumns; iter++)
	{
		float LocalX = iter * StorageObj->TileSize;
		FLine Line;
		Line.Start = FVector2D(LocalX, 0.f);
		Line.End = FVector2D(LocalX, VarRows * StorageObj->TileSize);
		Lines.Add(Line); 

	}
	
	/* create horizontal line */
	for (int32 iter = 0; iter <= VarRows; iter++)
	{
		float LocalY = iter * StorageObj->TileSize;
		FLine Line;
		Line.Start = FVector2D(0.f, LocalY);
		Line.End = FVector2D(VarColumns * StorageObj->TileSize, LocalY);
		Lines.Add(Line);
	}
}


/* 이 함수는 Item이 ItemStorageObject에서 업데이트 될때 (추가될때, 삭제될때) 호출시켜줘야한다.
* 
* 파라미터로 넘긴 Obj는 ItemStorageObject에서 Removed할때 빼고는 쓰이지 않고 있다.
* Remove할 때 QuickSlot에 등록되어있던거면 Quickslot에서 지워주기 위함.
*/
void UNewInventoryGrid::RefreshInventory(UNewItemObject* Obj)
{
	/* 안쪽에 있는 CanvasPanel을 전부 지워준다.
	* Line은 Border에 있으니 선을 다시 그을필요는 없음.
	* 그리고 InventoryComponent에 있는 모든 item을 불러와서 해당하는 아이콘과 사이즈만큼 
	* 채워 넣는다.
	* InventoryComponent에 모든 Item을 얻어오는 함수를 새로 추가해준다.
	*/

	AMainController* TMainCon = Cast<AMainController>(GetOwningPlayer());
	MainCon = (TMainCon != nullptr) ? TMainCon : nullptr;
	
	if (GridCanvasPanel)// && MainCon != nullptr)
	{
		GridCanvasPanel->ClearChildren();

		//Obj파라미터를 사용하는 경우는 아래 경우밖에 없다.
		/* "Obj가 Use되어 Count가 0 이하가 되어 삭제되는 경우" 
		*/
		if (Obj && Obj->bIsPendingDelete && Obj->bIsRegQuickSlot && Obj->ItemInfo.Count <= 0)
		{
			//QuickSlot에 등록되어 있는지 확인하고 등록 되어 있는 경우에 QuickSlot에서 삭제 process를 진행한다.
			MainCon->MainHud->QuickSlot->CheckAlreadyRegistered(Obj,false);

		}

		const TMap<UNewItemObject*, FTile> ItemsMap = StorageObj->GetAllItems(); //InventoryComp->GetAllItems();
		 
		for (auto ele : ItemsMap)
		{
			if (ele.Key != nullptr) //ItemsMap에 null이 들어와버려서 한번 더 검증.
			{
				UNewItemwidget* Itemwidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);
				if (Itemwidget)
				{
					Itemwidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));
					//Itemwidget->OnDragDetect.AddUFunction(this, FName("PendingRemoveItem"));

					Itemwidget->Tilesize = StorageObj->TileSize; //제대로 넘겨줌. 40.f
					Itemwidget->ItemObj = ele.Key;
					Itemwidget->MotherContainer = this;
					//Itemwidget->Refresh(); //여기다 하니까 DDoper때 DefaultDrag가 안나옴.ItemWidget의 NativeConstruct에서 호출하는것으로 변경함.

					ele.Key->SetMotherStorage(StorageObj);

					UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(Itemwidget);


					/*추가시킨 ItemWidget의 size와 Position을 맞춰 주기 위한 작업을 진행한다.
					* Setsize를 사용하기 위해 CanvasPanelSlot으로 Cast를 해준다.
					* SetPosition도 사용한다.
					*/
					UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PanelSlot);
					if (CanvasSlot)
					{
						CanvasSlot->SetAutoSize(true);
						FVector2D ItemTopLeft = FVector2D(ele.Value.X * StorageObj->TileSize, ele.Value.Y * StorageObj->TileSize);
						CanvasSlot->SetPosition(ItemTopLeft);
					}


					/**
					 * QuickSlot에 등록되어 있다면
					 * 계속해서 Quickslot에 등록될 수 있는 여부를 알 수 있는 - QuickNum을 return하는 함수를 호출한뒤
					 * QuickSlot text를 세팅하거나 지워준다.
					 */
					if (ele.Key->bIsRegQuickSlot)
					{
						EQuickSlotNumber QuickNum = MainCon->MainHud->QuickSlot->JudgeCanbeInQuickSlotOrUpdate(ele.Key);
						if (QuickNum == EQuickSlotNumber::EQSN_MAX)
						{
							Itemwidget->RemoveQuickSlotNumber();
						}
						else
						{
							Itemwidget->SetQuickSlotNumber(QuickNum);
						}
						
					}
				}
			}
		}
	}
}


bool UNewInventoryGrid::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::OnDrop func called"));

	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	UNewInventoryComponent* BaseInvComp = Cast<UNewInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UNewInventoryComponent::StaticClass()));
	if (!DDOper || !BaseInvComp) return false;

	if (DDOper->ItemObj->bIsDragging)
	{
		FTile DraggedTile;
		DraggedTile.X = DraggedTopLeftTile.X;
		DraggedTile.Y = DraggedTopLeftTile.Y;

		int32 ind = StorageObj->TileToIndex(DraggedTile);

		if (BaseInvComp->IsAvailableSpace(StorageObj, DDOper->ItemObj, ind))
		{	
			//Drop시에 DropSound를 재생한다.
			if ( DDOper->ItemObj->ItemInfo.DataAsset->SlotDropSound )
			{
				UE_LOG(LogTemp,Warning,TEXT("NewInvGrid::OnDrop// Play UI sound"));
				UGameplayStatics::PlaySound2D(GetWorld(), DDOper->ItemObj->ItemInfo.DataAsset->SlotDropSound);
			}
			DDOper->ItemObj->bIsDragging = false;
			bNeedDropLocation = false;
			BaseInvComp->AddItemAtIndex(StorageObj, DDOper->ItemObj, ind);
		}
		else
		{
			//drop 불가.
		}

	}


	//OldVersion.
	/*
	UNewItemObject* ItemObj = Cast<UNewItemObject>(InOperation->Payload);
	if (ItemObj && ItemObj->bIsDragging)
	{

		ItemObj->bIsDragging = false;

		FTile DraggedTile;
		DraggedTile.X = DraggedTopLeftTile.X;
		DraggedTile.Y = DraggedTopLeftTile.Y;
		
		int32 index = InventoryComp->TileToIndex(DraggedTile);
		
		// Drop widget 상태 변환 
		if (Dropwidget && MainCon && !MainCon->bIsInteractLootBox)
		{
			//UE_LOG(LogTemp, Warning, TEXT("InvGrid:: Drop Widget Change 'Normal' State "));
			Dropwidget->bReturnNormal = true;
			Dropwidget->ChangeState();
		}

		// 공간이 된다면 해당 공간에 넣는다.
		if (InventoryComp->IsAvailableSpace(ItemObj, index))
		{
			InventoryComp->AddItemAtIndex(ItemObj, index);
			return true;
		}
		//공간이 되지 않는다면
		else
		{
			//ItemObj가 원래 있던 MotherContainer를 가져와
			if (ItemObj->GetMotherContainer() != nullptr)
			{
				//MotherContainer가 지금 이 Gridwidget과 다른거라면 
				if (ItemObj->GetMotherContainer() != this)
				{
					UCustomInventoryLibrary::BackToItem(ItemObj);
				}
				else
				{
					//MotherContainer가 지금 이 Gridwidget이라면
					if (InventoryComp->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex))
					{
						InventoryComp->AddItemAtIndex(ItemObj, ItemObj->TopLeftIndex);
					}
					else
					{
						InventoryComp->TryAddItem(ItemObj);
					}
					
				}
			}
			//Mothercontainer에 있지 않고, 장착중이었던 장비라면
			else if (ItemObj->GetMotherEquipSlot() != nullptr)
			{
				UCustomInventoryLibrary::BackToItem(ItemObj);
				//ItemObj->GetMotherEquipSlot()->TrySlotEquip(ItemObj);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UNewInvGrid::OnDrop , Error!"));
			}
		}
	}
	*/
	return bReturn;
}

bool UNewInventoryGrid::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	

	//UNewItemwidget* ItemWid = Cast<UNewItemwidget>(InOperation->Payload);
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);

	if (DDOper && DDOper->ItemObj)
	{
		int32 AddRight = 0;
		int32 AddDown = 0;

		// Returns The position of the cursor in screen space 
		const FVector2D MousePosInWindow = InDragDropEvent.GetScreenSpacePosition();

		// Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in.
		FVector2D MousePosInWidget = InGeometry.AbsoluteToLocal(MousePosInWindow);

		FVector2D MousePosInEachTile = GetMousePositionInEachTile(MousePosInWidget);

		FIntPoint Itemsize = DDOper->ItemObj->GetTempItemSize();

		// 해당 타일의 오른쪽으로 반이상, 아래로 반이상 내려갔는지 판단.
		if (MousePosInEachTile.X > StorageObj->TileSize / 2.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Right"));
			AddRight = 1;
		}

		if (MousePosInEachTile.Y > StorageObj->TileSize / 2.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Down"));
			AddDown = 1;
		}

		// 최소, 최대값 제한 
		int32 Reindex_X = FMath::Clamp<int32>(Itemsize.X - AddRight, 0, Itemsize.X - AddRight);
		int32 Reindex_Y = FMath::Clamp<int32>(Itemsize.Y - AddDown, 0, Itemsize.Y - AddDown);

		FIntPoint CenterPosition = FIntPoint(Reindex_X, Reindex_Y) / 2;
		FIntPoint GetPos = FIntPoint((MousePosInWidget / StorageObj->TileSize).X, (MousePosInWidget / StorageObj->TileSize).Y);

		DraggedTopLeftTile = GetPos - CenterPosition;
		//bReturn = true;
	}	
	//Old Version
	/*
	UNewItemObject* ItemObj = Cast<UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		int32 AddRight = 0;
		int32 AddDown = 0;
		// Returns The position of the cursor in screen space 
		const FVector2D MousePosInWindow = InDragDropEvent.GetScreenSpacePosition();

		// Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in.
		FVector2D MousePosInWidget = InGeometry.AbsoluteToLocal(MousePosInWindow); 

		FVector2D MousePosInEachTile = GetMousePositionInEachTile(MousePosInWidget);


		//UE_LOG(LogTemp, Warning, TEXT("========================================="));
		//UE_LOG(LogTemp, Warning, TEXT("MousePOSinWINDOW = %s"), *MousePosInWindow.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("MousePOSinWIDGET = %s"), *MousePosInWidget.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("MousePOSinTILE = %s"), *MousePosInEachTile.ToString());

		FIntPoint Itemsize = ItemObj->GetItemSize();
		//UE_LOG(LogTemp, Warning, TEXT("========================================="));
		//UE_LOG(LogTemp, Warning, TEXT("OnDragOver ItemSize : %s"), *ItemObj->GetItemSize().ToString());

		// 해당 타일의 오른쪽으로 반이상, 아래로 반이상 내려갔는지 판단.
		if (MousePosInEachTile.X > StorageObj->TileSize / 2.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Right"));
			AddRight = 1;
		}

		if (MousePosInEachTile.Y > StorageObj->TileSize / 2.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Down"));
			AddDown = 1;
		}

		// 최소, 최대값 제한 
		int32 Reindex_X = FMath::Clamp<int32>(Itemsize.X - AddRight, 0, Itemsize.X - AddRight);
		int32 Reindex_Y = FMath::Clamp<int32>(Itemsize.Y - AddDown, 0, Itemsize.Y - AddDown);

		FIntPoint CenterPosition = FIntPoint(Reindex_X, Reindex_Y) /2;
		FIntPoint GetPos = FIntPoint((MousePosInWidget / StorageObj->TileSize).X, (MousePosInWidget / StorageObj->TileSize).Y);

		DraggedTopLeftTile = GetPos - CenterPosition;	
		//UE_LOG(LogTemp, Warning, TEXT("CenterPosition = %s"), *CenterPosition.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("GetPos = %s"), *GetPos.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("DraggedTopLeftTile = %s"), *DraggedTopLeftTile.ToString());

		//bReturn = true;
	}
	*/
	return bReturn;
}

FVector2D UNewInventoryGrid::GetMousePositionInEachTile(FVector2D MousePos)
{
	FVector2D VectorToReturn;

	VectorToReturn = FVector2D(FGenericPlatformMath::Fmod(MousePos.X, StorageObj->TileSize), fmod(MousePos.Y, StorageObj->TileSize));

	return VectorToReturn;
}


void UNewInventoryGrid::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	SetFocus();
	bNeedDropLocation = true;
	//bCanDrop = true;
	//UE_LOG(LogTemp, Warning, TEXT("InvGrid bCanDrop = %s"), bCanDrop ? "true" : "false");
}

void UNewInventoryGrid::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	//remove focus
	if (HasAnyUserFocus())
	{
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		if (LocalPlayer)
		{
			FReply& DelayedSlateOperations = LocalPlayer->GetSlateOperations();
			DelayedSlateOperations.ClearUserFocus();
			UE_LOG(LogTemp, Warning, TEXT("NewItemWidget:: DragDetected / Remove Focus Name : %s"), *GetFName().ToString());
		}
	}
	bNeedDropLocation = false;
	//bCanDrop = false;
	//UE_LOG(LogTemp, Warning, TEXT("InvGrid bCanDrop = %s"), bCanDrop ? "true" : "false");
}

void UNewInventoryGrid::DrawDropLocation(FPaintContext& Context) const
{
	//UNewItemObject* Obj = Cast< UNewItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::GetDragDroppingContent());
	if (!DDOper) return;
	
	if (DDOper->ItemObj)
	{
		FTile DraggedTile;
		DraggedTile.X = DraggedTopLeftTile.X;
		DraggedTile.Y = DraggedTopLeftTile.Y;

		int32 index = StorageObj->TileToIndex(DraggedTile);

		bool bEmptyAtIndex = StorageObj->IsAvailableSpace(DDOper->ItemObj, index);

		

		FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
		FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
		FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);

		FVector2D DropLocationPos = FVector2D(DraggedTile.X, DraggedTile.Y);
		FVector2D DropLocationSize = FVector2D(DDOper->ItemObj->GetTempItemSize().X * StorageObj->TileSize, DDOper->ItemObj->GetTempItemSize().Y * StorageObj->TileSize);
		
		if (Brush)
		{
			//UE_LOG(LogTemp, Warning, TEXT("========================================="));
			//UE_LOG(LogTemp, Warning, TEXT("DrawDropLo ItemSize : %s"), *Obj->GetItemSize().ToString());
			//UE_LOG(LogTemp, Warning, TEXT("DropLo Pos : %s"), *DropLocationPos.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("DropLo Size : %s"), *DropLocationSize.ToString());

			//비어있다면 green
			if (bEmptyAtIndex)
			{
				UWidgetBlueprintLibrary::DrawBox(Context, DropLocationPos * StorageObj->TileSize, DropLocationSize, Brush, Green);
				//DrawBox
			}
			//비어있지 않다면 red
			else
			{
				UWidgetBlueprintLibrary::DrawBox(Context, DropLocationPos * StorageObj->TileSize, DropLocationSize, Brush, Red);
			}
		}
		
		
		//USlateBrushAsset BrushAsset = USlateBrushAsset();
		//USlateBrushAsset Brush = USlateBrushAsset::
		//FSlateBrush Brush = FSlateBrush();
		//brushAsset.Brush = Brush;
		//UWidgetBlueprintLibrary::MakeBrushFromTexture(UTexture2D::make)
		
	}
}

UNewInventoryComponent* UNewInventoryGrid::GetInventoryComp()
{
	if (InventoryComp)
	{
		return InventoryComp;
	}
	return nullptr;
}

void UNewInventoryGrid::MoveItemToItem(UNewItemObject* DroppedItemObj, UNewItemObject* OnItemObj)
{
	UNewInventoryComponent* BaseInvComp = Cast<UNewInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UNewInventoryComponent::StaticClass()));
	if(BaseInvComp == nullptr) return;

	BaseInvComp->AddItemCount(DroppedItemObj,OnItemObj);
}

// NewItemwidget으로 옮김

FReply UNewInventoryGrid::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::OnPreviewKeyDown"));
	//FReply Reply = Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);

	//bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();
	//if (bDragging)
	{
		UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::GetDragDroppingContent());
		if (DDOper == nullptr) return FReply::Unhandled();

		UNewItemObject* Obj = DDOper->ItemObj;
		if (Obj && Obj->ItemInfo.DataAsset->bCanRotate)
		{
			if (InKeyEvent.GetKey() == EKeys::R)
			{
				UNewItemwidget* Itemwidget = Cast<UNewItemwidget>(DDOper->DefaultDragVisual);// DragWidget;// Cast<UNewItemwidget>(CusDDOper->DefaultDragVisual);
				if (Itemwidget)
				{
					Obj->ItemRotate();
					Itemwidget->Refresh(true);
					FReply::Handled();
				}
			}
		}
	}
	return FReply::Unhandled();
}






///////////////////////////////////////////////////////////////////////////////////
/*                                이하는 쓰지 않는 함수                            */
///////////////////////////////////////////////////////////////////////////////////

void UNewInventoryGrid::BindDropWidget(UDropWidget* T_Dropwidget)
{
	Dropwidget = T_Dropwidget;
}



void UNewInventoryGrid::OnItemRemove(UObject* T_ItemObj)
{
	if (T_ItemObj)
	{
		UNewItemObject* ItemObj = Cast<UNewItemObject>(T_ItemObj);
		if (ItemObj && InventoryComp)
		{
			//if (ItemObj->GetMotherContainer() != nullptr)
			{
				/*
				* NewItemWidget의 delegate에 bind된 함수.
				* 이 함수가 호출되면서,
				* 해당 InventoryGrid에 연결된 InventoryComponent의 특정 함수를 호출한다.
				*/
				bool bRemoveResult = InventoryComp->RemoveItem(ItemObj);

				/* Drop widget 상태 변환 */
				if (Dropwidget && !MainCon->bIsInteractLootBox)
				{
					//UE_LOG(LogTemp, Warning, TEXT("InvGrid:: Drop Widget Change 'Drop' State "));
					Dropwidget->ChangeState(true);
				}
			}
		}
	}
}

bool UNewInventoryGrid::MoveItemInSameContainer(UNewItemObject* Item)
{
	//UE_LOG(LogTemp, Warning, TEXT("UNewInventoryGrid::MoveItemInSameContainer() is called"));
	FTile DraggedTile;
	DraggedTile.X = DraggedTopLeftTile.X;
	DraggedTile.Y = DraggedTopLeftTile.Y;

	int32 index = StorageObj->TileToIndex(DraggedTile);

	if (InventoryComp->IsAvailableSpace(Item, index))
	{
		//InventoryComp->RemoveItem(Item);
		InventoryComp->AddItemAtIndex(Item, index);



		return true;
	}
	else
	{
		/* 공간이 없으면 기존 저장된 Top-Left Index에 넣는다.*/
		if (InventoryComp->IsAvailableSpace(Item, Item->TopLeftIndex))
		{
			InventoryComp->AddItemAtIndex(Item, Item->TopLeftIndex);
		}
		else
		{
			/* 뭔가 꼬였다면 그냥 Item을 넣는다.*/
			InventoryComp->TryAddItem(Item);
		}
	}
	return false;

}


//GridInit으로 변경함. Old version.
void UNewInventoryGrid::GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize)
{
	//UE_LOG(LogTemp, Warning, TEXT("InvGrid:: called GridInit func"));
	check(p_InvComp);

	InventoryComp = p_InvComp;
	/*TileSize = p_TileSize;
	Rows = InventoryComp->Rows;
	Columns = InventoryComp->Columns;*/


	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
	if (CanvasSlot)
	{
		//FVector2D Size = FVector2D(Columns * TileSize, Rows * TileSize);
		FVector2D Size = FVector2D(StorageObj->Columns * StorageObj->TileSize, StorageObj->Rows * StorageObj->TileSize);
		CanvasSlot->SetSize(Size);


		//InventoryComp의 AddItem함수가 성공할때마다 broadcast,해당 함수를 실행시키기 위해 bind
		if (InventoryComp)
		{
			InventoryComp->OnInventoryUpdated.Clear();
			InventoryComp->OnInventoryUpdated.AddUFunction(this, FName("RefreshInventory"));
		}

		CreateLineSegments(StorageObj->Rows, StorageObj->Columns);
		RefreshInventory();

	}

}
