// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FStrengthStats : public FTableRowBase
{
	GENERATED_BODY()
public:	
	FStrengthStats() :StrengthLevel(1), Stamina(100.f), JumpZ(400.f) {}

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Stats | Strength")
	int32 StrengthLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Strength")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Strength")
	float JumpZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Strength")
	float NextExp;
};

UCLASS()
class OPENWORLDRPG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();

	UPROPERTY()
	class UDataTable* STRLevelDataTable;

	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;
	FStrengthStats* GetStrengthStats(int32 Level);
};
