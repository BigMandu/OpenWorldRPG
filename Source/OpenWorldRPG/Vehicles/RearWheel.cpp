// Fill out your copyright notice in the Description page of Project Settings.


#include "RearWheel.h"
#include "TireConfig.h"
#include "UObject/ConstructorHelpers.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

URearWheel::URearWheel()
{
	ShapeRadius = 55.f;
	ShapeWidth = 10.f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;

	//Suspension forces setup
	SuspensionForceOffset = -4.0f;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;

	static ConstructorHelpers::FObjectFinder<UTireConfig> TireCon(TEXT("/Game/Vehicles/VehicleRearTireConfig.VehicleRearTireConfig"));
	TireConfig = TireCon.Object;
}


void URearWheel::SettingWheel(FVehicleDataTable Data)
{
	ShapeRadius = Data.WheelRadius;
	ShapeWidth = Data.WheelWidth;
	SteerAngle = 0.f;
	
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS