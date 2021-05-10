// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


/*  https://www.youtube.com/watch?v=-WNwo-riV1Y
*	12�� 44��
*
*/

//Inventory�� ������Ʈ �ɶ����� �������Ʈ���� ui�� ������Ʈ�� �ϱ� ���� delegate�� ������ش�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UItem*> DefaultItems; //���� ���۽� �־����� item��

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TArray<class UItem*> InventoryItems; //inventory�� �ִ� item��

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool AddItem(class UItem* Item);
	bool RemoveItem(class UItem* Item);
		
};
