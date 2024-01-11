// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "OpenWorldRPG/GameData/MyGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	
	/* bind click */
	NewGameBtn->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::Click_Newgame);
	ContinueBtn->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::Click_Continue);
	OptionBtn->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::Click_Option);
	CreditBtn->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::Click_Credit);
	ExitBtn->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::Click_Exit);

	
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

	//if save file is not exist, hide Continue button.
	if ( !UGameplayStatics::DoesSaveGameExist(SlotPlName, Index)
	|| !UGameplayStatics::DoesSaveGameExist(SlotWoName, Index))
	{
		ContinueBtn->SetVisibility(ESlateVisibility::Collapsed);
		ContinueBtn->SetIsEnabled(false);
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

	UMyGameInstance* MyGameInst = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (!MyGameInst) return;

	//이미 있다면 추가 생성은 하지 않고 return한다.
	if (OptionWidget.IsValid()) return;

	OptionWidget = MyGameInst->CreateOptionWidget();

	if (OptionWidget.IsValid())
	{
		OptionWidget->OnRemoveWidget.AddUniqueDynamic(this, &UMainMenuWidget::RemoveAdditionalWidget);
		OptionWidget->AddToViewport(9);

		//when showing option widget, hide this widget.
		SetVisibility(ESlateVisibility::Collapsed);
		
	}
}

void UMainMenuWidget::Click_Credit()
{
	//credit widget 생성
}

void UMainMenuWidget::Click_Exit()
{
	//종료
}


void UMainMenuWidget::RemoveAdditionalWidget()
{
	OptionWidget.Reset();
	SetVisibility(ESlateVisibility::Visible);
}