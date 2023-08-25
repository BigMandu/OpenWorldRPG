// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Intel/IntelAcquiredListWidget.h"
#include "OpenWorldRPG/UI/Intel/IntelListEntryWidget.h"
#include "OpenWorldRPG/UI/Intel/IntelListItemObject.h"

#include "OpenWorldRPG/UI/Intel/IntelPageBaseWidget.h"
#include "OpenWorldRPG/UI/Intel/IntelPageOneWidget.h"
#include "OpenWorldRPG/UI/Intel/IntelPageOtherWidget.h"

#include "OpenWorldRPG/GameData/MyGameInstance.h"

#include "Components/ListView.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "Blueprint/UserWidget.h"

void UIntelAcquiredListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UMyGameInstance* MyGameInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if ( MyGameInst )
	{
		if ( MyGameInst->IntelDataTable )
		{
			IntelDataTable = MyGameInst->IntelDataTable;
			SetListView();
		}

	}
	CreateBasePageWidget();
}

//List view Setting.
void UIntelAcquiredListWidget::SetListView()
{
	if(IntelDataTable.IsValid() == false) return;

	for (auto Tablerow : IntelDataTable->GetRowMap())
	{
		FIntelligenceDataTable* RowData = (FIntelligenceDataTable*)Tablerow.Value;

		UIntelListItemObject* ListObj = NewObject<UIntelListItemObject>(this);
		ListObj->IntelName = RowData->Subject;
		ListObj->IntelType = RowData->IntelType;

		//AddItem함수를 호출하면, EntryWidget의 NativeOnListItemObjectSet 함수가 호출된다.
		IntelList->AddItem(ListObj);
	}
}
//Basepage widget create.
void UIntelAcquiredListWidget::CreateBasePageWidget()
{
	if ( W_IntelPageBaseWidget && ( WidgetIntelPageBase == nullptr ) )
	{
		if ( UIntelPageBaseWidget* Createdbasewidget = CreateWidget<UIntelPageBaseWidget>(this, W_IntelPageBaseWidget, FName("IntelBasePageWidget")))
		{
			WidgetIntelPageBase = Createdbasewidget;
			WidgetIntelPageBase->ListWidget = this;

			WidgetIntelPageBase->ExitButton->OnClicked.AddDynamic(this, &UIntelAcquiredListWidget::ExitIntelPage);
			WidgetIntelPageBase->NextButton->OnClicked.AddDynamic(this, &UIntelAcquiredListWidget::ToNextIntelPage);
			WidgetIntelPageBase->PrevButton->OnClicked.AddDynamic(this, &UIntelAcquiredListWidget::ToPrevIntelPage);
		}
		
	}
}


//this func called from UIntelListEntryWidget click func.
void UIntelAcquiredListWidget::ReadIntel(EIntelType ClickIntelType)
{
	if(IntelDataTable.IsValid() == false) return;

	FString TableName(IntelDataTable->GetName());

	//enum value to string type.

	FString RowString = UEnum::GetValueAsString(ClickIntelType);
	UE_LOG(LogTemp, Warning, TEXT("Enum row name : %s"), *RowString);
	RowString.RemoveFromStart(FString("EIntelType::"));
	UE_LOG(LogTemp, Warning, TEXT("Enum row name : %s"), *RowString);

	FName RowName = FName(*RowString);

	FIntelligenceDataTable* GetRowData = IntelDataTable->FindRow<FIntelligenceDataTable>(RowName,TableName);
	if ( GetRowData )
	{
		if ( WidgetIntelPageBase == nullptr )
		{
			CreateBasePageWidget();
		}
		WidgetIntelPageBase->AddToViewport();
		WidgetIntelPageBase->SetVisibility(ESlateVisibility::Visible);
		WidgetIntelPageBase->NextButton->SetVisibility(ESlateVisibility::Visible);
		WidgetIntelPageBase->PrevButton->SetVisibility(ESlateVisibility::Visible);

		//키보드 이벤트를 위해 focus한다.
		WidgetIntelPageBase->SettingFocus();

		//DataIntoBasePage(*GetRowData);
		SettingIntelPage(*GetRowData);
	}
}

void UIntelAcquiredListWidget::SettingIntelPage(const FIntelligenceDataTable& RowData)
{
	PageWidgetsMap.Empty();
	/*for ( TMap<int32, UUserWidget*>::TIterator IterRemove = PageWidgetsMap.CreateIterator(); IterRemove; ++IterRemove )
	{
		IterRemove.RemoveCurrent();
	}*/

	int32 loopMAX = RowData.PageCount;
	for ( int32 loop = 0; loop < loopMAX; ++loop )
	{
		if ( loop == 0 && W_IntelPageOneWidget)
		{
			if(UIntelPageOneWidget* WidgetIntelPageOne = CreateWidget<UIntelPageOneWidget>(this, W_IntelPageOneWidget))
			{
				DataIntoWidget(WidgetIntelPageOne, RowData, loop);
				//Overlay 패널슬롯을 가져와서 childwidget의 align을 세팅해준다.
				if (UPanelSlot* PSlot = WidgetIntelPageBase->PaperOverlay->AddChild(WidgetIntelPageOne) )
				{
					if ( UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PSlot) )
					{
						OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
						OverlaySlot->SetVerticalAlignment(VAlign_Fill);
					}
				}
				//중요.꼭 AdjustWidgetLocation함수는 여기에서 호출한다.
				WidgetIntelPageOne->AdjustWidgetLocation();
				WidgetIntelPageOne->SetVisibility(ESlateVisibility::HitTestInvisible);

				CurrentViewPageNumber = 0;
				PageWidgetsMap.Add(loop, WidgetIntelPageOne);
			}			
		}
		else
		{
			if ( W_IntelPageOtherWidget )
			{
				if(UIntelPageOtherWidget* OtherPage = CreateWidget<UIntelPageOtherWidget>(this, W_IntelPageOtherWidget))
				{
					DataIntoWidget(OtherPage,RowData, loop);
					if ( UPanelSlot* Pslot = WidgetIntelPageBase->PaperOverlay->AddChild(OtherPage) )
					{
						if ( UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Pslot) )
						{
							OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
							OverlaySlot->SetVerticalAlignment(VAlign_Fill);
						}
					}
					OtherPage->AdjustWidgetLocation();
					OtherPage->SetVisibility(ESlateVisibility::Collapsed);
				
					PageWidgetsMap.Add(loop, OtherPage);
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Row Data : %s"), *RowData.Date.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Row Data : %s"), *RowData.To.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Row Data : %s"), *RowData.From.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Row Data : %s"), *RowData.Subject.ToString());
}

void UIntelAcquiredListWidget::DataIntoWidget(UUserWidget* Widget, const FIntelligenceDataTable& RowData, const int32& pageindex)
{
	if ( UIntelPageOneWidget* FirstPage = Cast<UIntelPageOneWidget>(Widget) )
	{
		FirstPage->SettingPage(RowData,pageindex);
		//FirstPage->AdjustWidgetLocation();
	}
	else if ( UIntelPageOtherWidget* OtherPage = Cast<UIntelPageOtherWidget>(Widget) )
	{
		OtherPage->Classification_TOP->SetText(RowData.ClassificationStamp);
		OtherPage->Classification_BOT->SetText(RowData.ClassificationStamp);
		OtherPage->Additional_TOP->SetText(RowData.AdditionalStamp);
		OtherPage->Additional_BOT->SetText(RowData.AdditionalStamp);		

		if ( RowData.Context.IsValidIndex(pageindex))
		{
			OtherPage->ContentText->SetText(RowData.Context[pageindex]);
		}
		else
		{
			OtherPage->ContentText->SetText(FText::FromString(""));
		}
	}
}

void UIntelAcquiredListWidget::ExitIntelPage()
{
	WidgetIntelPageBase->PaperOverlay->ClearChildren();
	WidgetIntelPageBase->SetVisibility(ESlateVisibility::Collapsed);
	WidgetIntelPageBase->RemoveFromViewport();
}

//다음 페이지 버튼을 누름
void UIntelAcquiredListWidget::ToNextIntelPage()
{
	int32 LastPage = PageWidgetsMap.Num()-1;

	if ( CurrentViewPageNumber + 1 <= LastPage )
	{
		WidgetIntelPageBase->NextButton->SetVisibility(ESlateVisibility::Visible);
		WidgetIntelPageBase->PrevButton->SetVisibility(ESlateVisibility::Visible);

		//현재 페이지를 보이지 않게 한다.
		UUserWidget* currentPage = PageWidgetsMap.FindRef(CurrentViewPageNumber);
		currentPage->SetVisibility(ESlateVisibility::Collapsed);


		//다음 페이지가 마지막 페이지인 경우, NextButton을 보이지 않게 한다.
		if ( CurrentViewPageNumber + 1 == LastPage )
		{
			WidgetIntelPageBase->NextButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		CurrentViewPageNumber += 1;
		UUserWidget* nextpage = PageWidgetsMap.FindRef(CurrentViewPageNumber);
		nextpage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

//이전 페이지 버튼을 누름
void UIntelAcquiredListWidget::ToPrevIntelPage()
{
	if ( CurrentViewPageNumber - 1 >= 0)
	{
		WidgetIntelPageBase->NextButton->SetVisibility(ESlateVisibility::Visible);
		WidgetIntelPageBase->PrevButton->SetVisibility(ESlateVisibility::Visible);		

		//현재 페이지를 보이지 않게 한다.
		UUserWidget* currentPage = PageWidgetsMap.FindRef(CurrentViewPageNumber);
		currentPage->SetVisibility(ESlateVisibility::Collapsed);


		//다음 페이지가 마지막 페이지인 경우, NextButton을 보이지 않게 한다.
		if ( CurrentViewPageNumber - 1 == 0 )
		{
			WidgetIntelPageBase->PrevButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		CurrentViewPageNumber -= 1;
		UUserWidget* prevpage = PageWidgetsMap.FindRef(CurrentViewPageNumber);
		prevpage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}