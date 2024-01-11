// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Option/OptionWidget.h"

void UOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VideoBtn->CommonBtn->OnClicked.AddDynamic(this,&UOptionWidget::ClickVideoButton);
	AudioBtn->CommonBtn->OnClicked.AddDynamic(this,&UOptionWidget::ClickAudioButton);
	KeyBindBtn->CommonBtn->OnClicked.AddDynamic(this,&UOptionWidget::ClickKeyBindButton);
	ApplyBtn->CommonBtn->OnClicked.AddDynamic(this,&UOptionWidget::ClickApplyButton);
	CloseBtn->CommonBtn->OnClicked.AddDynamic(this,&UOptionWidget::ClickCloseButton);

	ChangeWidgetSwitcher(0);
}

void UOptionWidget::ClickVideoButton()
{
	ChangeWidgetSwitcher(0);
}

void UOptionWidget::ClickAudioButton()
{
	ChangeWidgetSwitcher(1);
}

void UOptionWidget::ClickKeyBindButton()
{
	ChangeWidgetSwitcher(2);
}

void UOptionWidget::ClickApplyButton()
{
	
}
void UOptionWidget::ClickCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
	RemoveFromViewport();
	OnRemoveWidget.Broadcast();
	//RemoveFromParent();
}

void UOptionWidget::ChangeWidgetSwitcher(int32 Switchindex)
{
	Switcher->SetActiveWidgetIndex(Switchindex);
}