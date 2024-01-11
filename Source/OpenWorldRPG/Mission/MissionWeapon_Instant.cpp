// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionWeapon_Instant.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

AMissionWeapon_Instant::AMissionWeapon_Instant(): Super()
{
	NotifySphere = CreateDefaultSubobject<USphereComponent>(TEXT("NotifySphere"));
	NotifySphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	NotifySphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	NotifyIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("NotifyIcon"));
	

	NotifyIcon->SetupAttachment(GetRootComponent());
	NotifySphere->SetupAttachment(GetRootComponent());

}

void AMissionWeapon_Instant::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	bCanNotInteractable = false;

	NotifySphere->SetSphereRadius(90.f);	
	NotifySphere->OnComponentBeginOverlap.AddDynamic(this, &AMissionWeapon_Instant::OverlapMissionObj);
	NotifySphere->OnComponentEndOverlap.AddDynamic(this, &AMissionWeapon_Instant::EndOverlapMissionObj);

	WidgetSize = FVector2D(25.f);
	NotifyIcon->SetVisibility(false);

	HideNotifyIcon();
}

void AMissionWeapon_Instant::BeginPlay()
{
	Super::BeginPlay();

	PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController()); //Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
	
	MissionUniqueID_Interface = MissionUniqueID;
}

void AMissionWeapon_Instant::Tick(float DeltaSeconds)
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

void AMissionWeapon_Instant::Interaction(AActor* Actor)
{
	Super::Interaction(Actor);

	if (UWorld* World = GetWorld())
	{
		if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(World)))
		{
			if (Gmode->EvaluateMissionStatus(this))
			{
				HideNotifyIcon();
			}
		}
	}

}


void AMissionWeapon_Instant::OverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		
		SetOutline();
	}
}

void AMissionWeapon_Instant::EndOverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		UnsetOutline();
	}
}

void AMissionWeapon_Instant::ShowNotifyIcon()
{
	bIsShowing = true;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AMissionWeapon_Instant::HideNotifyIcon()
{
	bIsShowing = false;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::Collapsed);
		NotWid->RemoveFromViewport();
	}
}