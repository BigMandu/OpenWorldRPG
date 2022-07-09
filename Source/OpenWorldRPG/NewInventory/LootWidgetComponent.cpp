// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/MainController.h"
#include "ContainerWidget.h"
#include "NewInventory.h"
#include "OpenWorldRPG/BaseCharacter.h"

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
		UNewInventory* MainInventory = Cast<UNewInventory>(MainCon->NewInventory);
		if (MainInventory)
		{
			switch (WidgetType)
			{
			case EWidgetType::EWT_LootBox:
				{
					UContainerWidget* Widget = CreateWidget<UContainerWidget>(MainCon, WBPWidget);
					if (Widget)
					{
						Widget->InitContainerWidget(actor);
						MainInventory->SetRightWidget(Widget);

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
		MainCon->ShowInventory_Implementation();// ToggleInventory();
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
