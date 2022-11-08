// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CharacterStatusWidget.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "Components/ProgressBar.h"

void UCharacterStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHPRatio();
	UpdateStaminaRatio();
}

void UCharacterStatusWidget::BindStatManager(UStatManagementComponent* NewManageComponent)
{
	if (NewManageComponent)
	{
		StatManger = NewManageComponent;
		StatManger->OnStaminaChange.AddDynamic(this,&UCharacterStatusWidget::UpdateStaminaRatio);
		StatManger->OnStaminaChange.AddDynamic(this,&UCharacterStatusWidget::UpdateHPRatio);

		UpdateHPRatio();
		UpdateStaminaRatio();

	}
}

void UCharacterStatusWidget::UpdateHPRatio()
{
	if (StatManger)
	{
		HPBar->SetPercent(StatManger->GetHealthRatio());
	}
}

void UCharacterStatusWidget::UpdateStaminaRatio()
{
	if(StatManger)
	{
		switch(StatManger->GetStaminaStatus())
		{
		case EStaminaStatus::ESS_BelowMinimum:
			//UE_LOG(LogTemp,Warning,TEXT("CharStatusWidget Below"));
		StaminaBar->SetFillColorAndOpacity(BelowMinimumStamBarColor);
		break;
		case EStaminaStatus::ESS_ExhaustedRecovery:
			//UE_LOG(LogTemp, Warning, TEXT("CharStatusWidget ExhRecov"));
			StaminaBar->SetFillColorAndOpacity(ExhaustedRecoveryStamBarColor);
		break;
		default:
			//UE_LOG(LogTemp, Warning, TEXT("CharStatusWidget Def"));
			StaminaBar->SetFillColorAndOpacity(DefaultStamBarColor);//DefaultStamBarColor;
		break;
		}

		StaminaBar->SetPercent(StatManger->GetStaminaRatio());

	}
	
}