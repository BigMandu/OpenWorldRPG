// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Interactable.h"
#include "Door.generated.h"

/**
 * 
 */
 UENUM()
enum class EDoorType : uint8
{	
	EDT_CollisionDoor	UMETA(DisplayName = "CollisionDoor"),
	EDT_InteractDoor	UMETA(DisplayName = "InteractDoor"),
	EDT_CardKeyDoor		UMETA(DisplayName = "CardKeyDoor"),
	EDT_PhysicsDoor		UMETA(DisplayName = "PhysicsDoor"),
	
	EDT_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API ADoor : public AInteractable//, public IInteractive_Interface
{
	GENERATED_BODY()

	bool bIsOpen = false;
	FTimerHandle CallCloseTimerHandle;

	FTimerHandle OpenTimer;
	FTimerHandle CloseTimer;

	//FRotator InitialRelativeRotation;
	FRotator MeshInitRotation;
	FVector MeshInitLocation;

	FRotator CBInitRotation;
	FVector CBInitLocation;


public:
	ADoor();

	UPROPERTY(EditAnywhere, Category = "Door | Type")
	EDoorType DoorType;

	//Actor의 방향대로 미는문인지, 단 방향으로만 열리는 문인지.
	//Only affect rotation
	UPROPERTY(EditAnywhere, Category = "Door | Settings")
	bool bIsDuplexDoor = false;

	//CollisionDoor
	UPROPERTY(EditAnywhere, Category = "Door | Settings")
	float AutoCloseTime = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Comp")
	class UBoxComponent* collisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Comp")
	class UPhysicsConstraintComponent* Topconstraint;
	UPROPERTY(VisibleAnywhere, Category = "Comp")
	UPhysicsConstraintComponent* Bottomconstraint;

	//UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Rotation")
	
	UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Rotation")
	FRotator TargetRotation;
	UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Rotation")
	float RotationLerpSpeed;

	//UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Location")
	
	UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Location")
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Location")
	float LocationLerpSpeed;
	
private:
	void SettingPhysicsDoor();
	bool IsPlayerBehind(AActor* Actor);
public:

	virtual void Interaction(AActor* Actor) override;
	virtual void PostInitializeComponents() override;

	void ToggleDoor(AActor* Actor);

	bool CanClearTimer(bool& bHasRot, bool& bHasLoc, bool& bReachedLoc, bool& bReachedRot);
	void ClearTimer(FTimerHandle& ClearTimerHandle);

	UFUNCTION()
	void OnDoorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDoorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
	void OpenDoor(bool bNeedToReverse);
	void CloseDoor();
};
