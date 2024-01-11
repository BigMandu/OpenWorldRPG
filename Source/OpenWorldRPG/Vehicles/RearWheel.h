// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "OpenWorldRPG/GameData/VehicleDataTable.h"
#include "RearWheel.generated.h"

/**
 * 
 */
PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS()
class OPENWORLDRPG_API URearWheel : public UVehicleWheel
{
	GENERATED_BODY()
public:
	URearWheel();
	void SettingWheel(FVehicleDataTable Data);
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS