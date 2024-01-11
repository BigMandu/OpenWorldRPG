// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionDoor.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"
#include "Components/WidgetComponent.h"

AMissionDoor::AMissionDoor(): Super()
{
	PrimaryActorTick.bCanEverTick = true;
	NotifyIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("NotifyIcon"));
	NotifyIcon->SetupAttachment(GetRootComponent());

}

void AMissionDoor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	bCanNotInteractable = false;

	WidgetSize = FVector2D(25.f);
	NotifyIcon->SetVisibility(false);

	HideNotifyIcon();
}

void AMissionDoor::BeginPlay()
{
	Super::BeginPlay();

	PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController()); //Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 

	MissionUniqueID_Interface = MissionUniqueID;
}

void AMissionDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsShowing)
	{
		APlayerCameraManager* PlayerCamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (PlayerCamManager)
		{
			if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
			{
				if (NotWid->IsInViewport() == false)
				{
					NotWid->AddToViewport();
					NotWid->SetDesiredSizeInViewport(WidgetSize);
					NotWid->SetAlignmentInViewport(FVector2D(0.5f));
				}

				//위젯이 Viewport에 표시될 위치
				FVector2D ScreenPos;

				//Viewport의 size를 vector로 묶는다.
				int32 ScreenX;
				int32 ScreenY;
				PlayerCon->GetViewportSize(ScreenX, ScreenY);
				FVector2D ScreenSizeVec = FVector2D(ScreenX, ScreenY);
				bool bIsBehind = false;

				ThisActorsLocation = GetActorLocation();// NotifyIcon->GetComponentLocation();

				UCustomSystemLibrary::CustomProjectionWorldToScreen(PlayerCon, ThisActorsLocation, ScreenPos, bIsBehind);
				UCustomSystemLibrary::AdjustProjectionScreenPosition(ScreenPos, ScreenSizeVec, WidgetSize, bIsBehind);

				NotWid->SetPositionInViewport(ScreenPos);
			}
		}
	}
}

void AMissionDoor::Interaction(AActor* Actor)
{
	if(bIsOpen) return;
	if(bIsLocked) return;
	
	if (UWorld* World = GetWorld())
	{
		if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(World)))
		{
			if (bStartMissionAgain && Gmode->EvaluateMissionStatus(this))
			{
				Gmode->StartCQBMission();
				
				HideNotifyIcon();

			}
			else if (Gmode->EvaluateMissionStatus(this))
			{
				HideNotifyIcon();
			}
		}
	}

	Super::Interaction(Actor);
}

void AMissionDoor::ShowNotifyIcon()
{
	bIsShowing = true;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AMissionDoor::HideNotifyIcon()
{
	bIsShowing = false;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::Collapsed);
		NotWid->RemoveFromViewport();
	}
}