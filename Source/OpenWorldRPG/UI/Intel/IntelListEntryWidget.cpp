// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Intel/IntelListEntryWidget.h"
#include "OpenWorldRPG/UI/Intel/IntelListItemObject.h"
#include "OpenWorldRPG/UI/Intel/IntelAcquiredListWidget.h"
#include "OpenWorldRPG/GameData/MyPlayerState.h"

//this function called from AcquiredListWidget's SetListView  IntelList->AddItem func.
void UIntelListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	//Object를 생성한담에 여기서 cast후 setting하자.
	UIntelListItemObject* ListObj = Cast<UIntelListItemObject>(ListItemObject);
	if(ListObj)
	{
		IntelName->SetText(ListObj->IntelName);
		IntelType = ListObj->IntelType;
		IntelButton->OnClicked.AddDynamic(this, &UIntelListEntryWidget::ClickIntel);
	}
}

//not use broadcast,, just static called.
void UIntelListEntryWidget::ClickIntel()
{
	if (GetOuter()->GetOuter())
	{
		if (UIntelAcquiredListWidget* IALW = Cast<UIntelAcquiredListWidget>(GetOuter()->GetOuter()))
		{
			if (AMyPlayerState* PlayerState = Cast<AMyPlayerState>(GetOwningPlayerState()))
			{
				if (PlayerState->CanReadThisIntel(IntelType))
				{
					IALW->ReadIntel(IntelType);
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("UIntelListEntryWidget::ClickIntel// You can't read this INTEL. you haven't got this intel yet."));
				}
			}
			

			//ClickIntel.Broadcast(IntelType);
			//IALW;


		}
	}
}