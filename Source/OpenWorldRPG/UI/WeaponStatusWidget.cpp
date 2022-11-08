// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "Components/TextBlock.h"

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
	MainChar->OnChangeWeapon.AddDynamic(this,&UWeaponStatusWidget::UpdateWeaponImage);
	MainChar->OnGetAmmo.AddDynamic(this, &UWeaponStatusWidget::GetAmmoStatus);
}

void UWeaponStatusWidget::UpdateWeaponImage(AWeapon* EquippedWeapon)
{
	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWeaponStatusWidget::UpdateWeaponImage / EquipWeapon : %s"), *EquippedWeapon->GetFName().ToString());
	}
}


void UWeaponStatusWidget::GetAmmoStatus(AWeapon* EquippedWeapon)
{
	if(EquippedWeapon)
	{
		LeftAmmoTot = EquippedWeapon->CntAmmoSameType;
		LeftAmmoMag = EquippedWeapon->AmmoLeftInMag; //Cast<UWeaponPDA>(EquippedWeapon->ItemSetting.DataAsset)->WeaponStat.AmmoPerMag - EquippedWeapon->ConsumeAmmoCnt;

		FText MagazineAmmo = FText::FromString(FString::FromInt(LeftAmmoMag));
		FText TotalSameTypeAmmo = FText::FromString(FString::FromInt(LeftAmmoTot));
	
		LeftinMagazine->SetText(MagazineAmmo);
		LeftinInventory->SetText(TotalSameTypeAmmo);
	}
}