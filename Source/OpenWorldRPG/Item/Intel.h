// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "Intel.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AIntel : public AInteractable
{
	GENERATED_BODY()
public:
	virtual void Interaction(AActor* Actor) override;

	/*virtual void SetOutline() override;
	virtual void UnsetOutline() override;*/

};
