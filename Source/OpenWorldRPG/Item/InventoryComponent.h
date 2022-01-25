// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AActor;
class AItem;

//Inventory가 업데이트 될때마다 블루프린트에서 ui를 업데이트를 하기 위해 delegate를 만들어준다.
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) //Instanced)
	//TArray<TSubclassOf<class AItem>> DefaultItems; //게임 시작시 주어지는 item들
	
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	int32 Capacity;

	//UPROPERTY(BlueprintAssignable, Category = Inventory)
	//FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	TArray<AActor*> InventoryItems; //inventory에 있는 item들

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool AddItem(AItem* Item);
	bool RemoveItem(AItem* Item);
		
};
