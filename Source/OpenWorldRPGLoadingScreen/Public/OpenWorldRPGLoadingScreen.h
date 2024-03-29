// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/** Module interface for this game's loading screens */
class IOpenWorldRPGLoadingScreenModule : public IModuleInterface
{
public:
	/** Loads the module so it can be turned on */
	static inline IOpenWorldRPGLoadingScreenModule& Get()
	{
		
		return FModuleManager::LoadModuleChecked<IOpenWorldRPGLoadingScreenModule>("OpenWorldRPGLoadingScreen");
	}

	/** Kicks off the loading screen for in game loading (not startup) */
	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) = 0;

	/** Stops the loading screen */
	virtual void StopInGameLoadingScreen() = 0;
};
