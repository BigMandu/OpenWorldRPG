// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "SpawnItemEquipComponent.generated.h"

//for Enum iterator
//ENUM_RANGE_BY_COUNT(EEquipmentType, EEquipmentType::EET_MAX)

class UDataTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API USpawnItemEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnItemEquipComponent();

	//if this true, bhasSpawnItems And bHasSpawnEquipments must be false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems")
	bool bRandomSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	bool bMustSpawnHelmet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	bool bMustSpawnVest;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	bool bMustSpawnBackPpack;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	UDataTable* RifleDT;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	UDataTable* HelmetDT;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	UDataTable* VestDT;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bRandomSpawn"))
	UDataTable* BackPackDT;


	/* Spawn Items */
	//if this true, brandomspawn must be false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "!bRandomSpawn"))
	bool bHasSpawnItems = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bHasSpawnItems"))
	TArray<struct FItemSetting> SpawnItemList;

	/* Spawn Equipments */
	//if this true, brandomspawn must be false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "!bRandomSpawn"))
	bool bHasSpawnEquipments = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bHasSpawnEquipments"))
	TArray<TSubclassOf<class AEquipment>> SpawnEquipmentList;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	void TryEquip(class UCustomPDA* CPDA, class ABaseCharacter* Char);
	
	int32 SpawnORNot(bool bIsMustSpawn, int32 returnIndex);

public:	

	void JudgeSpawn();

	/**** Spawn items *******/
	void SpawnItems();
	void SpawnEquipments();
	void SpawnRandomItems();

	
};
