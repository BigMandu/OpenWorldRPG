// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "Door.generated.h"

/**
 * 
 */
 UENUM()
enum class EDoorType : uint8
{	
	EDT_CollisionDoor	UMETA(DisplayName = "CollisionDoor"),
	EDT_InteractDoor	UMETA(DisplayName = "InteractDoor"),
	EDT_LockedDoor		UMETA(DisplayName = "LockedDoor"),
	EDT_CardKeyDoor		UMETA(DisplayName = "CardKeyDoor"),
	EDT_PhysicsDoor		UMETA(DisplayName = "PhysicsDoor"),
	
	EDT_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API ADoor : public AInteractable//, public IInteractive_Interface
{
	GENERATED_BODY()

	FTimerHandle CallCloseTimerHandle;
	FTimerHandle OpenTimer;
	FTimerHandle CloseTimer;

	//FRotator InitialRelativeRotation;
	FRotator MeshInitRotation;
	FVector MeshInitLocation;

	FRotator CBInitRotation;
	FVector CBInitLocation;

	FTimerHandle DestroyHandle;
	float DestoryAlphaTime;
	float deltaTime;

	FTransform DoorKnobInitTF;

protected:
	bool bIsLocked = false;
	bool bIsOpen = false;
public:
	ADoor();

	UPROPERTY(EditAnywhere, Category = "Door | Type")
	EDoorType DoorType;

	//Actor의 방향대로 미는문인지, 단 방향으로만 열리는 문인지.
	//Only affect rotation
	UPROPERTY(EditAnywhere, Category = "Door | Settings")
	bool bIsDuplexDoor = false;

	//for CollisionDoor
	UPROPERTY(EditAnywhere, Category = "Door | Settings")
	float AutoCloseTime = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	class UBoxComponent* collisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	class UPhysicsConstraintComponent* Topconstraint;
	UPROPERTY(VisibleAnywhere, Category = "Door")
	UPhysicsConstraintComponent* Bottomconstraint;

	UPROPERTY(EditAnywhere, Category = "Door")
	class UStaticMeshComponent* DoorKnob;

	//for CollisionDoor
	UPROPERTY(EditAnywhere, Category = "Door | Settings")
	FTransform CollisionBoxRelativeTF;

	UPROPERTY(EditAnywhere, Category = "Door | Settings | RotationSetting")
	FRotator TargetRotation;
	UPROPERTY(EditAnywhere, Category = "Door | Settings | RotationSetting")
	float RotationLerpSpeed;

	//UPROPERTY(EditAnywhere, Category = "Door | CollisionType_Location")
	
	UPROPERTY(EditAnywhere, Category = "Door | Settings | LocationSetting")
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, Category = "Door | Settings | LocationSetting")
	float LocationLerpSpeed;
	
private:
	void SettingPhysicsDoor();
	bool IsPlayerBehind(AActor* Actor);
	void DestoryAsset(float DestoryTime);
	void ClearTimer(FTimerHandle& ClearHandle);
protected:
	virtual void BeginPlay() override;
public:

	virtual void Interaction(AActor* Actor) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;

	void ToggleDoor(AActor* Actor);

	bool CanClearTimer(bool& bHasRot, bool& bHasLoc, bool& bReachedLoc, bool& bReachedRot);
	//void ClearDestroyTime(FTimerHandle& ClearDestoryTime);

	UFUNCTION()
	void OnDoorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDoorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*UFUNCTION()
	void OnDoorKnobDestroy(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

	void OpenDoor(bool bNeedToReverse);
	void CloseDoor();
	void LockTheDoor();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
};
