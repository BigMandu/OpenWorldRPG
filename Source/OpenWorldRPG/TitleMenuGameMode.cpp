// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleMenuGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "OpenWorldRPG/UI/MainMenuWidget.h"

ATitleMenuGameMode::ATitleMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWid(TEXT("/Game/UI/WBP_MainMenu"));//.WBP_MainMenu"));
	if ( MainMenuWid.Succeeded() )
	{
		WMainMenuWidget = MainMenuWid.Class;
	}

	//DefaultPawnClass = nullptr;
}

void ATitleMenuGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName,Options,ErrorMessage);


	UE_LOG(LogTemp, Warning,TEXT("ATitleMenuGameMode::InitGame"));
	
}
	
void ATitleMenuGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("ATitleMenuGameMode::PreInitializeComponents"));

	if ( !WMainMenuWidget ) return;
		
	MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), WMainMenuWidget);

	if ( MainMenuWidget )
	{
		MainMenuWidget->AddToViewport();
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

		/*FInputModeUIOnly Inputmode;
		GetWorld()->GetFirstPlayerController()->SetInputMode(Inputmode);*/
	}
	
	
}