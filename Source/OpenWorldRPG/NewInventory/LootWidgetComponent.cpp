// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/MainController.h"
#include "LootBoxWidget.h"
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
					ULootBoxWidget* Widget = CreateWidget<ULootBoxWidget>(MainCon, WBPWidget);
					if (Widget)
					{
						Widget->InitLootBoxWidget(actor);
						MainInventory->SetRightWidget(Widget);
						//NewInventory�� ������ ������ LootBoxWidget�� �־��ش�.

					}
				}
				break;
			case EWidgetType::EWT_Character:
			{
				//UEquipWidget* Widget = CreateWidget<UEquipWidget>(MainCon, WBPWidget);
				ABaseCharacter* BChar = Cast<ABaseCharacter>(actor);
				UCharacterLootWidget* LWidget = CreateWidget<UCharacterLootWidget>(MainCon, WBPWidget);//(BChar, WBPWidget);
				if (LWidget && BChar)
				{
					LWidget->InitCharLootWidget(BChar);
					//ABaseCharacter* BChar = Cast<ABaseCharacter>(actor);
					//Widget->EquipInitialize(BChar->Equipment);
					MainInventory->SetRightWidget(LWidget);
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
