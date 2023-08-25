// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/GameData/CraftSystemComponent.h"

// Sets default values for this component's properties
UCraftSystemComponent::UCraftSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCraftSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCraftSystemComponent::SaveCraftStatus_TwoIng(FName CraftItemName, FTwoIngredientRecipe RemainReciepeStatus)
{
	//이미 저장되어있는 Data가 있다면 덮어쓴다.
	FTwoIngredientRecipe* FindRow = SaveTwoIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		FindRow->IngOneRemainCount = RemainReciepeStatus.IngOneRemainCount;
		FindRow->IngTwoRemainCount = RemainReciepeStatus.IngTwoRemainCount;
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_TwoIng // Save to Exist data."));
	}
	//저장된게 없으면 Data를 새로 추가한다.
	else
	{
		SaveTwoIngRecipeStatusMap.Add(CraftItemName,RemainReciepeStatus);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_TwoIng // Save New data."));
	}
}

FTwoIngredientRecipe* UCraftSystemComponent::LoadCraftIngredient_TwoIng(FName CraftItemName)
{
	FTwoIngredientRecipe* FindRow = SaveTwoIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		return FindRow;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UCraftSystemComponent::LoadCraftIngredient_TwoIng // Can't Find Data"));
	}
	return nullptr;
}


void UCraftSystemComponent::DeleteCraftStatus_TwoIng(FName CraftItemName)
{
	FTwoIngredientRecipe* FindRow = SaveTwoIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		SaveTwoIngRecipeStatusMap.Remove(CraftItemName);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // data delete complete"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // Can't Find Data, something wrong"));
	}
}


/************************ Three ****************************/

void UCraftSystemComponent::SaveCraftStatus_ThreeIng(FName CraftItemName, FThreeIngredientRecipe RemainReciepeStatus)
{
	FThreeIngredientRecipe* FindRow = SaveThreeIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		FindRow->IngOneRemainCount = RemainReciepeStatus.IngOneRemainCount;
		FindRow->IngTwoRemainCount = RemainReciepeStatus.IngTwoRemainCount;
		FindRow->IngThreeRemainCount = RemainReciepeStatus.IngThreeRemainCount;
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_ThreeIng // Save to Exist data."));
	}
	else
	{
		SaveThreeIngRecipeStatusMap.Add(CraftItemName, RemainReciepeStatus);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_ThreeIng // Save New data."));
	}
}
FThreeIngredientRecipe* UCraftSystemComponent::LoadCraftIngredient_ThreeIng(FName CraftItemName)
{
	FThreeIngredientRecipe* FindRow = SaveThreeIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		return FindRow;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_TwoIng // Can't Find Data"));
	}
	return nullptr;
}
void UCraftSystemComponent::DeleteCraftStatus_ThreeIng(FName CraftItemName)
{

	FThreeIngredientRecipe* FindRow = SaveThreeIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		SaveThreeIngRecipeStatusMap.Remove(CraftItemName);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // data delete complete"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // Can't Find Data, something wrong"));
	}
}

/************************ Four ****************************/

void UCraftSystemComponent::SaveCraftStatus_FourIng(FName CraftItemName, FFourIngredientRecipe RemainReciepeStatus)
{
	FFourIngredientRecipe* FindRow = SaveFourIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		FindRow->IngOneRemainCount = RemainReciepeStatus.IngOneRemainCount;
		FindRow->IngTwoRemainCount = RemainReciepeStatus.IngTwoRemainCount;
		FindRow->IngThreeRemainCount = RemainReciepeStatus.IngThreeRemainCount;
		FindRow->IngFourRemainCount = RemainReciepeStatus.IngFourRemainCount;
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_ThreeIng // Save to Exist data."));
	}
	else
	{
		SaveFourIngRecipeStatusMap.Add(CraftItemName, RemainReciepeStatus);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_ThreeIng // Save New data."));
	}
}

FFourIngredientRecipe* UCraftSystemComponent::LoadCraftIngredient_FourIng(FName CraftItemName)
{
	FFourIngredientRecipe* FindRow = SaveFourIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		return FindRow;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::LoadCraftIngredient_FourIng // Can't Find Data"));
	}
	return nullptr;
}

void UCraftSystemComponent::DeleteCraftStatus_FourIng(FName CraftItemName)
{
	FFourIngredientRecipe* FindRow = SaveFourIngRecipeStatusMap.Find(CraftItemName);
	if (FindRow)
	{
		SaveFourIngRecipeStatusMap.Remove(CraftItemName);
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // data delete complete"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCraftSystemComponent::DeleteCraftStatus_TwoIng // Can't Find Data, something wrong"));
	}
}