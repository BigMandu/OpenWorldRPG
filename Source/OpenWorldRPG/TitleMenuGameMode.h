// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ATitleMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
	ATitleMenuGameMode(const FObjectInitializer& ObjectInitializer);
public:
	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidget)
	TSubclassOf<class UMainMenuWidget> WMainMenuWidget;
	UMainMenuWidget* MainMenuWidget;

	/**
	 * Initialize the game.
	 * The GameMode's InitGame() event is called before any other functions (including PreInitializeComponents() )
	 * and is used by the GameMode to initialize parameters and spawn its helper classes.
	 * @warning: this is called before actors' PreInitializeComponents.
	 */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PreInitializeComponents() override;
};
