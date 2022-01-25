// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AActor;
class AItem;

//Inventory�� ������Ʈ �ɶ����� �������Ʈ���� ui�� ������Ʈ�� �ϱ� ���� delegate�� ������ش�.
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) //Instanced)
	//TArray<TSubclassOf<class AItem>> DefaultItems; //���� ���۽� �־����� item��
	
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	int32 Capacity;

	//UPROPERTY(BlueprintAssignable, Category = Inventory)
	//FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TArray<AActor*> InventoryItems; //inventory�� �ִ� item��

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool AddItem(AItem* Item);
	bool RemoveItem(AItem* Item);
		
};
