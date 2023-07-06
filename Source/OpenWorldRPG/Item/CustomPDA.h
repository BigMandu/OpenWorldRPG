// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "CustomPDA.generated.h"

/**
 * 
 */


//class EEquipmentType;

UCLASS(BlueprintType)
class OPENWORLDRPG_API UCustomPDA : public UBasePDA

{
	GENERATED_BODY()
public:
	/**** Equipment ****/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	bool bCanEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;
	
	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform MeshAttachTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	class USoundCue* EquippedSound;
	

	UPROPERTY(EditAnywhere, Category = "Parts")
	EWeaponPartsLimitationType WeaponPartsLimitationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Widget")
	bool bHasWidget;

	/***** Storage ********/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage")
	bool bHasStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage", meta=(EditCondition="bHasStorage"))
	int32 StorageX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage", meta=(EditCondition = "bHasStorage"))
	int32 StorageY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Storage", meta=(EditCondition = "bHasStorage"))
	float TileSize = 60.f;

};
