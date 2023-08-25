// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyMessageWidget.h"

// 제일 마지막에 호출됨
void UNotifyMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("UNotifyMessageWidget::NativeConstruct"));
	
	GetWorld()->GetTimerManager().SetTimer(DisplayedTimer,this, &UNotifyMessageWidget::RemoveThisMessage, 4.f,false);
}

void UNotifyMessageWidget::SetMessageText(FText msg)
{
	UE_LOG(LogTemp, Warning, TEXT("UNotifyMessageWidget::SetMessageText"));
	Message->SetText(msg);
}

void UNotifyMessageWidget::RemoveThisMessage()
{
	RemoveFromParent();
	ConditionalBeginDestroy();
}