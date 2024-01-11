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
struct FCurrentMAXStats : public FTableRowBase
{
	GENERATED_BODY()
public:	
	FCurrentMAXStats() :Level(1), Stamina(100.f), JumpZ(400.f) {}

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Stats | Level")
	int32 Level;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats | Level")
	float Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats | Level")
	float StaminaDrainRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats | Level")
	float AimInitializeRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats | Level")
	float JumpZ;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats | Level")
	float NextExp;
};

UCLASS()
class OPENWORLDRPG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY()
	class UESCMenuWidget* EscMenuWidget;

	bool bShowEscMenu;
	bool bShowOptionWidget;

public:
	UMyGameInstance();

	UPROPERTY()
	class UDataTable* LevelDataTable;

	UPROPERTY()
	UDataTable* CraftRecipeDataTable;

	UPROPERTY()
	UDataTable* FootStepDataTable;

	UPROPERTY()
	UDataTable* IntelDataTable;

	UPROPERTY()
	UDataTable* MainMissionDataTable;

	UPROPERTY()
	UDataTable* TutorialMissionDataTable;

	UPROPERTY()
	UDataTable* CommonImpactEffectTable;

	/* Widget */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	//TSubclassOf<UUserWidget> WESCWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UOptionWidget> WOptionWidget;

private:
	
	

public:
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;
	FCurrentMAXStats* GetLevelStats(int32 Level);

	class USoundCue* GetFootStepSound(TWeakObjectPtr<class UPhysicalMaterial> PhysMat);

	void PlayLoadingScreen();
	void StopLoadingScreen();

	void ToggleESCWidget();
	void InitVariableRelevantEscWidget();

	UOptionWidget* CreateOptionWidget();

	//int32 GetCraftRecipeCount();
	//TMap<FName,uint8*> GetCraftRecipeMap();

};
