// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "BasePDA.generated.h"
/**
 * 
 */
class UMaterialInterface;

UCLASS()
class OPENWORLDRPG_API UBasePDA : public UPrimaryDataAsset
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Icon")
	UMaterialInterface* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Icon", meta=(EditCondition ="bCanRotate"))
	UMaterialInterface* IconRotated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FIntPoint ItemSize;

	/********* Item Info *********/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Info")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Info")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Info")
	FText UseActionText;

	//UPROPERTY(EditAnywhere, Category = "Item | Info")
	//FString InteractText;
	
	

	UPROPERTY(EditAnywhere, Category = "Item | Info")
	EInteractType InteractType;

	UPROPERTY(EditAnywhere, Category = "Item | Info")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item | Info")
	EAmmoType AmmoType;


	/**** Stackable ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stack")
	bool bCanStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stack", meta=(EditCondition="bCanStack"))
	int32 MaxStackSize;
	
};
