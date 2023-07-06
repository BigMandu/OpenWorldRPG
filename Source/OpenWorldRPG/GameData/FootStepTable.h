#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FootStepTable.generated.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS


USTRUCT(BlueprintType)
struct FFootStepTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	class UPhysicalMaterial* PhysMat;
	UPROPERTY(EditAnywhere)
	class USoundCue* StepSound;
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS