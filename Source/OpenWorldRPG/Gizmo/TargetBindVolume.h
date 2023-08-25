// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetBindVolume.generated.h"

UCLASS()
class OPENWORLDRPG_API ATargetBindVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetBindVolume();

	UPROPERTY(EditAnywhere, Category = "Volume")
	class UBoxComponent* TargetPopupVolume;

	class UBillboardComponent* billboard;

	UPROPERTY(EditAnywhere, Category = "Volume")
	TArray<class AShootingTarget*> BindShootingTargets;

	//한번이라도 Overlap이 발생했다면, 더이상 overlap 이벤트를 발생하지 않도록 한다.
	// 외부에서 초기화 하는 변수다.(Gamemode에서 초기화함)
	bool bIsAlreadyOccurredOverlap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void CallPopUp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

