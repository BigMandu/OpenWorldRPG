// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactive_Interface.h"
#include "Interactable.generated.h"

UCLASS()
class OPENWORLDRPG_API AInteractable : public AActor, public IInteractive_Interface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact")
	class UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual AActor* Interaction(AActor* Actor) override;
	

};
