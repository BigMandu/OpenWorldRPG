// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Engine/DataTable.h"

UMyGameInstance::UMyGameInstance()
{
	
	//FString StrLevelDataPath = TEXT("DataTable'/Game/GameData/StrLevelData.StrLevelData'");
	FString StrLevelDataPath = TEXT("/Game/GameData/StrLevelData.StrLevelData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_StrLevelData(*StrLevelDataPath);
	
	//UDataTable* DT_StrLevelData = LoadObject<UDataTable>(this,StrLevelDataPath);

	if(DT_StrLevelData.Succeeded())
	{ 
		STRLevelDataTable = DT_StrLevelData.Object;
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
