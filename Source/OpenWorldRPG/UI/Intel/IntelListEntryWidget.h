// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "OpenworldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "IntelListEntryWidget.generated.h"

/**
 * 
 */

 //DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickIntel, EIntelType, IntelType)

UCLASS()
class OPENWORLDRPG_API UIntelListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (bindwidget))
	class UTextBlock* IntelName;

	UPROPERTY(meta = (bindwidget))
	class UButton* IntelButton;

	UPROPERTY()
	EIntelType IntelType;


	//FClickIntel ClickIntel;

	UFUNCTION()
	void ClickIntel();
};
