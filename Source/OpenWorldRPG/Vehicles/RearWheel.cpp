// Fill out your copyright notice in the Description page of Project Settings.


#include "RearWheel.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

URearWheel::URearWheel()
{
	ShapeRadius = 55.f;
	ShapeWidth = 10.f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS