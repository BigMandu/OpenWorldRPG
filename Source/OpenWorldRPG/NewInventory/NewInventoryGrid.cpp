// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/LineBatchComponent.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"


UNewInventoryGrid::UNewInventoryGrid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNewInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UNewInventoryGrid::GridInitialize(UNewInventoryComponent* p_InvComp, float p_TileSize)
{
	InventoryComp = p_InvComp;
	TileSize = p_TileSize;
	
	

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
	if (CanvasSlot)
	{
		FVector2D Size = FVector2D(InventoryComp->Columns * TileSize, InventoryComp->Rows * TileSize);
		CanvasSlot->SetSize(Size);
	
		
		//InventoryComp의 AddItem함수가 성공할때마다 broadcast,해당 함수를 실행시키기 위해 bind
		if (InventoryComp)
		{
			FDelegateHandle DelHandle = InventoryComp->OnInventoryUpdated.AddUFunction(this, FName("RefreshInventory"));
		}

		CreateLineSegments();
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
	return bReturn;
}

void UNewInventoryGrid::CreateLineSegments()
{
	/* create vertical line */
	/* 0열부터 끝열까지 순차적으로 그리기 위해 라인을 만들어 넣어줌*/
	for (int32 iter = 0; iter <= InventoryComp->Columns; iter++)
	{
		float LocalX = iter * TileSize;
		FLine Line;
		Line.Start = FVector2D(LocalX, 0.f);
		Line.End = FVector2D(LocalX, InventoryComp->Rows * TileSize);
		Lines.Add(Line); 
		
	}
	
	/* create horizontal line */
	for (int32 iter = 0; iter <= InventoryComp->Rows; iter++)
	{
		float LocalY = iter * TileSize;
		FLine Line;
		Line.Start = FVector2D(0.f, LocalY);
		Line.End = FVector2D(InventoryComp->Columns * TileSize, LocalY);
		Lines.Add(Line);
	}
}

/* 이 함수는 Item이 Inventory에서 업데이트 될때 (추가될때, 삭제될때) 호출시켜줘야한다.*/
void UNewInventoryGrid::RefreshInventory()
{
	/* 안쪽에 있는 CanvasPanel을 전부 지워준다.
	* Line은 Border에 있으니 선을 다시 그을필요는 없음.
	* 그리고 InventoryComponent에 있는 모든 item을 불러와서 해당하는 아이콘과 사이즈만큼 
	* 채워 넣는다.
	* InventoryComponent에 모든 Item을 얻어오는 함수를 새로 추가해준다.
	*/
	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain != nullptr) ? TMain : nullptr;
	if (Main)
	{
		MainCon = Main->MainController;
	}
	
	if (GridCanvasPanel && MainCon != nullptr)
	{
		//TMap<UNewItemObject*, FTile> ItemsMap;
		GridCanvasPanel->ClearChildren();
		auto ItemsMap = InventoryComp->GetAllItems();

		for (auto ele : ItemsMap)
		{
			UNewItemwidget* ItemWidget = Cast<UNewItemwidget>(CreateWidget<UUserWidget>(this, MainCon->WItemwidget));
			if (ItemWidget)
			{
				ItemWidget->Tilesize = TileSize;
				ItemWidget->ItemObj = ele.Key;

				//Item이 삭제됐을때 실행될 function을 생성한뒤, NewItemWidget의 Delegate에 연결한다.
				// func의 생성은 delegate의 파라미터에 맞춤.
				ItemWidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));// &UNewInventoryGrid::OnItemRemove);
				
				ItemWidget->Refresh();

				//canvas panel의 자식으로 ItemWidget을 추가시킨다.
				UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(ItemWidget);


				/*추가시킨 ItemWidget의 size와 Position을 맞춰 주기 위한 작업을 진행한다.
				* Setsize를 사용하기 위해 CanvasPanelSlot으로 Cast를 해준다.
				* SetPosition도 사용한다.
				*/
				UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PanelSlot);
				if (CanvasSlot)
				{
					CanvasSlot->SetAutoSize(true);
					FVector2D ItemTopLeft = FVector2D(ele.Value.X * TileSize, ele.Value.Y * TileSize);
					CanvasSlot->SetPosition(ItemTopLeft);
				}


				
			}
		}
	}
}

void UNewInventoryGrid::OnItemRemove(UObject* T_ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("Call OnRemoveFunc"));
	UNewItemObject* ItemObj = Cast<UNewItemObject>(T_ItemObj);
	if (ItemObj && InventoryComp)
	{
		/* 
		* NewItemWidget의 delegate에 bind된 함수.
		* 이 함수가 호출되면서,
		* 해당 InventoryGrid에 연결된 InventoryComponent의 특정 함수를 호출한다.
		*/
		bool bRemoveResult = InventoryComp->RemoveItem(ItemObj);
	}
}

