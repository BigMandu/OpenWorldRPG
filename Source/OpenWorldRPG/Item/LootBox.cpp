// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBox.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/NewInventory/LootBoxWidget.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ALootBox::ALootBox()
{
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	BoxInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("BoxInventoryComp"));


	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;

	//WidgetBlueprint'/Game/Inventory/WBP_LootBox.WBP_LootBox'
	//���� �ϵ��ڵ�
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
	MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	


	/*if (WLootBoxWidget && MainCon)
	{
		LootBoxWidget = CreateWidget<ULootBoxWidget>(MainCon, WLootBoxWidget);
		if (LootBoxWidget)
		{
			LootBoxWidget->InitLootBoxWidget(this);
		}
	}	*/
}

void ALootBox::OpenBox(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("LootBox::OpenBox, BoxName : %s"), *this->GetFName().ToString());

	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		MainCon = Main->MainController;
		MainCon->bIsInteractLootBox = true;
		ShowWidget();
	}
}

/*
	�������ڸ��� LootBox Interaction�� LootBox������ ������ ����.
	�κ��丮�� �ѹ� ������ ����... ->> �ذ��ؾߵ�.
*/
void ALootBox::ShowWidget()
{
	UNewInventory* MainInventory = Cast<UNewInventory>(MainCon->NewInventory);
	if (MainInventory)
	{
		LootBoxWidget = CreateWidget<ULootBoxWidget>(MainCon, WLootBoxWidget);
		if (LootBoxWidget)
		{
			LootBoxWidget->InitLootBoxWidget(this);

			UE_LOG(LogTemp, Warning, TEXT("LootBox::ShowWidget, Widget name : %s"), *LootBoxWidget->GetFName().ToString());
			MainInventory->SetRightWidget(LootBoxWidget);
			MainCon->ShowInventory_Implementation();// ToggleInventory();
		}
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