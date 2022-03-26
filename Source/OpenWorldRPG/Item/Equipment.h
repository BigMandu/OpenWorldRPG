// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Item.h"
#include "Equipment.generated.h"

/**
 * 
 */
class AMainController;
class USkeletalMeshComponent;
class UEquipmentComponent;
class USoundCue;
class UNewInventoryComponent;
class UNewInventoryGrid;

//UENUM(BlueprintType)
//enum class EEquipmentType : uint8
//{
//	EET_Rifle		UMETA(DisplayName = "Rifle"),
//	EET_Pistol		UMETA(DisplayName = "Pistol"),
//	EET_Helmet		UMETA(DisplayName = "Helmet"),
//	EET_Vest		UMETA(DisplayName = "Vest"),
//	EET_Backpack	UMETA(DisplayName = "Backpack"),
//
//	EET_MAX			UMETA(DisplayName = "DefaltMAX")
//};

UCLASS()
class OPENWORLDRPG_API AEquipment : public AItem
{
	GENERATED_BODY()
private:


	//TSubclassOf<UNewInventoryGrid>WEquipGridWidget;
public:

	AEquipment();

	//AMainController* MainCon;
	UEquipmentComponent* OwningEquipment;
	AActor* OwningPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	USkeletalMeshComponent* SKMesh;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	USoundCue* EquippedSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	bool bHasStorage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	//float GridTileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TSubclassOf<UUserWidget> WEquipGridWidget;

	UNewInventoryGrid* EquipGridWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UNewInventoryComponent* EquipInventoryComp;

	

protected:
	virtual void BeginPlay() override;

public:
	void SetOwningPlayer(AActor* Actor);
	

	virtual void Equip(AActor* Actor);
	bool CheckSendToInventory(AActor* Actor);

	virtual void Drop() override;
};
