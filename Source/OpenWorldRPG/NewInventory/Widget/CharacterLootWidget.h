// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterLootWidget.generated.h"

/**
 * 
 */



UCLASS()
class OPENWORLDRPG_API UCharacterLootWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UEquipWidget* EquipWidget;
	UPROPERTY(meta = (BindWidget))
	class UEquipStorageWidget* EquipStorageWidget;

	virtual bool Initialize() override;
	void InitCharLootWidget(class ABaseCharacter* DeadChar);

};
