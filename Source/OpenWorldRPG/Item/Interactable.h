// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactive_Interface.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "Interactable.generated.h"

class UStaticMeshComponent;
class USkeletalMesh;


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
class OPENWORLDRPG_API AInteractable : public AActor, public IInteractive_Interface, public IItemInterface
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	/* Item Static Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	UStaticMeshComponent* Mesh;

	/* Item의 사용 액션 (Equip, Food, etc..) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText UseActionText;

	/* Item 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ItemName;
	
	/* Item의 Interactive할때 나오는 Tip*/
	UPROPERTY(EditAnywhere, Category = Item)
	FString InteractText;


	UPROPERTY(EditAnywhere, Category = Item)
	EInteractType InteractType;


	//AI끼리 충돌방지, 먼저 선점(인식) 했는지 여부.
	bool bIsPreOccupied;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void PostInitializeComponents() override;
	virtual void Interaction(AActor* Actor) override;

	virtual void SetOutline() override;
	virtual void UnsetOutline() override;
};
