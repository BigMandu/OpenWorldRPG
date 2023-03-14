// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/ResourceStatusWidgetInInventory.h"
#include "Components/ProgressBar.h"

void UResourceStatusWidgetInInventory::NativeConstruct()
{
	Super::NativeConstruct();

	//안해도됨.
}

void UResourceStatusWidgetInInventory::BindStatManager(TWeakObjectPtr<UStatManagementComponent> StatWizardManager)
{
	if (StatWizardManager.IsValid())
	{
		StatManager = StatWizardManager.Get();

		StatManager->OnHPChange.AddDynamic(this, &UResourceStatusWidgetInInventory::UpdateHPRatio);
		StatManager->OnHPChange.AddDynamic(this, &UResourceStatusWidgetInInventory::UpdateCurrentHPNum);

		StatManager->OnStaminaChange.AddDynamic(this, &UResourceStatusWidgetInInventory::UpdateStamRatio);
		StatManager->OnStaminaChange.AddDynamic(this, &UResourceStatusWidgetInInventory::UpdateCurrentStamNum);

		UpdateHPRatio();
		UpdateStamRatio();

		UpdateCurrentHPNum();
		UpdateCurrentStamNum();
		UpdateMaxHPNum();
		UpdateMaxStamNum();
	}
}




void UResourceStatusWidgetInInventory::UpdateHPRatio()
{
	if (StatManager.IsValid())
	{
		HPProgress->SetPercent(StatManager->GetHealthRatio());
	}
}

void UResourceStatusWidgetInInventory::UpdateStamRatio()
{
	if (StatManager.IsValid())
	{
		StamProgress->SetPercent(StatManager->GetStaminaRatio());
	}
}


void UResourceStatusWidgetInInventory::UpdateCurrentHPNum()
{

}

void UResourceStatusWidgetInInventory::UpdateCurrentStamNum()
{

}

void UResourceStatusWidgetInInventory::UpdateMaxHPNum()
{

}

void UResourceStatusWidgetInInventory::UpdateMaxStamNum()
{

}
