// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Interactable.h"
#include "Cheat_Level.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ACheat_Level : public AInteractable
{
	GENERATED_BODY()
public:
	virtual void Interaction(AActor* Actor) override;
};
