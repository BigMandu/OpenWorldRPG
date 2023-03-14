// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontWheel.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UFrontWheel::UFrontWheel()
{
	ShapeRadius = 55.f;
	ShapeWidth = 10.f;
	bAffectedByHandbrake = false;
	SteerAngle = 70.f;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS