// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "CoreIntelligence.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ACoreIntelligence : public AInteractable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = IntelligenceData)
	EIntelType Inteltype;

	virtual void Interaction(AActor* Actor) override;

	/*UPROPERTY(EditAnywhere, Category = IntelligenceData)
	UDataTable* DT_Intel;

	UPROPERTY(EditAnywhere, Category = IntelligenceData)
	struct FIntelligenceDataTable IntelligenceDataTableRow;*/
};
