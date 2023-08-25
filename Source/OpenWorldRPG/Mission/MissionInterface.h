// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MissionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMissionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLDRPG_API IMissionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	FString MissionUniqueID_Interface;
	//AActorclass로 형변환하여 런타임때 cost를 낭비하기 보단 스트링변수를 하나 더 추가하기로 결정함.
	FString CharacterUniqueID_Interface;

	virtual void ShowNotifyIcon() = 0;
	virtual void HideNotifyIcon() = 0;
	

};
