// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CustomCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ACustomCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	ACustomCameraManager(const FObjectInitializer& ObjectInitializer);
};
