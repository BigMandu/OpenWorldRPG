// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterLootWidget.generated.h"

/**
 * 
 */
class UEquipWidget;
class UNewInventoryGrid;
class UCharacterInventoryWidget;

class ABaseCharacter;


UCLASS()
class OPENWORLDRPG_API UCharacterLootWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UEquipWidget* EquipWidget;
	UPROPERTY(meta = (BindWidget))
	UCharacterInventoryWidget* CharInvWidget;

	virtual bool Initialize() override;
	void InitCharLootWidget(ABaseCharacter* DeadChar);

};
