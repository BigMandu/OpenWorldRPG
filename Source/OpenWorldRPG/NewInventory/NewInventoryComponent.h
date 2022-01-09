// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NewInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UNewInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNewInventoryComponent();

	/* Instance Editable, ReadOnly, Expose on Spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Constants")
	int32 Columns;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Constants")
	int32 Rows;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

		
};
