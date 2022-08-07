// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomPDA.generated.h"

/**
 * 
 */

class UMaterialInterface;
//class EEquipmentType;

UCLASS(BlueprintType)
class OPENWORLDRPG_API UCustomPDA : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	/***** Mesh ******/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	USkeletalMesh* SKMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	FVector ReSizeScale;



	/****  Icon *****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Icon")
	UMaterialInterface* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Icon")
	UMaterialInterface* IconRotated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FIntPoint ItemSize;

	/********* ItemText *********/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	UPROPERTY(EditAnywhere, Category = "Item")
	FString InteractText;

	UPROPERTY(EditAnywhere, Category = "Item")
	EInteractType InteractType;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemState ItemState;*/

	


	/**** Equipment ****/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Equipment")
	bool bCanEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Equipment")
	EEquipmentType EquipmentType;
	
	UPROPERTY(EditAnywhere, Category = "Equip | Transform")
	FTransform MeshAttachTransform;

	/***** Storage ********/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Storage")
	bool bHasStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Storage")
	int32 StorageX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Storage")
	int32 StorageY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Storage")
	float TileSize = 60.f;

};
