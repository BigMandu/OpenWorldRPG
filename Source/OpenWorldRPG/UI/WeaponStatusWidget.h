// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UWeaponStatusWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	int32 LeftAmmoMag = 0;
	int32 LeftAmmoTot = 0;
protected:
	virtual void NativeConstruct() override;
public:

	UPROPERTY(meta=(BindWidget))
	class UBorder* WeaponImage;

	//1 ÅºÃ¢¿¡ ³²Àº Åº °³¼ö
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LeftinMagazine;

	//Inventory¿¡ ³²Àº Åº °³¼ö
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LeftinInventory;

	class AWeapon* PlayerEquippedWeapon;

	void BindWeaponWidget(class AMainCharacter* MainChar);


	UFUNCTION()
	void UpdateWeaponImage(AWeapon* EquippedWeapon);
	UFUNCTION()
	void GetAmmoStatus(AWeapon* EquippedWeapon);

	/*void UpdateAmmoInMagazine(int32 LeftAmmo);
	void UpdateAmmoInInventory(int32 LeftAmmo);*/
	
	
};
