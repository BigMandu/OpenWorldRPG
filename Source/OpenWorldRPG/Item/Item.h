// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "Item.generated.h"

//��� Item�� �뻧 Ŭ����
UCLASS()
class OPENWORLDRPG_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	/* Item�� ��� �׼� (Equip, Food, etc..) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText UseActionText;

	/* Item thumbnail (in inventory slot thumbnail) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;

	/* Item �̸� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText ItemName;

	/* item �� ���� ���� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText ItemDescription;

	/* Item Skeletal Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class USkeletalMesh* ItemMesh;

	/* Item Static Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UStaticMeshComponent* ItemSMesh;

	/* Item�� Interactive�Ҷ� ������ Tip*/
	UPROPERTY(EditDefaultsOnly, Category = Item)
	FString InteractText;

	//�� item�� ���� inventory
	UInventoryComponent* OwningInventory;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	virtual void Interact();

	virtual void Pickup();

	UFUNCTION(BlueprintCallable)
	virtual void Use(class ACharacter* Character);

	

};
