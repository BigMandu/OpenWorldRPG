// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "CraftSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UCraftSystemComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	TMap<FName, FTwoIngredientRecipe> SaveTwoIngRecipeStatusMap;
	TMap<FName, FThreeIngredientRecipe> SaveThreeIngRecipeStatusMap;
	TMap<FName, FFourIngredientRecipe> SaveFourIngRecipeStatusMap;

public:	
	// Sets default values for this component's properties
	UCraftSystemComponent();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void SaveCraftStatus_TwoIng(FName CraftItemName, FTwoIngredientRecipe RemainReciepeStatus);
	FTwoIngredientRecipe* LoadCraftIngredient_TwoIng(FName CraftItemName);
	void DeleteCraftStatus_TwoIng(FName CraftItemName);


	void SaveCraftStatus_ThreeIng(FName CraftItemName, FThreeIngredientRecipe RemainReciepeStatus);
	FThreeIngredientRecipe* LoadCraftIngredient_ThreeIng(FName CraftItemName);	
	void DeleteCraftStatus_ThreeIng(FName CraftItemName);


	void SaveCraftStatus_FourIng(FName CraftItemName, FFourIngredientRecipe RemainReciepeStatus);
	FFourIngredientRecipe* LoadCraftIngredient_FourIng(FName CraftItemName);
	void DeleteCraftStatus_FourIng(FName CraftItemName);
};
