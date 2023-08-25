// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Weapon_Instant.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "MissionWeapon_Instant.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AMissionWeapon_Instant : public AWeapon_Instant, public IMissionInterface
{
	GENERATED_BODY()

	bool bIsShowing;
	FVector ThisActorsLocation;
	class AMainController* PlayerCon;

public:

	AMissionWeapon_Instant();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* NotifySphere;
	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* NotifyIcon;

	UPROPERTY(EditAnywhere)
	FVector2D WidgetSize = FVector2D(100.f);

	UPROPERTY(EditAnywhere)
	FString MissionUniqueID;

	

protected:
	virtual void BeginPlay() override;
public:
	
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Interaction(AActor* Actor) override;

	UFUNCTION()
	void OverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();
};
