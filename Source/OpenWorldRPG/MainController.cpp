// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "Item/Container.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/CompassWidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"
#include "OpenWorldRPG/UI/CQBMissionResultWidget.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/Vehicles/NiceCar.h"

#include "OpenWorldRPG/Save/SavePlayer.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"



AMainController::AMainController()
{
	bIsInventoryVisible = false;

	
	if (AOpenWorldRPGGameModeBase* Mode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		Gmode = Mode;		
	}

}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("testSave", IE_Pressed, this, &AMainController::SaveGame);
	InputComponent->BindAction("testLoad", IE_Pressed, this, &AMainController::LoadGame);

}

void AMainController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	InitPlayerState();
	
	Cast<AMyPlayerState>(PlayerState)->AfterPossess();	
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();
	Main = Cast<AMainCharacter>(GetPawn());

	if (WMainHud)
	{
		if (!MainHud )
		{
			MainHud = CreateWidget<UMainHud>(this, WMainHud);
		}
		
		MainHud->AddToViewport();
		MainHud->SetVisibility(ESlateVisibility::Visible);
		MainHud->MainCon = this;
	}


}

void AMainController::ToggleInventory()
{
	if (MainHud->bIsInventoryWindowShow)
	{
		/* Hide Inventory */
		//HideInventory();
		MainHud->HideInventoryWindow();
		SetInputAndFocus(false);
	}
	else
	{
		/* Show Inventory */
		//ShowInventory();
		MainHud->ShowInventoryWindow();
		SetInputAndFocus(true);

		if (Gmode)
		{
			Gmode->ShowGameProgressNotiMessage(EGameProgressNotificationMessageType::EGPNMT_Levelup);
		}

	}
}

void AMainController::ControlInteractText(bool bIsInteract)
{
	if (bIsInteract)
	{
		MainHud->ShowInteractText();
	}
	else
	{
		MainHud->HideInteractText();
	}
}

void AMainController::ControlCoreUsableWidget(bool bIsTPSMode)
{
	//TPS mode인 경우 
	if (bIsTPSMode)
	{
		bIsCompassWidgetVisible = true;
		MainHud->CompassWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		bIsCompassWidgetVisible = false;
		MainHud->CompassWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	//for MainHUd::Show/HideOtherUI func
	MainHud->SettingIsCompassWidgetVisible(bIsCompassWidgetVisible);
}

void AMainController::SetInputAndFocus(bool bIsShow)
{
	if (bIsShow) //Set To UI Mode

	{
		//Show 일때 InputMode는 MainHud에서 직접해준다.

		bShowMouseCursor = true;
		//Main->DisableInput(this); //Player의 움직임 제한. -> 입력자체를 막아버리기 때문에 Toggle을 할 수 없음.
		Main->bDisableInput = true;
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
		bIsInventoryVisible = true;
	}
	else //Return To GameMode
	{
		Main->bDisableInput = false;

		FInputModeGameOnly Mode;
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
		bIsInventoryVisible = false;
		bIsInteractLootBox = false;
		bIsInteractCharacterLoot = false;
	}
}

void AMainController::Die()
{
	if (!WDeathWidget )	return;

	if(!DeathWidget )
	{
		DeathWidget = CreateWidget<UUserWidget>(this,WDeathWidget);
	}

	if (DeathWidget)
	{
		SetInputAndFocus(true);
		DeathWidget->AddToViewport();
	}
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void AMainController::Respawn()
{
	if (Main)
	{
		Main->Destroy();
	}

	//SpawnLocation
	FVector RespawnLoc = FVector(0.f);
	if (Car.IsValid())
	{
		//Car의 SeatSocketName으로
		RespawnLoc = Car->GetMesh()->GetSocketLocation(Car->SeatSocketName);

	}

	FActorSpawnParameters SpawnParms;
	SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Main = GetWorld()->SpawnActor<AMainCharacter>(AMainCharacter::StaticClass(), RespawnLoc, FRotator::ZeroRotator, SpawnParms);
	Possess(Main);
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

/*********** Vehicle ***********/
bool AMainController::ToggleCar(ANiceCar* vCar, FVector OutPos)
{
	bool Returnvalue = true;
	bool bNewState = !bIsinCar;
	if (bNewState != bIsinCar)
	{		
		switch (bIsinCar)
		{
		case true:
			//Get out car
			GetOuttheCar(OutPos);
			break;
		case false:
			//Get in car			
			Returnvalue = GetIntheCar(vCar);
			break;
		}
	}
	return Returnvalue;
}

bool AMainController::GetIntheCar(ANiceCar* vCar)
{
	//이 Owner를 타겟팅한 AI가 없어야 차를 탑승할 수 있도록 한다.
	if (vCar && bIsthisTargetForAI() == false)
	{
		
		Car = vCar;
		bIsinCar = true;
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget,false);
		Main->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Main->SetActorEnableCollision(false);
		Main->AttachToActor(vCar,Rules,vCar->SeatSocketName);
		this->Possess(vCar);

		Main->bDisableInteractionLineTrace = true;
		return true;
	}
	else
	{
		if (Gmode)
		{
			Gmode->ShowSystemNotiMessage(ESystemNotificationMessageType::ESNMT_GetInCarFailed);
		}
	}
	return false;
}

void AMainController::GetOuttheCar(FVector OutPos)//AVehicle* Car)
{
	bIsinCar = false;
	this->Possess(Main);
	
	Main->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	this->SetControlRotation(FRotator::ZeroRotator);

	Main->SetActorLocation(OutPos);
	Main->SetActorRotation(FRotator::ZeroRotator);
	Main->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Main->bDisableInteractionLineTrace = false;
	
}


void AMainController::AddToListTargetingThisActor(class AEnemyAIController* AICon)
{
	if(AICon)
	{
		TargetAIList.Add(AICon);
	}
}

void AMainController::RemoveAtListTargetingThisActor(AEnemyAIController* AICon)
{
	if (AICon && TargetAIList.Find(AICon) != INDEX_NONE)
	{
		TargetAIList.Remove(AICon);
	}
}

bool AMainController::bIsthisTargetForAI()
{
	//이 Controller의 Owner를 Targeting한 AI가 목록에 있다면 true를 리턴한다.
	if ( TargetAIList.Num() > 0 )
	{
		UE_LOG(LogTemp,Warning,TEXT("AMainController::bIsthisTargetForAI// Can't Take Car"));
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("AMainController::bIsthisTargetForAI// Can Take Car"));
	return false;
}



//void AMainController::UseQuickSlotItem(EQuickSlotNumber QuickSlotNum)
//{
//	OnQuickSlotUse.Broadcast(QuickSlotNum);
//	//MainHud->QuickSlot->UseItemInQuickSlot(QuickSlotNum);
//}

/* Save and Load game*/
void AMainController::LoadingProcessCompleted()
{
	if ( LoadingWidget && LoadingWidget->IsInViewport() )
	{
		LoadingWidget->SetVisibility(ESlateVisibility::Collapsed);
		GetWorldTimerManager().ClearTimer(LoadingwidgetTimer);
	}
}
void AMainController::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainController::SaveGame// try saving "));

	if(!WLoadingWidget ) return;

	if ( !LoadingWidget )
	{
		LoadingWidget = CreateWidget<UUserWidget>(this, WLoadingWidget);		
	}
	if ( LoadingWidget )
	{
		if ( !LoadingWidget->IsInViewport() )
		{
			LoadingWidget->AddToViewport();
		}
		LoadingWidget->SetVisibility(ESlateVisibility::Visible);
		
		FTimerDelegate TimerCallFunc;
		TimerCallFunc.BindUFunction(this, FName("LoadingProcessCompleted"));

		GetWorldTimerManager().SetTimer(LoadingwidgetTimer, TimerCallFunc, 2.f, true);
	}


	SavePlayerStatus();

	AOpenWorldRPGGameModeBase* Gamemode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if ( Gamemode )
	{
		Gamemode->SaveWorldStatus();
	}
	

}

void AMainController::LoadGame()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainController::LoadGame// try loading "));
	LoadPlayerStatus();

	AOpenWorldRPGGameModeBase* Gamemode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if ( Gamemode )
	{
		Gamemode->LoadWorldStatus();
	}
}

void AMainController::SavePlayerStatus()
{
	FString SlotName = TEXT("SavePlayer");
	int32 Index = 0;

	//Save가 이미 있는지 확인하고 있으면 로드후 저장, 없으면 새로 생성한다.
	if ( UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false )
	{
		SaveGame_Player = Cast<USavePlayer>(UGameplayStatics::CreateSaveGameObject(USavePlayer::StaticClass()));
	}
	else
	{
		SaveGame_Player = Cast<USavePlayer>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));
	}

	AMyPlayerState* MyState = Cast<AMyPlayerState>(PlayerState);

	if ( SaveGame_Player == nullptr || Main == nullptr || MyState == nullptr) return;

	//기존에 저장되어 있던걸 날리고 새로 저장한다.
	SaveGame_Player->PlayerEquipment.Empty(0);
	SaveGame_Player->SavedPocketItems.Empty(0);
	SaveGame_Player->SavedSecureItems.Empty(0);

	SaveGame_Player->IntelAcq = MyState->IntelAcq;

	SaveGame_Player->PlayerLevel = Main->StatManagementComponent->GetLevel();
	SaveGame_Player->PlayerStat = Main->StatManagementComponent->CurrentStat;

	
	OriginalToCopy(Main->Equipment->EquipmentItems, SaveGame_Player->PlayerEquipment);
	OriginalToCopy(Main->PocketStorage->Inventory, SaveGame_Player->SavedPocketItems);
	OriginalToCopy(Main->SecureBoxStorage->Inventory, SaveGame_Player->SavedSecureItems);

	/*SaveGame_Player->PlayerEquipment = Main->Equipment->EquipmentItems;
	SaveGame_Player->SavedPocketItems = Main->PocketStorage->Inventory;
	SaveGame_Player->SavedSecureItems = Main->SecureBoxStorage->Inventory;*/


	if ( UGameplayStatics::SaveGameToSlot(SaveGame_Player, SlotName, Index) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainController::SaveGame// Success Save!"));
	}
}

void AMainController::LoadPlayerStatus()
{
	FString SlotName = TEXT("SavePlayer");
	int32 Index = 0;
	if(UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainController::LoadGame// Load fail!"));
		return;
	}

	USavePlayer* LoadGame_Player = Cast<USavePlayer>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));

	if ( LoadGame_Player == nullptr || Main == nullptr ) return;
	UE_LOG(LogTemp, Warning, TEXT("AMainController::LoadGame// Load Success!"));

	Main->StatManagementComponent->SetLevel(LoadGame_Player->PlayerLevel);
	//Main->StatManagementComponent->StrengthLevel = LoadGame_Player->PlayerLevel;
	Main->StatManagementComponent->UpdateCurrentStats(LoadGame_Player->PlayerStat);

	TArray<UNewItemObject*> LoadObjArr;
	CopyToOriginal(LoadObjArr, LoadGame_Player->PlayerEquipment);
	Main->Equipment->UpdateEquipment(LoadObjArr);

	CopyToOriginal(LoadObjArr, LoadGame_Player->SavedPocketItems);
	Main->PocketStorage->UpdateInventory(LoadObjArr);

	CopyToOriginal(LoadObjArr, LoadGame_Player->SavedSecureItems);
	Main->SecureBoxStorage->UpdateInventory(LoadObjArr);
}


void AMainController::OriginalToCopy(TArray<UNewItemObject*>&Original, TArray<FSaveItem>& Copy)
{
	for ( UNewItemObject* ItemObj : Original )
	{
		if(ItemObj == nullptr || ItemObj->ItemInfo.DataAsset == nullptr) continue;

		//DataAsset 객체의 경로를 FString형태로 저장한다.
		FString PDAID = ItemObj->ItemInfo.DataAsset->GetPathName();
		
		FSaveItem SaveItemInfo(PDAID, ItemObj->ItemInfo.Count, ItemObj->ItemInfo.TopLeftIndex);
		Copy.Add(SaveItemInfo);

		//객체 저장
		//UBasePDA* PDA = LoadObject<UBasePDA>(nullptr, *PDAPN);
		//Copy.Add(ItemObj);
	}
}

void AMainController::CopyToOriginal(TArray<class UNewItemObject*>& Original, TArray<FSaveItem>& Copy)
{
	Original.Empty();
	for ( FSaveItem stct : Copy )
	{
		//PDAID에 저장된 경로로 해당하는 PDA를 찾는다.
		//첫 매개변수는 부모객체인데, nullptr을 넘겨줘 기본 Outer를 사용하도록함.
		//두번째 매개변수는 찾을 PDA의 경로.
		UBasePDA* LoadPDA = LoadObject<UBasePDA>(nullptr, *stct.PDAID);
		if(!LoadPDA ) continue;
		FItemSetting ItemSet(LoadPDA,stct.Count, stct.TopLeftIndex);
		bool bIsCreated = false;
		UNewItemObject* ItemObj = UCustomInventoryLibrary::CreateObject(ItemSet, bIsCreated);

		if(!bIsCreated) continue;

		Original.Add(ItemObj);
	}
}

void AMainController::ShowCQBResult(float ElapsedTime, int32 PerfectDrillCnt, int32 TotalScore)
{
	if (WCQBResultWidget)
	{
		CQBResultWidget = CreateWidget<UCQBMissionResultWidget>(this,WCQBResultWidget);
		if (CQBResultWidget)
		{
			CQBResultWidget->AfterConstruct(this); //CloseButton->OnClicked.AddDynamic(this,&AMainController::HideCQBResult);
			CQBResultWidget->SetVariable(ElapsedTime,PerfectDrillCnt,TotalScore);
			CQBResultWidget->AddToViewport();
			SetInputAndFocus(true);
		}
	}
}

void AMainController::HideCQBResult()
{
	CQBResultWidget->SetVisibility(ESlateVisibility::Collapsed);
	CQBResultWidget->RemoveFromViewport();
	SetInputAndFocus(false);
}