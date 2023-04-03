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

	//StrengthLevel = 5;
}

void UStatManagementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	StrengthLevelUp(StrengthLevel);
	

}


void UStatManagementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatManagementComponent::StrengthLevelUp(int32 SetLevel)
{
	UMyGameInstance* MyGameInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MyGameInst)
	{
		CurrentMAXStrengthStats = MyGameInst->GetStrengthStats(SetLevel);

		if (CurrentMAXStrengthStats)
		{
			StrengthLevel = SetLevel;
			//CurrentMAXStamina = CurrentMAXStrengthStats->Stamina;
			//CurrentMAXJumpZ = CurrentMAXStrengthStats->JumpZ;
			CurrentStat.Stamina = CurrentMAXStrengthStats->Stamina;
			//CurrentStat.JumpMaximum = CurrentMAXStrengthStats->JumpZ;

		}
	}
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
		UE_LOG(LogTemp, Warning, TEXT("Maximum Stamina : %f"), CurrentMAXStrengthStats->Stamina);
		UE_LOG(LogTemp, Warning, TEXT("Current Stamina : %f"), CurrentStat.Stamina);
		UE_LOG(LogTemp, Warning, TEXT("Current Stamina Ratio : %f"), GetStaminaRatio());
	}

	StaminaStatus = NewStatus;
}

void UStatManagementComponent::DamageApply(float Damage)
{
	TakeDamageSound();

	if (CurrentStat.Health - Damage <= 0.f)                           
	{
		OnHPZero.Broadcast();
		CurrentStat.Health = 0.f;

		return;
	}	
	CurrentStat.Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("StatManager::DamageApply// DamagePoint :  %f // CurrentHealth : %f"), Damage, CurrentStat.Health);
	OnHPChange.Broadcast();
	
}

void UStatManagementComponent::AddHPPoint(float RecoveryPoint)
{
	CurrentStat.Health += RecoveryPoint;
	UE_LOG(LogTemp,Warning,TEXT("StatManager::AddHPPoint// RecoveryPoint :  %f // CurrentHealth : %f"),RecoveryPoint, CurrentStat.Health);
	OnHPChange.Broadcast();
}
void UStatManagementComponent::AddStaminaPoint(float RecoveryPoint)
{

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
			if (CurrentStat.Stamina + DeltaStaminaGain < CurrentMAXStrengthStats->Stamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Recovery);
			}
			else
			{
				CurrentStat.Stamina = CurrentMAXStrengthStats->Stamina;
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
			if (CurrentStat.Stamina + DeltaStaminaGain >= CurrentMAXStrengthStats->Stamina)
			{
				CurrentStat.Stamina = CurrentMAXStrengthStats->Stamina;
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

float UStatManagementComponent::GetHealthRatio()
{
	return CurrentStat.Health / MaxHealth;
}
float UStatManagementComponent::GetStaminaRatio()
{
	return CurrentStat.Stamina / CurrentMAXStrengthStats->Stamina;
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
			Player->SpeakSound(Stam_ExhaustedSound);
		break;
		case EStaminaStatus::ESS_ExhaustedRecovery:
			Player->SpeakSound(Stam_ExhaustedRecoverySound);
		break;
		case EStaminaStatus::ESS_Recovery:
			Player->SpeakSound(nullptr);
			//Player->SpeakSound(Stam_RecoverySound);
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