// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/GrenadeBase.h"
//#include "OpenWorldRPG/Item/GrenadeHandler.h"
#include "GrenadeFragment.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AGrenadeFragment : public AGrenadeBase//, public IGrenadeHandler
{
	GENERATED_BODY()
protected:
	virtual void BeginEffect() override;

public:

};
