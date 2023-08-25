// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/NotificationWidget.h"
#include "OpenWorldRPG/UI/NotifyMessageWidget.h"

#include "Components/VerticalBox.h"

//#define LOCTEXT_NAMESPACE "NotificationMSG"

void UNotificationWidget::NativeConstruct()
{

}

FText UNotificationWidget::SystemMessage(ESystemNotificationMessageType MsgType)
{
	FText Message;
	switch (MsgType)
	{
		case ESystemNotificationMessageType::ESNMT_ReloadFailed:
			Message = NSLOCTEXT("NotificationNS","ReloadFail", "Weapon is busy. You can't reload while sprinting or shooting.");
		break;
		case ESystemNotificationMessageType::ESNMT_SaveFailed:
			Message = NSLOCTEXT("NotificationNS", "SaveFail", "Car is in open area. You have to park where the surrounded by walls on 3 sides.");
		break;
		case ESystemNotificationMessageType::ESNMT_GetInCarFailed:
			Message = NSLOCTEXT("NotificationNS", "GetInCarFailed", "The enemy chasing you is nearby. Defeat them or hide.");
		break;
		case ESystemNotificationMessageType::ESNMT_WeaponIsBusy:
			Message = NSLOCTEXT("NotificationNS", "WeaponBusy", "Weapon is busy.");
		break;
	}

	return Message;
}

FText UNotificationWidget::GameProgressMessage(EGameProgressNotificationMessageType MsgType)
{
	FText Message;
	switch (MsgType)
	{
		case EGameProgressNotificationMessageType::EGPNMT_Healing:
			Message = NSLOCTEXT("NotificationNS", "Healing", "Healing (10)");
		break;
		case EGameProgressNotificationMessageType::EGPNMT_MissionClear:
			Message = NSLOCTEXT("NotificationNS", "MissionClear", "Mission completed");
		break;
		case EGameProgressNotificationMessageType::EGPNMT_Levelup:
			Message = NSLOCTEXT("NotificationNS", "LevelUp", "Level up");
		break;
	}

	return Message;
}

/* System Message */
void UNotificationWidget::AddNotificationMessage(ESystemNotificationMessageType MsgType)
{
	if (W_SystemNotifyMsgWidget)
	{
		UNotifyMessageWidget* NotifyMsgWidget = CreateWidget<UNotifyMessageWidget>(this, W_SystemNotifyMsgWidget);
		if(NotifyMsgWidget)
		{
			//FText testtxt = FText::FromString(TEXT("show message, and longgggg mmmmmmmeeeesssaagggeeeee"));
			FText MessageContext = SystemMessage(MsgType);			

			if (MessageContext.IsEmpty() == false)
			{
				NotifyMsgWidget->SetMessageText(MessageContext);
				NotifyBox->AddChildToVerticalBox(NotifyMsgWidget);

				VisibleStackCount = 5;
				CheckStackCount();
			}		
		}
	}
}

/* Ingame Progress Message */
void UNotificationWidget::AddNotificationMessage(EGameProgressNotificationMessageType MsgType)
{
	if (W_GameProgressNotifyMsgWidget)
	{
		UNotifyMessageWidget* NotifyMsgWidget = CreateWidget<UNotifyMessageWidget>(this, W_SystemNotifyMsgWidget);
		if (NotifyMsgWidget)
		{
			//FText testtxt = FText::FromString(TEXT("show message, and longgggg mmmmmmmeeeesssaagggeeeee"));
			FText MessageContext = GameProgressMessage(MsgType);

			if (MessageContext.IsEmpty() == false)
			{
				/* GameProgress Message는 bottom-top으로 쌓기 위해
				*	verticalbox를 180도 회전하여 거꾸로 놓는다.
				*	messagewidget도 같이 돌아가므로, message는 180도 돌려 다시 원복하여
				*	bottom to top message로 보여지게 한다.
				*/
				NotifyMsgWidget->SetRenderTransformAngle(180.f);
				NotifyBox->SetRenderTransformAngle(180.f);

				NotifyMsgWidget->SetMessageText(MessageContext);
				
				NotifyBox->AddChildToVerticalBox(NotifyMsgWidget);

				VisibleStackCount = 2;
				CheckStackCount();
			}
		}
	}
}

void UNotificationWidget::CheckStackCount()
{
	TArray<UWidget*> AddedNotiWidget = NotifyBox->GetAllChildren();

	//추가된 message 위젯이 stackcount보다 적으면 추가해도 상관없음, 리턴한다.
	if (AddedNotiWidget.Num() <= VisibleStackCount) return;

	//가장 오래된 위젯을 가져와서 삭제한다.
	UWidget* wid = *AddedNotiWidget.GetData();

	if(wid)
	{
		if (UNotifyMessageWidget* Msgwid = Cast<UNotifyMessageWidget>(wid))
		{
			GetWorld()->GetTimerManager().ClearTimer(Msgwid->DisplayedTimer);
			Msgwid->RemoveThisMessage();
		}
	}
	

	/*for (UWidget* wid : AddedNotiWidget)
	{
		if (UNotifyMessageWidget* Msgwid = Cast<UNotifyMessageWidget>(wid))
		{
			GetWorld()->GetTimerManager().ClearTimer(Msgwid->DisplayedTimer);
			Msgwid->RemoveThisMessage();
		}
	}*/
}

//#undef LOCTEXT_NAMESPACE