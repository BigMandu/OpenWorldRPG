// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "Item/Container.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/CompassWidget.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/Vehicles/NiceCar.h"



AMainController::AMainController()
{
	bIsInventoryVisible = false;

}

void AMainController::BeginPlay()
{
	Super::BeginPlay();
	Main = Cast<AMainCharacter>(GetCharacter());

	if (WMainHud)
	{
		MainHud = CreateWidget<UMainHud>(this,WMainHud);
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
	if (WDeathWidget == nullptr)
	{
		return;
	}

	DeathWidget = CreateWidget<UUserWidget>(this,WDeathWidget);
	if (DeathWidget)
	{
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