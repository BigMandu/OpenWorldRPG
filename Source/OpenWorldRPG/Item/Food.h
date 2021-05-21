// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Food.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AFood : public AItem
{
	GENERATED_BODY()
public:
	AFood();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	float Heal;

	virtual void Interact() override;//_Implementation() override;

	virtual void Pickup() override;

	virtual void Use(class ACharacter* Character) override;

	
};
