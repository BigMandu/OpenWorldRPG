// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPDA.h"
#include "CustomStructLibrary.generated.h"




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
	UBasePDA* DataAsset;


	// Create Obj할때 Data를 넘겨주기 위함.
	UPROPERTY()
	TArray<class UNewItemObject*> Inventory;

	class UItemStorageObject* MotherStorage;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count;

	UPROPERTY()
	int32 TopLeftIndex;

	FItemSetting()
	:DataAsset(nullptr), Count(1), TopLeftIndex(0)
	{
		/*DataAsset = nullptr;
		Count = 1;
		TopLeftIndex = 0;*/
	}

	FItemSetting(UBasePDA* Var_PDA, int32 Var_Cnt, int32 Var_TpLeftInd)
	:DataAsset(Var_PDA), Count(Var_Cnt), TopLeftIndex(Var_TpLeftInd)
	{
		/*DataAsset = Var_PDA;
		Count = Var_Cnt;
		TopLeftIndex = Var_TpLeftInd;*/
	}

	void Serialize(FArchive& Ar)
	{
		Ar << DataAsset;
		//Ar << Inventory;
		//Ar << MotherStorage;
		Ar << Count;
		Ar << TopLeftIndex;
	}

};


USTRUCT()
struct FIntelAcquiredInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool AcqIntel1;
	UPROPERTY()
	bool AcqIntel2;
	UPROPERTY()
	bool AcqIntel3;
	UPROPERTY()
	bool AcqIntel4;
	UPROPERTY()
	bool AcqIntel5;

	void Serialize(FArchive& Ar)
	{
		Ar << AcqIntel1;
		Ar << AcqIntel2;
		Ar << AcqIntel3;
		Ar << AcqIntel4;
		Ar << AcqIntel5;
	}
};

USTRUCT()
struct FInProgressMissionInfo
{
	GENERATED_BODY()
public:
	FString InProgressMissionUnqID;
	EMissionType MissionType;
	EMissionActionType MissionActionType;
	FString MissionAIUnqID;

	TSubclassOf<AActor> RequireObjectType;
	//AActor* RequireObjectType;
	int32 RequireObjectCount;
	int32 AcquiredObjectCount;

	FText MissionName;
	FText MissionDescription;

	FText Choice1Name;
	FText Choice1Desc;
	FString Choice1MissionUniqID;
	FString Choice1AIUnqID;
	TSubclassOf<AActor> Choice1RequireObjectType;
	//AActor* Choice1RequireObjectType;

	FText Choice2Name;
	FText Choice2Desc;
	FString Choice2MissionUniqID;
	FString Choice2AIUnqID;
	TSubclassOf<AActor> Choice2RequireObjectType;
	//AActor* Choice2RequireObjectType;

	void Serialize(FArchive& Ar)
	{
		Ar << InProgressMissionUnqID;
		Ar << MissionType;
		Ar << MissionActionType;
		Ar << MissionAIUnqID;
		Ar << RequireObjectType;
		Ar << RequireObjectCount;
		Ar << AcquiredObjectCount;
		Ar << MissionName;
		Ar << MissionDescription;
		Ar << Choice1Name;
		Ar << Choice1Desc;
		Ar << Choice1MissionUniqID;
		Ar << Choice1AIUnqID;
		Ar << Choice1RequireObjectType;
		Ar << Choice2Name;
		Ar << Choice2Desc;
		Ar << Choice2MissionUniqID;
		Ar << Choice2AIUnqID;
		Ar << Choice2RequireObjectType;

		//Ar << MissionStatus;
	}
};



//저장할때 필요한 것만 저장하기 위한 Struct.
USTRUCT(BlueprintType)
struct FSaveItem
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString PDAID;

	UPROPERTY()
	int32 Count;

	UPROPERTY()
	int32 TopLeftIndex;


	FSaveItem()
		:Count(0), TopLeftIndex(0)
	{

	}

	FSaveItem(FString Var_PDA, int32 Var_Cnt, int32 Var_TpLeftInd)
		:PDAID(Var_PDA), Count(Var_Cnt), TopLeftIndex(Var_TpLeftInd)
	{
	}

	void Serialize(FArchive& Ar)
	{
		Ar << PDAID;
		Ar << Count;
		Ar << TopLeftIndex;
	}

};






///////////////////////////////////////////////////////////////////////
/*                      Data Table Row base                          */
///////////////////////////////////////////////////////////////////////

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