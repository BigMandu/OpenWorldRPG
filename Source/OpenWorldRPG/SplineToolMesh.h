// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SplineTool.h"
#include "SplineToolMesh.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ASplineToolMesh : public ASplineTool
{
	GENERATED_BODY()

public:
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
};
