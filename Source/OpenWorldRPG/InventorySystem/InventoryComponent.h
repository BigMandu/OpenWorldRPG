// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


/*  https://www.youtube.com/watch?v=-WNwo-riV1Y
*	12분 44초
*
*/

//Inventory가 업데이트 될때마다 블루프린트에서 ui를 업데이트를 하기 위해 delegate를 만들어준다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UItem*> DefaultItems; //게임 시작시 주어지는 item들

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TArray<class UItem*> InventoryItems; //inventory에 있는 item들

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool AddItem(class UItem* Item);
	bool RemoveItem(class UItem* Item);
		
};
