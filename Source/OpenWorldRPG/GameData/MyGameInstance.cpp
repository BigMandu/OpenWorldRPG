// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Engine/DataTable.h"

UMyGameInstance::UMyGameInstance()
{
	
	//FString StrLevelDataPath = TEXT("DataTable'/Game/GameData/StrLevelData.StrLevelData'");
	FString StrLevelDataPath = TEXT("/Game/GameData/StrLevelData.StrLevelData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_StrLevelData(*StrLevelDataPath);


	//DataTable'/Game/GameData/CraftRecipeTable.CraftRecipeTable'
	FString CraftRecipeTablePath = TEXT("/Game/GameData/CraftRecipeTable.CraftRecipeTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_CraftRecipe(*CraftRecipeTablePath);
	
	//UDataTable* DT_StrLevelData = LoadObject<UDataTable>(this,StrLevelDataPath);

	if(DT_StrLevelData.Succeeded())
	{ 
		STRLevelDataTable = DT_StrLevelData.Object;
	}

	if (DT_CraftRecipe.Succeeded())
	{
		CraftRecipeDataTable = DT_CraftRecipe.Object;
	}
}

void UMyGameInstance::Init()
{
	Super::Init();
}
//LogDataTable: Error: UDataTable::FindRow : 'StrLevelData' specified no row for DataTable '/Game/GameData/StrLevelData.StrLevelData'.
FStrengthStats* UMyGameInstance::GetStrengthStats(int32 Level)
{
	if(STRLevelDataTable)
	{
		FString TableName(STRLevelDataTable->GetName());
		FName RowName = FName(*FString::FromInt(Level));
	
		return STRLevelDataTable->FindRow<FStrengthStats>(RowName, TableName);
	}
	return nullptr;
}

//int32 UMyGameInstance::GetCraftRecipeCount()
//{
//	if (CraftRecipeDataTable)
//	{		
//		auto TableMap = CraftRecipeDataTable->GetRowMap();
//		return TableMap.Num();
//	}
//}

//TMap<FName, uint8*> UMyGameInstance::GetCraftRecipeMap()
//{
//	if (CraftRecipeDataTable)
//	{
//		return CraftRecipeDataTable->GetRowMap();
//	}
//	return;
//}