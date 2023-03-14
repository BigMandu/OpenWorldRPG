// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "CraftRecipe.generated.h"
/**
 * 
 */
//class OPENWORLDRPG_API CraftRecipe
//{
//public:
//	CraftRecipe();
//	~CraftRecipe();
//};



USTRUCT(BlueprintType)
struct FCraftRecipeTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	//GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBasePDA* CraftedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfIngredient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* Ingredient_One;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IngOne_NeedNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* Ingredient_Two;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IngTwo_NeedNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* Ingredient_Three;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IngThree_NeedNum;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBasePDA* Ingredient_Four;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IngFour_NeedNum;


};