// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
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
	class UBasePDA* DataAsset;


	// Create Obj할때 Data를 넘겨주기 위함.
	TArray<class UNewItemObject*> Inventory;
	class UItemStorageObject* MotherStorage;

	
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

	FItemSetting(UBasePDA* Var_PDA, int32 Var_Cnt, int32 Var_TpLeftInd)
	{
		DataAsset = Var_PDA;
		Count = Var_Cnt;
		TopLeftIndex = Var_TpLeftInd;
	}

};


USTRUCT(BlueprintType)
struct FRifleTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* WeaponDataAsset;

};

USTRUCT(BlueprintType)
struct FPistolTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* PistolDataAsset;

};

USTRUCT(BlueprintType)
struct FHelmetTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* HelmetDataAsset;

};

USTRUCT(BlueprintType)
struct FPlateCarrierTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* PlateCarrierDataAsset;
};

USTRUCT(BlueprintType)
struct FVestTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* VestDataAsset;

};

USTRUCT(BlueprintType)
struct FBackPackTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomPDA* BackPackDataAsset;
	
};

USTRUCT(BlueprintType)
struct FFoodTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* FoodDataAsset;
};

USTRUCT(BlueprintType)
struct FMedicalTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* MedicalDataAsset;
};

USTRUCT(BlueprintType)
struct FScrapTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* ScrapDataAsset;
};

USTRUCT(BlueprintType)
struct FAmmoTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* AmmoDataAsset;
};




USTRUCT(BlueprintType)
struct FTwoIngredientRecipe
{
	GENERATED_BODY()
public:
	int32 IngOneRemainCount;
	int32 IngTwoRemainCount;


	FTwoIngredientRecipe()
	{
		IngOneRemainCount = 0;
		IngTwoRemainCount = 0;
	}

	FTwoIngredientRecipe(int32 FirstRemainCount, int32 SecondRemainCount)
	{
		IngOneRemainCount = FirstRemainCount;
		IngTwoRemainCount = SecondRemainCount;
	}

};

USTRUCT(BlueprintType)
struct FThreeIngredientRecipe
{
	GENERATED_BODY()
public:
	int32 IngOneRemainCount;
	int32 IngTwoRemainCount;
	int32 IngThreeRemainCount;


	FThreeIngredientRecipe()
	{
		IngOneRemainCount = 0;
		IngTwoRemainCount = 0;
		IngThreeRemainCount = 0;
	}

	FThreeIngredientRecipe(int32 FirstRemainCount, int32 SecondRemainCount, int32 ThirdRemainCount)
	{
		IngOneRemainCount = FirstRemainCount;
		IngTwoRemainCount = SecondRemainCount;
		IngThreeRemainCount = ThirdRemainCount;
	}
};


USTRUCT(BlueprintType)
struct FFourIngredientRecipe
{
	GENERATED_BODY()
public:
	int32 IngOneRemainCount;
	int32 IngTwoRemainCount;
	int32 IngThreeRemainCount;
	int32 IngFourRemainCount;


	FFourIngredientRecipe()
	{
		IngOneRemainCount = 0;
		IngTwoRemainCount = 0;
		IngThreeRemainCount = 0;
		IngFourRemainCount = 0;
	}

	FFourIngredientRecipe(int32 FirstRemainCount, int32 SecondRemainCount, int32 ThirdRemainCount, int32 FourthReaminCount)
	{
		IngOneRemainCount = FirstRemainCount;
		IngTwoRemainCount = SecondRemainCount;
		IngThreeRemainCount = ThirdRemainCount;
		IngFourRemainCount = FourthReaminCount;
	}
};