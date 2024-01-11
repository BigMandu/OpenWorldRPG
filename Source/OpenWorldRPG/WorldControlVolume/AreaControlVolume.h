// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaControlVolume.generated.h"

/** This Class is only controlled spawn volume that spawn AI Enemy.
*	when player overlap sphere try spawn AI.
 */
UCLASS()
class OPENWORLDRPG_API AAreaControlVolume : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AAreaControlVolume();

	UPROPERTY(VisibleAnywhere, Category = "AreaControl")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AreaControl")
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaControl | Volume List")
	TArray<class ASpawnVolume*> SpawnVolumeList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void NeedToShowActors();
	void NeedToHideActors();
};
