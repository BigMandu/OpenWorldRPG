// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/ResourceStatusWidget.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "Components/ProgressBar.h"

void UResourceStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//아래 함수호출 안해도됨. 어차피 Statmanager가 없어서 실행 안됨.
	UpdateHPRatio();
	UpdateStaminaRatio();
}

void UResourceStatusWidget::BindStatManager(UStatManagementComponent* NewManageComponent)
{
	if (NewManageComponent)
	{
		StatManger = NewManageComponent;
		StatManger->OnStaminaChange.AddDynamic(this,&UResourceStatusWidget::UpdateStaminaRatio);
		StatManger->OnHPChange.AddDynamic(this,&UResourceStatusWidget::UpdateHPRatio);

		UpdateHPRatio();
		UpdateStaminaRatio();

	}
}

void UResourceStatusWidget::UpdateHPRatio()
{
	if (StatManger)
	{
		HPBar->SetPercent(StatManger->GetHealthRatio());
	}
}

void UResourceStatusWidget::UpdateStaminaRatio()
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