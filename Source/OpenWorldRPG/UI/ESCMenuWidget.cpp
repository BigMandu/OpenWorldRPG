// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/ESCMenuWidget.h"
#include "OpenWorldRPG/UI/Option/OptionWidget.h"

#include "OpenWorldRPG/MainController.h"
#include "Components/NamedSlot.h"
#include "Components/Button.h"

void UESCMenuWidget::NativeConstruct()
{
	if(!ResumeBtn || !OptionBtn || !ToTitleBtn) return;
	Super::NativeConstruct();

	/*ResumeBtn->OnClicked.AddDynamic(this, &UESCMenuWidget::ClickResumeBtn);
	OptionBtn->OnClicked.AddDynamic(this, &UESCMenuWidget::ClickOptionBtn);
	ToTitleBtn->OnClicked.AddDynamic(this, &UESCMenuWidget::ClickTitleBtn);*/

	ResumeBtn->OnClicked.AddUniqueDynamic(this, &UESCMenuWidget::ClickResumeBtn);
	OptionBtn->OnClicked.AddUniqueDynamic(this, &UESCMenuWidget::ClickOptionBtn);
	ToTitleBtn->OnClicked.AddUniqueDynamic(this, &UESCMenuWidget::ClickTitleBtn);
}

void UESCMenuWidget::ClickResumeBtn()
{
	if (AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController()))
	{
		UMyGameInstance* MyGameInst = GetWorld()->GetGameInstance<UMyGameInstance>();
		if (!MyGameInst) return;

		MyGameInst->InitVariableRelevantEscWidget();

		PlayerCon->SetInputAndFocus(false);

		RemoveFromParent();

		// if OptionWidget was created and showing, remove from viewport.
		if (OptionWidget.IsValid())
		{
			OptionWidget->RemoveFromViewport();
		}		
	}
}

void UESCMenuWidget::ClickOptionBtn()
{
	UMyGameInstance* MyGameInst = GetWorld()->GetGameInstance<UMyGameInstance>();
	if(!MyGameInst) return;

	//이미 있다면 추가 생성은 하지 않고 return한다.
	if(OptionWidget.IsValid()) return;

	OptionWidget = MyGameInst->CreateOptionWidget();

	if (OptionWidget.IsValid())
	{
		OptionWidget->OnRemoveWidget.AddUniqueDynamic(this, &UESCMenuWidget::RemoveOptionWidget);
		OptionWidget->AddToViewport(9);
		ShowOptionWidget();
	}
	

	//OptionSlotBorder->AddChild(OptionWidget);
	//OptionSlot->OnSlotAdded(OptionWidget);
	
}

void UESCMenuWidget::ClickTitleBtn()
{
	//open level
	FString TitleMapPath = TEXT("/Game/Maps_Normal/MenuMap.MenuMap");

	UMyGameInstance* Inst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (Inst)
	{
		Inst->PlayLoadingScreen();
	}
	UGameplayStatics::OpenLevel(GetWorld(), FName(*TitleMapPath));
}



void UESCMenuWidget::ShowOptionWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
void UESCMenuWidget::RemoveOptionWidget()
{
	OptionWidget.Reset();
	SetVisibility(ESlateVisibility::Visible);
}