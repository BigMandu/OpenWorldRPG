// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/AI/MissionAICharacter.h"
#include "OpenWorldRPG/GameData/CharacterVoice/CharacterVoiceTable.h"
#include "OpenWorldRPG/GameData/CharacterVoice/CharacterVoiceTable.h"
#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"
#include "OpenWorldRPG/OpenWorldRPGGameModeBase.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/MainHud.h"

#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"




AMissionAICharacter::AMissionAICharacter() : Super()
{
	/*NotifySphere = CreateDefaultSubobject<USphereComponent>(TEXT("NotifySphere"));
	NotifySphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	NotifySphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);*/

	NotifyIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("NotifyIcon"));
	NotifyIcon->SetupAttachment(GetRootComponent());
	//NotifySphere->SetupAttachment(GetRootComponent());

	BeforePlayedAudioInfo = FBeforePlayedAudioInfo();

	//MissionUniqueID_Interface = MissionUniqueID;
}

void AMissionAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterUniqueID_Interface = ThisCharacterUniqID;
	//AI 일 때는 아래를 쓰지 않는다.
	//MissionUniqueID_Interface = MissionUniqueID;
	
	//Interaction을 할때도 재생, 현 재생이 끝나도 재생한다.
	AudioComp->OnAudioFinished.AddDynamic(this,&AMissionAICharacter::StartDialogue);

	
	WidgetSize = FVector2D(25.f);
	NotifyIcon->SetVisibility(false);	

	HideNotifyIcon();
	
}

void AMissionAICharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
}
void AMissionAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

				ThisActorsLocation = NotifyIcon->GetComponentLocation(); //GetActorLocation();

				UCustomSystemLibrary::CustomProjectionWorldToScreen(PlayerCon, ThisActorsLocation, ScreenPos, bIsBehind);
				UCustomSystemLibrary::AdjustProjectionScreenPosition(ScreenPos, ScreenSizeVec, WidgetSize, bIsBehind);

				NotWid->SetPositionInViewport(ScreenPos);
			}
		}
	}
}


void AMissionAICharacter::Interaction(AActor* Actor)
{
	if (UWorld* World = GetWorld())
	{
		if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(World)))
		{
			if (Gmode->EvaluateMissionStatus(this))
			{
				BeforePlayedAudioInfo = FBeforePlayedAudioInfo();
				StartDialogue();
			}
		}
	}
}


void AMissionAICharacter::StartDialogue()
{
	AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!Gmode) return;
	if(!PlayerCon) return;

	bool bIsActivated = false;

	for (auto row : CharacterVoiceTable->GetRowMap())
	{
		FCharacterVoiceTable* Row = (FCharacterVoiceTable*)row.Value;
		if (Row->MissionUniqueID == Gmode->GetCurrentMission().InProgressMissionUnqID)
		{
			//저장되어있는 마지막 play인포를 가져온다 , 기존에 한번도 재생하지 않았거나(playednumber가 0이라면),
			//기존 재생 ID와 table과 일치하고 기존 재생 ID의 audio number +1이 RowTable의 num과 같다면 그 Row의 data를 저장하고 Row의 audio, text를 세팅한다.
			if((BeforePlayedAudioInfo.playedNumber == 0) || (BeforePlayedAudioInfo.playedMissionUniqID == Row->MissionUniqueID && BeforePlayedAudioInfo.playedNumber+1 == Row->VoiceNum))
			{
				BeforePlayedAudioInfo = FBeforePlayedAudioInfo(Row->MissionUniqueID, Row->VoiceNum, Row->MaxVoiceNum);

				bool bIsBlueTeam = false;
				if (GetTeamType() == ETeamType::ETT_Blue_PlayerSide)
				{
					bIsBlueTeam = true;
				}

				AudioComp->SetSound(Row->Voice);
				AudioComp->Play();
				PlayerCon->MainHud->SetSubtitleText(Name, Row->VoiceText, bIsBlueTeam);

				if (Row->Act)
				{
					StartAct(Row->Act);
				}

				bIsActivated = true;

			}
		}

		//Activated가 됐다면 for문을 break한다.
		if (bIsActivated)
		{			
			break;
		}
	}


	/* 재생이 됐고,
	* MissionID의 음성개수와 Play된 음성개수가 같다면
	* Mission status를 Evaluate하고 mission을 update한다.
	*/
	if(bIsActivated)
	{
		//
		if(BeforePlayedAudioInfo.playedNumber >= BeforePlayedAudioInfo.MAXPlayNumber)
		{
			//호출이 됐다면 강제로 다음 mission으로 update한다.
			// 
			//Gmode->UpdateMissionProgress();

			if (Gmode->EvaluateMissionStatus(this))
			{
				HideNotifyIcon();
			}



		/*	if (bAlsoGiveMission)
			{
				Gmode->UpdateMissionToThis(GiveMissionUniqueID);
				HideNotifyIcon();
			}
			else
			{
				if (Gmode->EvaluateMissionStatus(this))
				{
					HideNotifyIcon();
				}
			}*/
		}
	}
	/* 마지막 음성 재생후 이 함수가 또 실행되면서
	* 아래 else statement를 실행하게 된다.
	* 
	* 만약 for문이 다 끝났는데, 실행을 못했따면
	* 더이상 해당 mission ID에 voice가 없다고 판단, subtitle을 초기화 시킨다.
	* 
	*/
	else
	{
		PlayerCon->MainHud->RemoveSubtitleText();
	}
}

void AMissionAICharacter::StartAct(UAnimMontage* act)
{
	PlayAnimMontage(act);
}


void AMissionAICharacter::ShowNotifyIcon()
{
	bIsShowing = true;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
	
void AMissionAICharacter::HideNotifyIcon()
{
	bIsShowing = false;
	if (UUserWidget* NotWid = NotifyIcon->GetUserWidgetObject())
	{
		NotWid->SetVisibility(ESlateVisibility::Collapsed);
		NotWid->RemoveFromViewport();
	}
}