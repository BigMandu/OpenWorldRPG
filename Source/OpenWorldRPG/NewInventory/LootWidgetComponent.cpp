// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipStorageWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/ContainerWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainHud.h"

// Sets default values for this component's properties
ULootWidgetComponent::ULootWidgetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULootWidgetComponent::CreateInteractionWidget(AMainController* MainCon, AActor* actor)
{
	if (MainCon)
	{
		UNewInventory* MainInventory = Cast<UNewInventory>(MainCon->MainHud->NewInventoryWidget);
		if (MainInventory)
		{
			MainInventory->ChangeMainSwitchToInventory();

			switch (WidgetType)
			{
			case EWidgetType::EWT_LootBox:
				{
					UContainerWidget* RWidget = CreateWidget<UContainerWidget>(MainCon, WBPWidget);
					if (RWidget)
					{
						RWidget->InitContainerWidget(actor);
						MainInventory->SetRightWidget(RWidget);

						//NewInventory의 오른쪽 위젯에 LootBoxWidget을 넣어준다.

					}
				}
				break;
			case EWidgetType::EWT_Character:
			{
				ABaseCharacter* BChar = Cast<ABaseCharacter>(actor);
				UCharacterLootWidget* RWidget = CreateWidget<UCharacterLootWidget>(MainCon, WBPWidget);//(BChar, WBPWidget);
				if (RWidget && BChar)
				{
					//for Delegate, CharLootWidget에 있는 EquipStorageWidget의 OpenAdditional bind를 위함
					RWidget->EquipStorageWidget->MainWidget = MainInventory;
					RWidget->InitCharLootWidget(BChar);					
					//ABaseCharacter* BChar = Cast<ABaseCharacter>(actor);
					//Widget->EquipInitialize(BChar->Equipment);
					MainInventory->SetRightWidget(RWidget);
				}
			}
				break;
			case EWidgetType::EWT_StorageEquip:
				break;
			}

		}
		MainCon->ToggleInventory();
		//MainCon->ShowInventory_Implementation();// ToggleInventory();
		//MainCon->MainHud->ShowInventoryWindow();
	}
}


// Called when the game starts
//void ULootWidgetComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//	// ...
//	
//}
