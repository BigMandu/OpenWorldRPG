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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	UUserWidget* InteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	TSubclassOf<UUserWidget> WInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	UUserWidget* Inventory;

	
	bool bIsInteractLootBox;
	bool bIsInteractCharacterLoot;
	bool bIsInventoryVisible;

protected:
	virtual void BeginPlay() override;

public:
	void ToggleInventory();

	void ControlInteractText(bool bIsInteract);

	void CreateLootWidget();
	void ShowLootBoxWidget();
	void HideLootBoxWidget();

	void SetInputAndFocus(bool bIsShow);

	//void UseQuickSlotItem(EQuickSlotNumber QuickSlotNum);
};
