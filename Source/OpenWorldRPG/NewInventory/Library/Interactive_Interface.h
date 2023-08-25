// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactive_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractive_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

class OPENWORLDRPG_API IInteractive_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Force implement function.
	virtual void Interaction(class AActor* Actor) = 0;// { return nullptr; }
	virtual void SetOutline() = 0;
	virtual void UnsetOutline() = 0;



	FOnInteract OnInteract;
};
