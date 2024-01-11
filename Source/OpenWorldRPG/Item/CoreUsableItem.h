// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "CoreUsableItem.generated.h"

/**
 * 
 */

UCLASS()
class OPENWORLDRPG_API ACoreUsableItem : public AItem
{
	GENERATED_BODY()
private:
	FTimerHandle CompassHeldTimer;
	FVector NPLocation;
public:
	ACoreUsableItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Compass")
	UStaticMeshComponent* OptionalStaticMesh;



protected:
	
public:
	
	//override from AItem class
	virtual void Use(class ABaseCharacter* Actor, UNewItemObject* Obj = nullptr) override;
	virtual void AttachToHand(ABaseCharacter* Actor, class UNewItemObject* Obj, bool bIsNeedToDestory = true) override;
	virtual void SetMesh() override;

	void ToggleOn(class AMainCharacter* Actor, UNewItemObject* Obj = nullptr);
	void ToggleOff(AMainCharacter* Actor, UNewItemObject* Obj = nullptr);

	void UpdateCompassPointerRotation();

	virtual void ReadyToDestory(ABaseCharacter* Actor, UNewItemObject* Obj = nullptr) override;
};
