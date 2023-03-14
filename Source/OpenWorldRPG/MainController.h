// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainController.generated.h"

/**
 * 
 */

class UUserWidget;
class AMainCharacter;

UCLASS()
class OPENWORLDRPG_API AMainController : public APlayerController
{
	GENERATED_BODY()
public:

	AMainController();

	AMainCharacter* Main;

	/****** Main Hud *****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | MainHUD")
	TSubclassOf<UUserWidget> WMainHud;
	class UMainHud* MainHud;

	/***** Widget ******/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	TSubclassOf<UUserWidget> WInteractText;
	UUserWidget* InteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	TSubclassOf<UUserWidget> WInventory;
	UUserWidget* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Menu")
	TSubclassOf<UUserWidget> WDeathWidget;
	UUserWidget* DeathWidget;

	bool bIsInteractLootBox;
	bool bIsInteractCharacterLoot;
	bool bIsInventoryVisible;
	bool bIsCompassWidgetVisible;

	/********** Vehicle ********/
	bool bIsinCar;
	TWeakObjectPtr<class ANiceCar> Car;

protected:
	virtual void BeginPlay() override;

public:
	void ToggleInventory();

	void ControlInteractText(bool bIsInteract);

	void ControlCoreUsableWidget(bool bIsTPSMode);

	void CreateLootWidget();
	void ShowLootBoxWidget();
	void HideLootBoxWidget();

	void SetInputAndFocus(bool bIsShow);

	void Die();

	void Respawn();

	/*************Vehicle *********/
	void ToggleCar(class ANiceCar* vCar, FVector OutPos = FVector(0.f));
	void GetIntheCar(ANiceCar* vCar);
	void GetOuttheCar(FVector OutPos);//ANiceCar* Car);


	//void UseQuickSlotItem(EQuickSlotNumber QuickSlotNum);
};
