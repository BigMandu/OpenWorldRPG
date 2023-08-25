// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "Mission_Item.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AMission_Item : public AItem, public IMissionInterface
{
	GENERATED_BODY()
public:
	AMission_Item();

	UPROPERTY(EditAnywhere)
	FString MissionUniqueID;
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* NotifySphere;
	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* NotifyIcon;
	

	virtual void PostInitializeComponents() override;

	virtual void Interaction(AActor* Actor) override;

	UFUNCTION()
	void OverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlapMissionObj(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();

};
