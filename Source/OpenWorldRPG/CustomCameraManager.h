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

	/**
	 * Performs per-tick camera update. Called once per tick after all other actors have been ticked.
	 * Non-local players replicate the POV if bUseClientSideCameraUpdates is true.
	 */
	virtual void UpdateCamera(float DeltaTime) override;
};
