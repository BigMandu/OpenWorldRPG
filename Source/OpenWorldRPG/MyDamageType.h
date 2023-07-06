// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "MyDamageType.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OPENWORLDRPG_API UMyDamageType : public UDamageType
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Display)
	class UTexture* KillIcon;

	UPROPERTY(EditDefaultsOnly, Category = Display)
	FString DamageText;

	UPROPERTY(EditDefaultsOnly, Category = ForceFeedback)
	class UForceFeedbackEffect* HitForceFB;

	UPROPERTY(EditDefaultsOnly, Category = ForceFeedback)
	UForceFeedbackEffect* KilledForceFB;
};
