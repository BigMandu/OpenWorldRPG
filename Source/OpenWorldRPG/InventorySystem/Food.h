// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Food.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UFood : public UItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	float Heal = 10.f;

protected:
	virtual void Use(class AMainCharacter* Character) override;
	
};
