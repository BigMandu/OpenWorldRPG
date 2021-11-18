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

	/***** Widget ******/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	TSubclassOf<UUserWidget> WInteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	UUserWidget* InteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	TSubclassOf<UUserWidget> WInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	UUserWidget* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	TSubclassOf<UUserWidget> WInventoryItems;

	/* Loot Box TEST */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | LootBox")
	TSubclassOf<UUserWidget> WLootBoxInvWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | LootBox")
	UUserWidget* LootBoxInvWidget;

	bool bIsInventoryVisible;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetFunction | Interact")
	void ShowInteractText();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetFunction | Interact")
	void HideInteractText();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetFunction | Inventory")
	void ShowInventory();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetFunction | Inventory")
	void HideInventory();

	void ToggleInventory();

	void CreateLootWidget();
	void ShowLootBox();
	void HideLootBox();
};
