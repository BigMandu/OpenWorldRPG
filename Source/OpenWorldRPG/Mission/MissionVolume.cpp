// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionVolume.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

// Sets default values
AMissionVolume::AMissionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissionVol = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("billboard"));

	NotifyIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("NotifyIcon"));
	
	
	SetRootComponent(MissionVol);
	billboard->SetupAttachment(GetRootComponent());
	NotifyIcon->SetupAttachment(GetRootComponent());
	
}


void AMissionVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MissionVol->OnComponentBeginOverlap.AddDynamic(this, &AMissionVolume::OverlapBegin);

	//NotifyIcon->SetWidgetSpace(EWidgetSpace::Screen);
	//NotifyIcon->SetDrawSize(FVector2D(50.f));
	NotifyIcon->SetVisibility(false);
	HideNotifyIcon();
}

// Called when the game starts or when spawned
void AMissionVolume::BeginPlay()
{
	Super::BeginPlay();
	PlayerCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));//Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	ThisActorsLocation = NotifyIcon->GetComponentLocation();//GetActorLocation();

	MissionUniqueID_Interface = MissionUniqueID;
}


void AMissionVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsShowing)
	{	
		APlayerCameraManager* PlayerCamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
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
				PlayerCon->GetViewportSize(ScreenX,ScreenY);

				FVector2D ScreenSizeVec = FVector2D(ScreenX,ScreenY);

				
				bool bIsBehind = false;

				UCustomSystemLibrary::CustomProjectionWorldToScreen(PlayerCon,ThisActorsLocation,ScreenPos,bIsBehind);
				UCustomSystemLibrary::AdjustProjectionScreenPosition(ScreenPos, ScreenSizeVec, WidgetSize, bIsBehind);
				

				//UE_LOG(LogTemp, Warning, TEXT("widgetScreenFINAL: %s"), *ScreenPos.ToString());

				NotWid->SetPositionInViewport(ScreenPos);
				
				
				
			}
			//FRotator NewWidgetRot = UKismetMathLibrary::FindLookAtRotation(WidgetWLocation,PlayerCamManager->GetCameraLocation());

			//FRotator AdjustRot = FRotator(0.f, NewWidgetRot.Yaw - 90.f, NewWidgetRot.Pitch * -1 + 90.f);
			//NotifyIcon->SetWorldRotation(NewWidgetRot);
		}
	}
}


void AMissionVolume::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			if (EvaluateMissionStatus(*Gmode))
			{
				if (Gmode->EvaluateMissionStatus(this))
				{
					//특정 Mission(CQB Training Mission)인 경우 해당 EVent를 끝낸다.
					if (MissionUniqueID.Equals(TEXT("BCM008")))
					{
						Gmode->EndCQBMission();
					}
					HideNotifyIcon();
				}
				
			}
		}
	}	
}

bool AMissionVolume::EvaluateMissionStatus(AOpenWorldRPGGameModeBase& T_gMode)
{
	if (MissionUniqueID.Equals(T_gMode.CurrentMissionInfo.InProgressMissionUnqID))
	{
		return true;
	}
	
	return false;
}


void AMissionVolume::ShowNotifyIcon()
{
	bIsShowing = true;

	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	//NotifyIcon->SetVisibility(true,true);
}

void AMissionVolume::HideNotifyIcon()
{
	bIsShowing = false;

	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::Collapsed);
		NotWid->RemoveFromViewport();
	}
	//NotifyIcon->SetVisibility(false);
}