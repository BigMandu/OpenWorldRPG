// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "OpenWorldRPG\GameData\FootStepTable.h"
#include "OpenWorldRPGLoadingScreen/Public/OpenWorldRPGLoadingScreen.h"

UMyGameInstance::UMyGameInstance()
{
	
	FString LevelDataPath = TEXT("/Game/GameData/LevelData.LevelData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_LevelData(*LevelDataPath);


	//DataTable'/Game/GameData/CraftRecipeTable.CraftRecipeTable'
	FString CraftRecipeTablePath = TEXT("/Game/GameData/CraftRecipeTable.CraftRecipeTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_CraftRecipe(*CraftRecipeTablePath);
	
	FString FootStepDataTablePath = TEXT("/Game/GameData/FootStepDataTable.FootStepDataTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_FootStepData(*FootStepDataTablePath);

	FString IntelDataTablePath = TEXT("/Game/GameData/IntelDataTable.IntelDataTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_IntelData(*IntelDataTablePath);
	
	FString MissionDataTablePath = TEXT("/Game/GameData/MainMissionDataTable.MainMissionDataTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MissionData(*MissionDataTablePath);

	FString TutorialMissionDataTablePath = TEXT("/Game/GameData/TutorialMissionDataTable.TutorialMissionDataTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_TutorialMissionData(*TutorialMissionDataTablePath);

	


	if( DT_LevelData.Succeeded())
	{ 
		LevelDataTable = DT_LevelData.Object;
	}

	if (DT_CraftRecipe.Succeeded())
	{
		CraftRecipeDataTable = DT_CraftRecipe.Object;
	}

	if (DT_FootStepData.Succeeded())
	{
		FootStepDataTable = DT_FootStepData.Object;
	}

	if (DT_IntelData.Succeeded())
	{
		IntelDataTable = DT_IntelData.Object;		
	}

	if (DT_MissionData.Succeeded())
	{
		MainMissionDataTable = DT_MissionData.Object;
	}

	if (DT_TutorialMissionData.Succeeded())
	{
		TutorialMissionDataTable = DT_TutorialMissionData.Object;
	}
}

void UMyGameInstance::Init()
{
	Super::Init();
}
//LogDataTable: Error: UDataTable::FindRow : 'StrLevelData' specified no row for DataTable '/Game/GameData/StrLevelData.StrLevelData'.
FCurrentMAXStats* UMyGameInstance::GetLevelStats(int32 Level)
{
	if( LevelDataTable )
	{
		FString TableName(LevelDataTable->GetName());
		FName RowName = FName(*FString::FromInt(Level));
	
		return LevelDataTable->FindRow<FCurrentMAXStats>(RowName, TableName);
	}
	return nullptr;
}

USoundCue* UMyGameInstance::GetFootStepSound(TWeakObjectPtr<UPhysicalMaterial> PhysMat)
{
	USoundCue* ProvisionalCue = nullptr;
	if (FootStepDataTable)
	{
		for ( auto row : FootStepDataTable->GetRowMap())
		{
			
			FFootStepTable* RowData = (FFootStepTable*)row.Value;

			if (RowData && PhysMat.IsValid() && RowData->PhysMat == PhysMat.Get() )
			{
				if ( RowData->StepSound )
				{
					return RowData->StepSound;
				}
			}

			//PhysMat이 없는 경우엔 
			if ( RowData->PhysMat->GetFName().IsEqual(FName("PM_Tile")) )
			{
				ProvisionalCue = RowData->StepSound;
			}
		}
	}

	if ( ProvisionalCue )
	{
		return ProvisionalCue;
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("UMyGameInstance::GetFootStepSound // FootStep Data is weird."));
	}

	return nullptr;
}


void UMyGameInstance::PlayLoadingScreen()
{
	IOpenWorldRPGLoadingScreenModule& LoadingScreen = IOpenWorldRPGLoadingScreenModule::Get();
	LoadingScreen.StartInGameLoadingScreen(true,3.f);
}

void UMyGameInstance::StopLoadingScreen()
{
	IOpenWorldRPGLoadingScreenModule& LoadingScreen = IOpenWorldRPGLoadingScreenModule::Get();
	LoadingScreen.StopInGameLoadingScreen();
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


