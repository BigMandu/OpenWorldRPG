// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBox.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/EnemyAIController.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/NewInventory/LootBoxWidget.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ALootBox::ALootBox()
{
	BoxInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("BoxInventoryComp"));
	LootWidgetComp = CreateDefaultSubobject<ULootWidgetComponent>(TEXT("LootWidgetComp"));
	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;


	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	bHasSpawnItem = false;
	/*Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Block);*/


	


	

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
	
	//LootItemCount = 1;
}

void ALootBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StimuliComp->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliComp->RegisterWithPerceptionSystem();	
}

void ALootBox::BeginPlay()
{
	Super::BeginPlay();
	//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//InventoryComponent의 초기화가 BeginPlay에서 되기때문에 여기다 했다.
	if (bHasSpawnItem)
	{
		SpawnItem();
	}
}

void ALootBox::SpawnItem()
{
	for (auto AddItem : SpawnItemList)
	{
		AItem* Item = GetWorld()->SpawnActor<AItem>(AddItem);
		if (Item)
		{
			//Item->Pickup(this);

			if (BoxInventoryComp->TryAddItem(Item->GetDefaultItemObj()))
			{
				Item->GetDefaultItemObj()->bIsDestoryed = true;
				Item->Destroy();
			}
		}
	}
}

void ALootBox::OpenBox(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("LootBox::OpenBox, BoxName : %s"), *this->GetFName().ToString());

	AMainController* MainCon = Cast<AMainController>(Actor->GetInstigatorController());
	AEnemyAIController* AICon = Cast<AEnemyAIController>(Actor->GetInstigatorController());
	if (MainCon)
	{
		MainCon->bIsInteractLootBox = true;
		ShowWidget(MainCon);
	}

	if(AICon)
	{
		AICon->ItemFarming(this);
	}
}

/*
	시작하자마자 LootBox Interaction시 LootBox위젯이 나오지 않음.
	인벤토리를 한번 보고나면 보임... ->> 해결해야됨.
*/
void ALootBox::ShowWidget(AMainController* MainCon)
{
	UNewInventory* MainInventory = Cast<UNewInventory>(MainCon->NewInventory);
	if (MainInventory)
	{
		LootWidgetComp->CreateInteractionWidget(MainCon, this); //새로추가

		//LootBoxWidget = CreateWidget<ULootBoxWidget>(MainCon, WLootBoxWidget);
		//if (LootBoxWidget)
		//{
		//	LootBoxWidget->InitLootBoxWidget(this);

		//	UE_LOG(LogTemp, Warning, TEXT("LootBox::ShowWidget, Widget name : %s"), *LootBoxWidget->GetFName().ToString());
		//	//NewInventory의 오른쪽 위젯에 LootBoxWidget을 넣어준다.
		//	MainInventory->SetRightWidget(LootBoxWidget);
		//	MainCon->ShowInventory_Implementation();// ToggleInventory();
		//}
	}
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