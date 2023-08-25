// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Door.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "MissionDoor.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AMissionDoor : public ADoor, public IMissionInterface
{
	GENERATED_BODY()

	bool bIsShowing;
	FVector ThisActorsLocation;
	class AMainController* PlayerCon;

public:
	AMissionDoor();

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* NotifyIcon;

	UPROPERTY(EditAnywhere)
	FVector2D WidgetSize = FVector2D(100.f);

	UPROPERTY(EditAnywhere)
	bool bStartMissionAgain;

	UPROPERTY(EditAnywhere)
	FString MissionUniqueID;

protected:
	virtual void BeginPlay() override;
public:
	
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Interaction(AActor* Actor) override;

	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();

};
