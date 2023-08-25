// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/GameData/MyGameInstance.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Kismet/GameplayStatics.h"



UStatManagementComponent::UStatManagementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//InitializeComponent()를 사용하기 위해선 아래 boolean을 true로 해줘야함.
	bWantsInitializeComponent = true;

	MaxHealth = 100.f;
	Level = 1;
	
}

void UStatManagementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetLevel(Level);
}


void UStatManagementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatManagementComponent::SetLevel(int32 SetLevel)
{
	UMyGameInstance* MyGameInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MyGameInst)	
	{
		CurrentMAXStats = MyGameInst->GetLevelStats(SetLevel);
		
		if ( CurrentMAXStats )
		{
			Level = SetLevel;

			if(Level < 20 )
			{
				NextMAXStats = MyGameInst->GetLevelStats(Level+1);
			}

			CurrentStat.Stamina = CurrentMAXStats->Stamina;			
			CurrentStat.StaminaDrainRate = CurrentMAXStats->StaminaDrainRate;
			CurrentStat.AimInitrate = CurrentMAXStats->AimInitializeRate;
			CurrentStat.JumpMaximum = CurrentMAXStats->JumpZ;

			//레벨업을 하게되면 경험치를 0으로 초기화 시킨다.
			CurrentStat.ExperiencePoint = 0.f;
			

			OnLevelUp.Broadcast();
			UE_LOG(LogTemp,Warning,TEXT("UStatManagementComponent::SetLevel // Level Up! Level : %d"),Level);
			
		}
	}
}

void UStatManagementComponent::AddExpPoint(int32 ExpPoint)
{
	if(Level >= 20 ) return;
	int32 AdjustExpPoint = ExpPoint;

	//계산 편하게 하기 위해서.
	int32 ExpOver = CurrentStat.ExperiencePoint + ExpPoint;
	//Exppoint와 현재 경험치의 합이  레벨업에 필요한 총 경험치 보다 많으면
	//레벨업 이후 경험치에 포함 하기 위함.
	if ( ExpOver >= CurrentMAXStats->NextExp )
	{		
		AdjustExpPoint = ExpOver - CurrentMAXStats->NextExp;
		SetLevel(++Level);
	}

	CurrentStat.ExperiencePoint += AdjustExpPoint;
	OnExpPointChange.Broadcast();
}

void UStatManagementComponent::SetStaminaStatus(EStaminaStatus NewStatus)
{
	//새Status가 기존과 다르다면 그에 따른 Breath Sound를 출력하기 위해
	//새Status를 전달한다.
	if (StaminaStatus != NewStatus)
	{
		StaminaBreathSoundManager(NewStatus);
	}

	if (NewStatus == EStaminaStatus::ESS_BelowMinimum)
	{
		UE_LOG(LogTemp, Warning, TEXT("Maximum Stamina : %f"), CurrentMAXStats->Stamina);
		UE_LOG(LogTemp, Warning, TEXT("Current Stamina : %f"), CurrentStat.Stamina);
		UE_LOG(LogTemp, Warning, TEXT("Current Stamina Ratio : %f"), GetStaminaRatio());
	}

	StaminaStatus = NewStatus;
}

void UStatManagementComponent::DamageApply(float Damage, FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	TakeDamageSound();

	AOpenWorldRPGGameModeBase* GMode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	check(GMode)
	ABaseCharacter* BChar = Cast<ABaseCharacter>(GetOwner());
	check(BChar)
	const UDamageType* DmgType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();

	if(bIsGodMode) Damage = 0.f;

	
	//죽을 경우
	if (!bIsPendingKill && CurrentStat.Health - Damage <= 0.f)                          
	{
		//UDamageType* DmgType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();

		//Kill event가 연속 실행될 경우를 prevent하기 위해 bIsPendingKill bool을 true로 한다.
		//Actor에 붙은 component라 1회성으로 true로 변경하면 끝.
		bIsPendingKill = true;
		CurrentStat.Health = 0.f;
		
		//EventInstigator = BChar->GetDamageInstigator(EventInstigator, *DmgType);
		GMode->ProcessKillEvent(EventInstigator, BChar->GetController(),DamageEvent);
		
		OnHPChange.Broadcast();
		OnHPZero.Broadcast();

		return;
	}

	CurrentStat.Health -= Damage;
	GMode->ProcessHitEvent(EventInstigator, BChar->GetController(), DamageEvent);


	UE_LOG(LogTemp, Warning, TEXT("StatManager::DamageApply// DamagePoint :  %f // CurrentHealth : %f"), Damage, CurrentStat.Health);
	OnHPChange.Broadcast();
	
}

void UStatManagementComponent::AddHPPoint(float RecoveryPoint)
{
	float RecoveryAmount = CurrentStat.Health + RecoveryPoint;
	CurrentStat.Health = ( RecoveryAmount >= MaxHealth) ? MaxHealth : RecoveryAmount;
	
	UE_LOG(LogTemp,Warning,TEXT("StatManager::AddHPPoint// RecoveryPoint :  %f // CurrentHealth : %f"),RecoveryPoint, CurrentStat.Health);
	OnHPChange.Broadcast();
}

void UStatManagementComponent::AddStaminaPoint(float RecoveryPoint)
{
	float RecoveryAmount = CurrentStat.Stamina + RecoveryPoint;
	CurrentStat.Stamina = ( RecoveryAmount >= CurrentMAXStats->Stamina) ? CurrentMAXStats->Stamina : RecoveryAmount;

	OnStaminaChange.Broadcast();
}

//for Widget
void UStatManagementComponent::AddCurrentHPRecoveryPoint(float Points)
{
	TestHPRecPts += Points;
	UE_LOG(LogTemp, Warning, TEXT("StatManager::AddCurrentHPRecoveryPoint// HpRegen :  %f"), TestHPRecPts);
}

void UStatManagementComponent::RemoveCurrentHPRecoveryPoint(float Points)
{
	TestHPRecPts -= Points;
	UE_LOG(LogTemp, Warning, TEXT("StatManager::RemoveCurrentHPRecoveryPoint// HpRegen :  %f"), TestHPRecPts);
}

float UStatManagementComponent::GetHealthRatio()
{
	return CurrentStat.Health / MaxHealth;
}
float UStatManagementComponent::GetStaminaRatio()
{
	return CurrentStat.Stamina / CurrentMAXStats->Stamina;
}

void UStatManagementComponent::UpdateCurrentStats(FCharacterCurrentStat SavedStats)
{
	CurrentStat = SavedStats;
	OnHPChange.Broadcast();
	OnStaminaChange.Broadcast();
}




void UStatManagementComponent::StaminaManage(bool bIsSprintKeyDown)
{
	AMainCharacter* Player = Cast<AMainCharacter>(GetOwner());
	check(Player)
	//delta stamina drain = stamina drain rate * deltatime;
	//delta stamina gain = stamina recovery rate * deltatime;
	float deltatime = GetWorld()->GetDeltaSeconds();
	float DeltaStaminaDrain = CurrentStat.StaminaDrainRate * deltatime;
	float DeltaStaminaGain = CurrentStat.StaminaRecoveryRate * deltatime;
	
	

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bIsSprintKeyDown && (Player->bMoveRight || Player->bMoveForward))
		{
			if (CurrentStat.Stamina - DeltaStaminaDrain <= CurrentStat.StaminaDrainLine)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			else
			{
				CurrentStat.Stamina -= DeltaStaminaDrain;
			}
			Player->SetCharacterStatus(ECharacterStatus::EPS_Sprint);
		}
		else
		{
			if (CurrentStat.Stamina + DeltaStaminaGain < CurrentMAXStats->Stamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Recovery);
			}
			else
			{
				CurrentStat.Stamina = CurrentMAXStats->Stamina;
				Player->ClearSprintUpTimer();
			}

		}
	break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bIsSprintKeyDown && (Player->bMoveRight || Player->bMoveForward))
		{
			if (CurrentStat.Stamina - DeltaStaminaDrain <= 0.f)
			{
				CurrentStat.Stamina = 0.f;
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Player->SetCharacterStatus(ECharacterStatus::EPS_Normal);
			}
			else
			{
				CurrentStat.Stamina -= DeltaStaminaDrain;
				Player->SetCharacterStatus(ECharacterStatus::EPS_Sprint);
			}
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_Recovery);
		}
	break;
	case EStaminaStatus::ESS_Exhausted:
		if (bIsSprintKeyDown && (Player->bMoveRight || Player->bMoveForward))
		{
			CurrentStat.Stamina = 0.f;
			Player->SetCharacterStatus(ECharacterStatus::EPS_Normal);
			Player->UnSprint();
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovery);
		}
	break;
	case EStaminaStatus::ESS_ExhaustedRecovery:
		if (bIsSprintKeyDown && (Player->bMoveRight || Player->bMoveForward))
		{
			SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
		}
		else
		{
			if (CurrentStat.Stamina + DeltaStaminaGain >= CurrentStat.StaminaDrainLine)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Recovery);
			}
			else
			{
				CurrentStat.Stamina += DeltaStaminaGain;
				Player->SetCharacterStatus(ECharacterStatus::EPS_Normal);
			}
		}
	break;
	case EStaminaStatus::ESS_Recovery:
		if (bIsSprintKeyDown && (Player->bMoveRight || Player->bMoveForward))
		{
			if (CurrentStat.Stamina >= CurrentStat.StaminaDrainLine)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			else
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
		}
		else
		{
			if (CurrentStat.Stamina + DeltaStaminaGain >= CurrentMAXStats->Stamina)
			{
				CurrentStat.Stamina = CurrentMAXStats->Stamina;
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Player->ClearSprintUpTimer();
			}
			else
			{
				CurrentStat.Stamina += DeltaStaminaGain;
			}
			Player->SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}
	break;
	}

	OnStaminaChange.Broadcast();
}



/**************** Sound *****************/
//Only MainCharacter.
void UStatManagementComponent::StaminaBreathSoundManager(EStaminaStatus CurrentStatus)
{
	AMainCharacter* Player = Cast<AMainCharacter>(GetOwner());
	check(Player)

	switch (CurrentStatus)
	{
		case EStaminaStatus::ESS_BelowMinimum:
			Player->SpeakSound(Stam_BelowMinimumSound);
		break;
		case EStaminaStatus::ESS_Exhausted:
			/*Player->SpeakSound(Stam_ExhaustedSound);*/
		break;
		case EStaminaStatus::ESS_ExhaustedRecovery:
			Player->SpeakSound(Stam_ExhaustedSound);
			//Player->SpeakSound(Stam_ExhaustedRecoverySound);
		break;
		//Recovery할때 소리를 내는게 뭔가 어색함. shift를 뗏다 눌렀다 하면 너무 이상함.
		//그냥 이 구간은 소리를 빼는게 낫다고 판단함.
		case EStaminaStatus::ESS_Recovery:
			//Player->SpeakSound(Stam_ExhaustedRecoverySound);
			//Player->SpeakSound(Stam_RecoverySound);
			Player->SpeakSound(nullptr);
		break;

		default:
		break;
	}
}

//All Character.
void UStatManagementComponent::TakeDamageSound()
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(GetOwner());
	check(BChar)

	if (HurtSound)
	{
		BChar->SpeakSound(HurtSound);
	}

}