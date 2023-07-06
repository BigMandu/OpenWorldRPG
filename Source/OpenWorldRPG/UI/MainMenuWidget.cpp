// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "OpenWorldRPG/GameData/MyGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	
	/* bind click */
	NewGame->OnClicked.AddDynamic(this, &UMainMenuWidget::Click_Newgame);
	Continue->OnClicked.AddDynamic(this, &UMainMenuWidget::Click_Continue);
	Option->OnClicked.AddDynamic(this, &UMainMenuWidget::Click_Option);
	Credit->OnClicked.AddDynamic(this, &UMainMenuWidget::Click_Credit);
	Exit->OnClicked.AddDynamic(this, &UMainMenuWidget::Click_Exit);

	//NewGame->OnHovered.AddDynamic(this, &UMainMenuWidget::Hoverbutton);
	//Continue->OnHovered.AddDynamic(this, &UMainMenuWidget::Hoverbutton);
	//Option->OnHovered.AddDynamic(this, &UMainMenuWidget::Hoverbutton);
	//Credit->OnHovered.AddDynamic(this, &UMainMenuWidget::Hoverbutton);
	//Exit->OnHovered.AddDynamic(this, &UMainMenuWidget::Hoverbutton);

	/*NewGame->OnUnhovered.AddDynamic(this, &UMainMenuWidget::UnHoverbutton);
	Continue->OnUnhovered.AddDynamic(this, &UMainMenuWidget::UnHoverbutton);
	Option->OnUnhovered.AddDynamic(this, &UMainMenuWidget::UnHoverbutton);
	Credit->OnUnhovered.AddDynamic(this, &UMainMenuWidget::UnHoverbutton);
	Exit->OnUnhovered.AddDynamic(this, &UMainMenuWidget::UnHoverbutton);*/

	
	UWorld* world = GetWorld();
	if ( world )
	{
		world->GetTimerManager().SetTimer(GetControlHandle,this, &UMainMenuWidget::GetControllerDelayed,0.1f, false, 0.1f);
	}

	CheckContinueButtonEnabled();
	
}

void UMainMenuWidget::CheckContinueButtonEnabled()
{
	FString SlotPlName = TEXT("SavePlayer");
	FString SlotWoName = TEXT("SaveWorld");
	
	int32 Index = 0;

	if ( !UGameplayStatics::DoesSaveGameExist(SlotPlName, Index)
	|| !UGameplayStatics::DoesSaveGameExist(SlotWoName, Index))
	{
		Continue->SetVisibility(ESlateVisibility::Collapsed);
		Continue->SetIsEnabled(false);
	}
}

void UMainMenuWidget::GetControllerDelayed()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::GetControllerDelayed"));
	APlayerController* PCon = GetWorld()->GetFirstPlayerController();//UGameplayStatics::GetPlayerController(world, 0);
	if ( PCon )
	{
		UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::GetControllerDelayed // Success GetPcon "));
		FInputModeGameAndUI Inputmode;
		 
		PCon->SetInputMode(Inputmode);
		PCon->SetShowMouseCursor(true);
	}
}

void UMainMenuWidget::Click_Newgame()//FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	//save file 삭제후 초기 level 열기
	UE_LOG(LogTemp,Warning,TEXT("UMainMenuWidget::Click_Newgame"));

	FString LevelPath = "/Game/Maps_Normal/TrainingMap";

	//for test
	//FString LevelPath = "/Game/Maps/MainMap";

	UMyGameInstance* Inst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if ( Inst )
	{
		Inst->PlayLoadingScreen();
		//Inst->Show Loading Scren()
	}
	UGameplayStatics::OpenLevel(GetWorld(),FName(*LevelPath));
}

void UMainMenuWidget::Click_Continue()
{
	//save file 검증
	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::Click_Continue"));
}

void UMainMenuWidget::Click_Option()
{
	//option widget 생성
}

void UMainMenuWidget::Click_Credit()
{
	//credit widget 생성
}

void UMainMenuWidget::Click_Exit()
{
	//종료
}



void UMainMenuWidget::Hoverbutton()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::Hoverbutton"));
	

}

void UMainMenuWidget::UnHoverbutton()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::UnHoverbutton"));
}