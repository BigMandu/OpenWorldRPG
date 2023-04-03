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

	//Optional Static Mesh, if needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	UStaticMesh* Mesh_Optional;

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

	/***** Hand Attach Transform ******/
	UPROPERTY(EditAnywhere, Category = "Item | Attach")
	bool bIsNeedToAttachHandBeforeUse;

	UPROPERTY(EditAnywhere, Category = "Item | Attach", meta = ( EditCondition = "!bAttachedToRightHand" ))
	bool bAttachedToLeftHand = true;
	UPROPERTY(EditAnywhere, Category = "Item | Attach", meta = ( EditCondition = "!bAttachedToLeftHand" ))
	bool bAttachedToRightHand;

	UPROPERTY(EditAnywhere, Category = "Item | Attach")
	FTransform TPS_HandAttachTransform;
	UPROPERTY(EditAnywhere, Category = "Item | Attach")
	FTransform FPS_HandAttachTransform;

	/****** Using AnimMontage *****/
	UPROPERTY(EditAnywhere, Category = "Item | Animation")
	class UAnimMontage* FPS_AttachAnimMontage;
	UPROPERTY(EditAnywhere, Category = "Item | Animation")
	UAnimMontage* TPS_AttachAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Item | Animation")
	bool bIsNeedLooping;
	UPROPERTY(EditAnywhere, Category = "Item | Animation")
	UAnimMontage* FPS_UseAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Item | Animation")
	UAnimMontage* TPS_UseAnimMontage;

	/**** Stackable ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stack")
	bool bCanStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stack", meta=(EditCondition="bCanStack"))
	int32 MaxStackSize;
	
};
