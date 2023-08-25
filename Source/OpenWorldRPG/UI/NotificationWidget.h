// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "NotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	FText SystemMessage(ESystemNotificationMessageType MsgType);
	FText GameProgressMessage(EGameProgressNotificationMessageType MsgType);


	/*class UNotifyMessageWidget* NotifiSysMsgWidget;
	UNotifyMessageWidget* NotifyGameProgMsgWidget;*/

	int32 VisibleStackCount;
	void CheckStackCount();

protected:
	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNotifyMessageWidget> W_SystemNotifyMsgWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNotifyMessageWidget> W_GameProgressNotifyMsgWidget;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* NotifyBox;

	void AddNotificationMessage(ESystemNotificationMessageType MsgType);
	void AddNotificationMessage(EGameProgressNotificationMessageType MsgType);
	
};
