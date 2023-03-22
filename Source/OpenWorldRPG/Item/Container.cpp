// Fill out your copyright notice in the Description page of Project Settings.


#include "Container.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/MainHud.h"

#include "OpenWorldRPG/NewInventory/Widget/ContainerWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

AContainer::AContainer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable Mesh"));
	RootComponent = Mesh;

	ContainerInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("BoxInventoryComp"));
	LootWidgetComp = CreateDefaultSubobject<ULootWidgetComponent>(TEXT("LootWidgetComp"));
	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;
	
	bHasSpawnItem = false;
	

	//WidgetBlueprint'/Game/Inventory/WBP_LootBox.WBP_LootBox'
	//위젯 하드코딩
	/*
	WidgetBlueprint'/Game/Inventory/WBP_LootBox.WBP_LootBox'
	static ConstructorHelpers::FObjectFinder<UUserWidget> TempLootBoxWidget(TEXT("WidgetBlueprint'/Game/Inventory/WBP_LootBox.WBP_LootBox_C'"));
	if (TempLootBoxWidget.Succeeded())
	{
		WLootBoxWidget = TempLootBoxWidget.Object;
	
		
		//LootBoxWidget = CreateWidget<ULootBoxWidget>(this, WLootBoxWidget);
		if (WLootBoxWidget)
		{
			LootBoxWidget = Cast<ULootBoxWidget>(WLootBoxWidget.GetDefaultObject());
			if (LootBoxWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("LootBox:: widget success"));
			}
		}
	}
	*/
	
}

void AContainer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StimuliComp->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliComp->RegisterWithPerceptionSystem();	
}

void AContainer::BeginPlay()
{
	Super::BeginPlay();
	//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//InventoryComponent의 초기화가 BeginPlay에서 되기때문에 여기다 했다.
	LootWidgetComp->WidgetType = EWidgetType::EWT_LootBox;

	SettingStorage();
	if (bHasSpawnItem)
	{
		SpawnItem();
	}
}

void AContainer::SettingStorage()
{
	ContainerStorage = NewObject<UItemStorageObject>();
	ContainerStorage->InitializeStorage(StorageX, StorageY, StorageTileSize);
}

void AContainer::SpawnItem()
{
	for (auto AddItem : SpawnItemList)
	{
		AItem* Item = GetWorld()->SpawnActor<AItem>(AddItem);
		if (Item)
		{
			if (ContainerInventoryComp->TryAddItem(ContainerStorage,Item->ItemSetting,nullptr,Item,true))
			{
				Item->SetItemState(EItemState::EIS_Pickup);
				//Item->ItemObj->bIsDestoryed = true;
				Item->Destroy();
			}
		}
	}
}
void AContainer::Interaction(AActor* Actor)
{
	OpenContainer(Actor);
}

void AContainer::OpenContainer(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("LootBox::OpenBox, BoxName : %s"), *this->GetFName().ToString());

	AMainController* MainCon = Cast<AMainController>(Actor->GetInstigatorController());
	AEnemyAIController* AICon = Cast<AEnemyAIController>(Actor->GetInstigatorController());

	AMainCharacter* MainChar = Cast<AMainCharacter>(Actor);

	if (MainCon && MainChar)
	{
		//해제는 MainController::HideInventory에서 함.
		MainChar->InteractLootBox = this;

		MainCon->bIsInteractLootBox = true;
		ShowWidget(MainCon);
	}

	if(AICon)
	{
		AICon->ItemFarming(this);
	}
}

void AContainer::ShowWidget(AMainController* MainCon)
{
	UNewInventory* MainInventory = Cast<UNewInventory>(MainCon->MainHud->NewInventoryWidget);
	if (MainInventory)
	{
		LootWidgetComp->CreateInteractionWidget(MainCon, this); //새로추가
	}
}

void AContainer::SetOutline()
{
	Mesh->SetRenderCustomDepth(true);
}

void AContainer::UnsetOutline()
{
	Mesh->SetRenderCustomDepth(false);
}

/*
void ALootBox::SelectAndStoreLootItem()
{
	int32 BoxMaxNum = BoxItem.Num() - 1;
	if (BoxMaxNum > 0)
	{
		for (int32 i = 0; i < LootItemCount; ++i)
		{
			int32 Select = FMath::RandRange(0, BoxMaxNum);
			if (BoxItem[Select] != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("LootBox::Select"));
				AItem* Item = Cast<AItem>(BoxItem[Select].GetDefaultObject());
				if (Item)
				{
					UE_LOG(LogTemp, Warning, TEXT("LootBox::Call AddItem"));
					BoxInvComp->AddItem(Item);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LootBox::Select fail"));
			}
		}
	}

}
*/