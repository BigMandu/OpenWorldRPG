// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontWheel.h"
#include "TireConfig.h"
#include "UObject/ConstructorHelpers.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UFrontWheel::UFrontWheel()
{
	ShapeRadius = 55.f;
	ShapeWidth = 10.f;
	bAffectedByHandbrake = false;
	SteerAngle = 70.f;

	//Suspension forces setup
	SuspensionForceOffset = -4.0f;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;

	//UTireConfig TireConfig
	static ConstructorHelpers::FObjectFinder<UTireConfig> TireCon(TEXT("/Game/Vehicles/VehicleFrontTireConfig.VehicleFrontTireConfig"));
	TireConfig = TireCon.Object;

}

void UFrontWheel::SettingWheel(FVehicleDataTable Data)
{
	ShapeRadius = Data.WheelRadius;
	ShapeWidth = Data.WheelWidth;
	SteerAngle = Data.SteerAngle;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS