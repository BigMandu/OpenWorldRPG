// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "Components/TextBlock.h"

//#define LOCTEXT_NAMESPACE "WeaponWidget"

void UWeaponStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*UpdateAmmoInMagazine(1);
	UpdateAmmoInInventory(1);*/
	UpdateWeaponImage(nullptr);
}

/*
* BaseChar에 Declare를 했다.
* broadcast 위치는
* OnGetAmmo = Weapon::UseAmmo, AWeapon::Reload, AMainCharacter::ChangeWeapon, AMainCharacter::TabKeyDown
* OnChangeWeapon = AMainCharacter::ChangeWeapon, 
*/
void UWeaponStatusWidget::BindWeaponWidget(AMainCharacter* MainChar)
{
	Player = MainChar;
	Player->OnChangeWeapon.AddDynamic(this,&UWeaponStatusWidget::UpdateWeaponImage);
	Player->OnGetAmmo.AddDynamic(this, &UWeaponStatusWidget::GetAmmoStatus);
	Player->OnChangeMode.AddDynamic(this, &UWeaponStatusWidget::UpdateFiringMode);
}


void UWeaponStatusWidget::UpdateWeaponImage(AWeapon* EquippedWeapon)
{
	if (EquippedWeapon)
	{
		WeaponImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		WeaponImage->SetBrushFromTexture(EquippedWeapon->WeaponDataAsset->WeaponVectorIMG);
	}
	else
	{
		WeaponImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}

void UWeaponStatusWidget::GetAmmoStatus(AWeapon* EquippedWeapon)
{
	if(EquippedWeapon)
	{
		LeftAmmoTot = Player->GetTotalNumberofSameTypeAmmo(); //EquippedWeapon->CntAmmoSameType;
		LeftAmmoMag = EquippedWeapon->AmmoLeftInMag; //Cast<UWeaponPDA>(EquippedWeapon->ItemSetting.DataAsset)->WeaponStat.AmmoPerMag - EquippedWeapon->ConsumeAmmoCnt;


		FFormatNamedArguments Args;
		Args.Add(TEXT("InMag"),FText::AsNumber(LeftAmmoMag));
		Args.Add(TEXT("InvAmmo"),FText::AsNumber(LeftAmmoTot));

		AmmoText->SetText(FText::Format(NSLOCTEXT("WeaponWidgetNS", "AmmoText", "{InMag}   /   {InvAmmo}"), Args));

		//FText MagazineAmmo = FText::FromString(FString::FromInt(LeftAmmoMag));
		//FText TotalSameTypeAmmo = FText::FromString(FString::FromInt(LeftAmmoTot));
	
		
		//LeftinMagazine->SetText(MagazineAmmo);
		//LeftinInventory->SetText(TotalSameTypeAmmo);
	}
}

void UWeaponStatusWidget::UpdateFiringMode(AWeapon* EquippedWeapon)
{
	FText ModeText = FText::FromString(TEXT(""));
	if (EquippedWeapon)
	{
		
		switch (EquippedWeapon->WeaponFiringMode)
		{
		case EWeaponFiringMode::EWFM_SemiAuto:
			ModeText = FText::FromString(TEXT("SemiAuto"));
			UE_LOG(LogTemp, Warning, TEXT("SemiAuto"));
			break;
		case EWeaponFiringMode::EWFM_Burst:
			ModeText = FText::FromString(TEXT("Burst"));
			UE_LOG(LogTemp, Warning, TEXT("Burst"));
			break;
		case EWeaponFiringMode::EWFM_FullAuto:
			ModeText = FText::FromString(TEXT("FullAuto"));
			UE_LOG(LogTemp, Warning, TEXT("FullAuto"));
			break;
		case EWeaponFiringMode::EWFM_Safe:
			ModeText = FText::FromString(TEXT("Safe"));
			UE_LOG(LogTemp, Warning, TEXT("Safe"));
			break;
		}
	}
	FiringModeText->SetText(ModeText);
}


#undef LOCTEXT_NAMESPACE