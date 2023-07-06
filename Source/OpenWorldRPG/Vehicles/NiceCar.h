// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/GameData/VehicleDataTable.h"
#include "NiceCar.generated.h"

/**
 * 
 */
PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS()
class OPENWORLDRPG_API ANiceCar : public AWheeledVehicle, public IInteractive_Interface
{
	GENERATED_BODY()

public:
	ANiceCar();

	FName SeatSocketName = "SeatSocket";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USkeletalMeshComponent* SKMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* TPSCam;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FPScam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	class UAudioComponent* EngineSoundComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	UAudioComponent* TireRollingSoundComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	UAudioComponent* SkidSoundComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* DoorCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TrunkCollision;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DB)
	class UDataTable* VehicleData;*/	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DB)
	struct FDataTableRowHandle VehicleRowHandle;

	UPROPERTY()
	class UItemStorageObject* StorageObj;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class ULootWidgetComponent* LootWidgetComp;

private:
	UPROPERTY()
	UDataTable* VehicleDataTable;
	UPROPERTY()
	struct FVehicleDataTable VD;

	bool bUsingFPSCam = false;

	bool bIsOverlapDoor = false;
	bool bIsOverlapTrunk = false;
	
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComp) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void CamLookPitch(float Val);
	void CamLookYaw(float Val);
	void MoveForward(float Val);
	void MoveRight(float Val);

	void ChangeCamera();

	void TrySave();

	virtual void Interaction(AActor* Actor) override;
	void GetoutCar();
	void OpenTrunk();

	virtual void SetOutline() override;
	virtual void UnsetOutline() override;


	void PressHandbrake();
	void ReleaseHandbrake();

	UFUNCTION()
	void DoorOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DoorOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void TrunkOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void TrunkOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



private:
	void VehicleSetting(struct FVehicleDataTable Data);
	void TWDSetting(FVehicleDataTable Data);
	void FWDSetting(FVehicleDataTable Data);
	
	void StorageSetting(FVehicleDataTable Data);

	bool CanGetout(const FVector DoorOrigin, const FVector WantToGetOutPos, FVector HalfBoxSize);

	bool CheckThreeSurfaceHit();
	
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS
