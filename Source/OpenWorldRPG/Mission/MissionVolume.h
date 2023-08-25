// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "MissionVolume.generated.h"

UCLASS()
class OPENWORLDRPG_API AMissionVolume : public AActor, public IMissionInterface
{
	GENERATED_BODY()

	bool bIsShowing;

	class AMainController* PlayerCon;
	FVector ThisActorsLocation;
public:	
	// Sets default values for this actor's properties
	AMissionVolume();

	class UBillboardComponent* billboard;

	UPROPERTY(EditAnywhere, Category = "MissionVol")
	class UBoxComponent* MissionVol;
	UPROPERTY(EditAnywhere, Category = "MissionVol")
	FString MissionUniqueID;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* NotifyIcon;

	UPROPERTY(EditAnywhere)
	FVector2D WidgetSize = FVector2D(100.f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool EvaluateMissionStatus(class AOpenWorldRPGGameModeBase& T_gMode);

	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();

	//void CustomProjectionWorldToScreen(const FVector& WorldLocation, FVector2D& ScreenPos, bool& bIsBehind);
	//void AdjustProjectionScreenPosition(FVector2D& ScreenPos, const FVector2D& ScreenSize, const bool& bIsBehind);
};
