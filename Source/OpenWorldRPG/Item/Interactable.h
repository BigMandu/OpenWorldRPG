// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "Interactable.generated.h"

class UMeshComponent;
//class UStaticMeshComponent;
//class USkeletalMesh;
class UCustomPDA;

//UENUM(BlueprintType)
//enum class EInteractType : uint8
//{
//	EIT_Gizmo		UMETA(DisplayName = "EIT_Gizmo"),
//	EIT_Item		UMETA(DisplayName = "EIT_Item"),
//	EIT_Equipment	UMETA(DisplayName = "EIT_Equipment"),
//	EIT_LootBox		UMETA(DisplayName = "EIT_LootBox"),
//
//	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
//};

UCLASS()
class OPENWORLDRPG_API AInteractable : public AActor, public IInteractive_Interface//, public IItemInterface
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSettings")//, meta = (ExposeOnSpawn = "true"))
	FItemSetting ItemSetting;

	/* Item Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemSettings")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemSettings")
	USkeletalMeshComponent* SKMesh;

	//If true, This Actor Can't Interact.
	bool bCanNotInteractable;

	//AI끼리 충돌방지, 먼저 선점(인식) 했는지 여부.
	bool bIsPreOccupied;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/*Called when an instance of this class is placed(in editor) or spawned.
		* @param	Transform			The transform the actor was constructed at.
	*/
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;
	virtual void SetMesh();// UCustomPDA* PDA);// , UMeshComponent*& MeshComp);

	virtual void Interaction(AActor* Actor) override;
	virtual void SetOutline() override;
	virtual void UnsetOutline() override;
};
