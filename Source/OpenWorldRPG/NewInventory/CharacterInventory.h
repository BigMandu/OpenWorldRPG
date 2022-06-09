// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInventory.generated.h"

/**
 * 
 */
class ABaseCharacter;
class UNewInventoryGrid;
class UNewInventoryComponent;

UCLASS()
class OPENWORLDRPG_API UCharacterInventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UNewInventoryGrid* PocketWidget;

	UPROPERTY(meta = (BindWidget))
	UNewInventoryGrid* SecureBoxWidget;

	UNewInventoryComponent* PocketInvComp;
	UNewInventoryComponent* SecureBoxInvComp;

	virtual bool Initialize() override;
	void InitializeInventory(ABaseCharacter* BChar);
};
