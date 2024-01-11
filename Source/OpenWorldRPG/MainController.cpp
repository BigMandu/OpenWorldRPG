// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "Item/Container.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"

#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/CompassWidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"
#include "OpenWorldRPG/UI/CQBMissionResultWidget.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/Vehicles/NiceCar.h"

#include "OpenWorldRPG/GameData/MyPlayerState.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "OpenWorldRPG/Save/SavePlayer.h"





AMainController::AMainController()
{
	bIsInventoryVisible = false;

}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ESCmenu", IE_Pressed, this, &AMainController::ShowESCmenu);
	InputComponent->BindAction("testSave", IE_Pressed, this, &AMainController::SaveGame);
	InputComponent->BindAction("testLoad", IE_Pressed, this, &AMainController::LoadGame);
	

}

void AMainController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	//InitPlayerState();	
	//Cast<AMyPlayerState>(PlayerState)->AfterPossess();

	ANiceCar* CarRef = Cast<ANiceCar>(aPawn);

	//Car가 아닌경우에만 PlayerChar와 InputFocus를 설정한다.
	if(!CarRef)
	{
		PlayerChar = Cast<AMainCharacter>(aPawn);// GetPawn());
		SetInputAndFocus(false);
	}
	//if(MainHud && MainHud->NewInventoryWidget)
	//{
	//	//Possess가 호출되면 Inventory widget의 data를 초기화 시킨다.
	//	MainHud->NewInventoryWidget->InitUIData();
	//}
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	if (AOpenWorldRPGGameModeBase* Mode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		Gmode = Mode;
	}

	if (WMainHud)
	{
		if (!MainHud)
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
	if(!MainHud) return;

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
		SetInputAndFocus(true, MainHud->NewInventoryWidget);


		//this is for test
		if (Gmode)
		{
			Gmode->ShowGameProgressNotiMessage(EGameProgressNotificationMessageType::EGPNMT_Levelup);
		}

	}
}

void AMainController::InitUI()
{
	if (MainHud && MainHud->NewInventoryWidget)
	{
		//Possess가 호출되면 Inventory widget의 data를 초기화 시킨다.
		MainHud->NewInventoryWidget->InitUIData();
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

void AMainController::SetInputAndFocus(bool bIsShow, UUserWidget* FocusWidget)
{
	if (bIsShow && FocusWidget) //Set To UI Mode
	{
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(FocusWidget->TakeWidget());
		SetInputMode(Mode);
		bShowMouseCursor = true;

		//Main->DisableInput(this); //Player의 움직임 제한. -> 입력자체를 막아버리기 때문에 Toggle을 할 수 없음.
		PlayerChar->bDisableInput = true;
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
		bIsInventoryVisible = true;
	}
	else //Return To GameMode
	{
		PlayerChar->bDisableInput = false;

		//FInputModeGameOnly Mode;
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
		bIsInventoryVisible = false;
		bIsInteractLootBox = false;
		bIsInteractCharacterLoot = false;
	}
}


void AMainController::ShowESCmenu()
{
	UMyGameInstance* MyGameInst = GetWorld()->GetGameInstance<UMyGameInstance>();
	if(!MyGameInst) return;
	MyGameInst->ToggleESCWidget();
	

}


/**죽으면 deathwidget을 띄우도록 했는데 그냥, 별도 위젯 띄우지 말고
 * 바로 continue하도록 하자.
 */
void AMainController::Die()
{
	LoadGame();
	/*if (!WDeathWidget)	return;

	if(!DeathWidget)
	{
		DeathWidget = CreateWidget<UUserWidget>(this,WDeathWidget);
	} 

	if (DeathWidget)
	{
		SetInputAndFocus(true);
		DeathWidget->AddToViewport();
	}*/
}

//Not use function
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void AMainController::Respawn()
{
	if (PlayerChar)
	{
		PlayerChar->Destroy();
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

	//PlayerChar = GetWorld()->SpawnActor<AMainCharacter>(AMainCharacter::StaticClass(), RespawnLoc, FRotator::ZeroRotator, SpawnParms);
	//Possess(Main);
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

//MainCharacter에서 DetachFromControllerPendingDestroy함수 실행시 호출됨.
void AMainController::UnFreeze()
{
	ServerRestartPlayer();
}

/*********** Vehicle ***********/
bool AMainController::ToggleCar(ANiceCar* vCar, FVector OutPos)
{
	bool bIsCanGetInCar = false;
	
	switch (bIsinCar)
	{
	case true:
		//Get out car
		GetOuttheCar(OutPos);
		
		break;

	case false:
		//Get in car			
		bIsCanGetInCar = GetIntheCar(vCar);
		break;
	}
	
	return bIsCanGetInCar;
}

bool AMainController::GetIntheCar(ANiceCar* vCar)
{
	//이 Owner를 타겟팅한 AI가 없어야 차를 탑승할 수 있도록 한다.
	if (vCar && bIsthisTargetForAI() == false)
	{
		Car = vCar;
		bIsinCar = true;	

		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget,false);
		PlayerChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//PlayerChar->SetActorEnableCollision(false);
		PlayerChar->AttachToActor(Car.Get(), Rules, Car->SeatSocketName);

		Car->SetCharacter(PlayerChar/* Cast<ABaseCharacter>(this->GetCharacter())*/);


		this->Possess(Car.Get());

		PlayerChar->bDisableInteractionLineTrace = true;

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
	this->Possess(PlayerChar);
	
	PlayerChar->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	this->SetControlRotation(FRotator::ZeroRotator);

	PlayerChar->SetActorLocation(OutPos);
	PlayerChar->SetActorRotation(FRotator::ZeroRotator);
	PlayerChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	PlayerChar->bDisableInteractionLineTrace = false;
	
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

void AMainController::RemoveAllAITargetingList()
{
	TargetAIList.Empty();
	//TargetAIList.RemoveAll(,)
}

bool AMainController::bIsthisTargetForAI()
{
	if (GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainController::GetIntheCar"));
		UE_LOG(LogTemp, Warning, TEXT("Did This call Twice? Owning Actor: %s | Component Name: %s"), *GetOwner()->GetName(), *GetName());
	}

	//이 Controller의 Owner를 Targeting한 AI가 목록에 있다면 true를 리턴한다.
	if (TargetAIList.Num() > 0)
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
	if (LoadingWidget && LoadingWidget->IsInViewport())
	{
		LoadingWidget->SetVisibility(ESlateVisibility::Collapsed);
		GetWorldTimerManager().ClearTimer(LoadingwidgetTimer);
	}
}
void AMainController::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainController::SaveGame// try saving "));
	AOpenWorldRPGGameModeBase* Gamemode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(!WLoadingWidget) return;
	if(!Gamemode) return;

	if (!LoadingWidget)
	{
		LoadingWidget = CreateWidget<UUserWidget>(this, WLoadingWidget);		
	}

	if(!LoadingWidget) return;
			
	if (LoadingWidget->IsInViewport() == false)
	{
		LoadingWidget->AddToViewport();
	}

	LoadingWidget->SetVisibility(ESlateVisibility::Visible);
		
	FTimerDelegate TimerCallFunc;
	TimerCallFunc.BindUFunction(this, FName("LoadingProcessCompleted"));

	GetWorldTimerManager().SetTimer(LoadingwidgetTimer, TimerCallFunc, 2.f, true);
	

	SavePlayerStatus();
	Gamemode->SaveWorldStatus(Car.Get());

}

void AMainController::LoadGame()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainController::LoadGame// try loading "));
	LoadPlayerStatus();

	AOpenWorldRPGGameModeBase* Gamemode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (Gamemode)
	{
		Gamemode->LoadWorldStatus();
	}
}

void AMainController::SavePlayerStatus()
{
	FString SlotName = TEXT("SavePlayer");
	int32 Index = 0;

	//Save가 이미 있는지 확인하고 있으면 로드후 저장, 없으면 새로 생성한다.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false)
	{
		SaveGame_Player = Cast<USavePlayer>(UGameplayStatics::CreateSaveGameObject(USavePlayer::StaticClass()));
	}
	else
	{
		SaveGame_Player = Cast<USavePlayer>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));
	}

	AMyPlayerState* MyState = Cast<AMyPlayerState>(PlayerState);

	if (SaveGame_Player == nullptr || PlayerChar == nullptr || MyState == nullptr) return;

	//기존에 저장되어 있던걸 날리고 새로 저장한다.
	SaveGame_Player->PlayerEquipment.Empty(0);
	SaveGame_Player->SavedPocketItems.Empty(0);
	SaveGame_Player->SavedSecureItems.Empty(0);

	SaveGame_Player->IntelAcq = MyState->IntelAcq;
	SaveGame_Player->PlayerLevel = PlayerChar->StatManagementComponent->GetLevel();
	SaveGame_Player->PlayerStat = PlayerChar->StatManagementComponent->CurrentStat;

	
	OriginalToCopy(PlayerChar->Equipment->EquipmentItems, SaveGame_Player->PlayerEquipment);

	OriginalToCopy(*PlayerChar->PocketStorage, SaveGame_Player->SavedPocketItems);
	OriginalToCopy(*PlayerChar->SecureBoxStorage, SaveGame_Player->SavedSecureItems);
	/* 자동차 저장*/
	SaveGame_Player->SavedCarTrunkItems.Empty();
	OriginalToCopy(*Car->StorageObj, SaveGame_Player->SavedCarTrunkItems);

	SaveGame_Player->SaveCarInfo = FSaveUserCar(Car->VehicleRowHandle.RowName, Car->GetActorTransform());
	


	if (UGameplayStatics::SaveGameToSlot(SaveGame_Player, SlotName, Index))
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

	if (LoadGame_Player == nullptr || PlayerChar == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("AMainController::LoadGame// Load Success!"));

	PlayerChar->StatManagementComponent->SetLevel(LoadGame_Player->PlayerLevel);
	//Main->StatManagementComponent->StrengthLevel = LoadGame_Player->PlayerLevel;
	PlayerChar->StatManagementComponent->UpdateCurrentStats(LoadGame_Player->PlayerStat);

	TArray<UNewItemObject*> LoadObjArr;
	CopyToOriginal(LoadObjArr, LoadGame_Player->PlayerEquipment);
	PlayerChar->Equipment->UpdateEquipment(LoadObjArr);

	CopyToOriginal(LoadObjArr, LoadGame_Player->SavedPocketItems);
	PlayerChar->PocketStorage->UpdateInventory(LoadObjArr);

	CopyToOriginal(LoadObjArr, LoadGame_Player->SavedSecureItems);
	PlayerChar->SecureBoxStorage->UpdateInventory(LoadObjArr);

	LoadUsedCar(*LoadGame_Player);
}

void AMainController::LoadUsedCar(USavePlayer& LoadGamePlayer)
{
	TArray<AActor*> WorldVehicleList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANiceCar::StaticClass(), WorldVehicleList);

	for (AActor* Vehicle : WorldVehicleList)
	{
		if (ANiceCar* CarRef = Cast<ANiceCar>(Vehicle))
		{
			if (CarRef->VehicleRowHandle.RowName.Compare(LoadGamePlayer.SaveCarInfo.VehicleDataRowName) == 0)
			{
				Car = CarRef;
				/* Vehicle Teleport to saved point. */
				FTransform TfVehicle = LoadGamePlayer.SaveCarInfo.VehicleTransform;
				Car->TeleportVehicle(TfVehicle);

				/* Load vehicle trunk items. */
				TArray<UNewItemObject*> LoadObjArr;
				CopyToOriginal(LoadObjArr, LoadGamePlayer.SavedCarTrunkItems);
				Car->StorageObj->RemoveAllItems();
				Car->StorageObj->UpdateInventory(LoadObjArr);				

				return;				
			}
		}
	}
}

/* StorageObject의 Inventory를 그대로 복사해
* 복사된 Array를 이용해 data를 저장한다.
* Object를 새로 생성하는것 보다 비용이 적게 들기 때문에 아래 방식을 사용함.
*
* 원래는 NewObject를 이용하여 새 StorageObject를 생성, 해당 객체로 편하게도 가능하지만
* 아래 방식이 더 빠름.
*/
void AMainController::OriginalToCopy(UItemStorageObject& TempStorage, TArray<FSaveItem>& Copy)
{
	TArray<UNewItemObject*> TempInventory = TempStorage.Inventory;

	for (UNewItemObject* ItemObj : TempInventory)
	{
		if (ItemObj == nullptr || ItemObj->ItemInfo.DataAsset == nullptr) continue;

		//DataAsset 객체의 경로를 FString형태로 저장한다.
		FString PDAID = ItemObj->ItemInfo.DataAsset->GetPathName();
		FSaveItem SaveItemInfo(PDAID, ItemObj->ItemInfo.Count, ItemObj->ItemInfo.TopLeftIndex);
		Copy.Add(SaveItemInfo);

		/* 저장 했다면 TempInventory에서 저장된 Item을 삭제한다.
		 * 삭제 하지 않으면 Item의 Size만큼 계속해서 저장됨.
		 *
		 * 아래 코드는 ItemStorageObject의 RemoveItem 함수임.
		 */

		FIntPoint Size = ItemObj->GetItemSize();
		FTile ItemIndex = TempStorage.IndexToTile(ItemObj->TopLeftIndex);
		int32 loopX = ItemIndex.X + Size.X - 1;
		int32 loopY = ItemIndex.Y + Size.Y - 1;

		FTile removeTile;

		for (int32 i = ItemIndex.X; i <= loopX; i++)
		{
			for (int32 j = ItemIndex.Y; j <= loopY; j++)
			{
				removeTile.X = i;
				removeTile.Y = j;
				int32 removeIndex = TempStorage.TileToIndex(removeTile);

				if (TempInventory[removeIndex] != ItemObj)
				{
					UE_LOG(LogTemp, Warning, TEXT("AMainController::OriginalToCopy : Some thing wrong.."));
					return;
				}
				TempInventory[removeIndex] = nullptr;
			}
		}

	}
}

void AMainController::OriginalToCopy(TArray<UNewItemObject*>& Original, TArray<FSaveItem>& Copy)
{
	for (UNewItemObject* ItemObj : Original)
	{
		if(ItemObj == nullptr || ItemObj->ItemInfo.DataAsset == nullptr) continue;

		//DataAsset 객체의 경로를 FString형태로 저장한다.
		FString PDAID = ItemObj->ItemInfo.DataAsset->GetPathName();
		
		FSaveItem SaveItemInfo(PDAID, ItemObj->ItemInfo.Count, ItemObj->ItemInfo.TopLeftIndex);
		Copy.Add(SaveItemInfo);
	}
}

void AMainController::CopyToOriginal(TArray<UNewItemObject*>& Original, TArray<FSaveItem>& Copy)
{
	Original.Empty();
	for (FSaveItem stct : Copy)
	{
		//PDAID에 저장된 경로로 해당하는 PDA를 찾는다.
		//첫 매개변수는 부모객체인데, nullptr을 넘겨줘 기본 Outer를 사용하도록함.
		//두번째 매개변수는 찾을 PDA의 경로.
		UBasePDA* LoadPDA = LoadObject<UBasePDA>(nullptr, *stct.PDAID);
		if(!LoadPDA) continue;
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
			SetInputAndFocus(true, CQBResultWidget);
		}
	}
}

void AMainController::HideCQBResult()
{
	CQBResultWidget->SetVisibility(ESlateVisibility::Collapsed);
	CQBResultWidget->RemoveFromViewport();
	SetInputAndFocus(false);
}