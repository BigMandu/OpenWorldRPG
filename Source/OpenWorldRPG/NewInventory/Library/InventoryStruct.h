// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryStruct.generated.h"

USTRUCT(BlueprintType)
struct FLine
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D End;
};

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;

	FTile()
	{
		X = 0, Y = 0;
	}
	FTile(int32 varX, int32 varY)
	{
		X = varX; Y = varY;
	}
};

USTRUCT(Atomic, BlueprintType)
struct FItemSetting
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCustomPDA* DataAsset;

	//아래 3개는 아직 쓰이지 않음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count;

	UPROPERTY()
	int32 TopLeftIndex;

	FItemSetting()
	{
		DataAsset = nullptr;
		Count = 1;
		TopLeftIndex = 0;
	}

	FItemSetting(UCustomPDA* Var_PDA, int32 Var_Cnt, int32 Var_TpLeftInd)
	{
		DataAsset = Var_PDA;
		Count = Var_Cnt;
		TopLeftIndex = Var_TpLeftInd;
	}

};