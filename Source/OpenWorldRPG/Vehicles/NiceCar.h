// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/GameData/VehicleDataTable.h"
#include "OpenWorldRPG/GameData/CommonImpactEffectTable.h"
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
	USpringArmComponent* FPSpringArm;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DefaultSetting)
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

	UPROPERTY()
	struct FCommonImpactEffectTable CIED;

	UPhysicalMaterial* SlipperyMaterial;
	UPhysicalMaterial* NonSlipperyMaterial;

	TWeakObjectPtr<class ABaseCharacter> CurrentHaveSeatChar;

	TArray<FVector> WheelPos;

	FTimerHandle EngineIgnitionTimer;

	FTimerHandle FirstLockedCamTimer;
	FTimerHandle CamRotTimer;


	bool bUsingFPSCam = false;
	bool bIsOverlapDoor = false;
	bool bIsOverlapTrunk = false;

	bool bIsLowFriction;
	bool bIsStuck;
	bool bIsInAir;
	//bool bIsInputThrottle;

	float EngineIgnitionTime;
	bool bIsEngineStarting;

	float RotTime;
	float RotAlphaTime;
	float CamFixTime;
	bool bPitchIsZero;
	bool bYawIsZero;
	bool bIsStillNoInput;

	bool bFirstFixed;

	

	void VehicleSetting(struct FVehicleDataTable Data);
	void TWDSetting(FVehicleDataTable Data);
	void FWDSetting(FVehicleDataTable Data);

	void UpdatePhysicsMaterial();

	void StorageSetting(FVehicleDataTable Data);

	bool CanGetout(const FVector DoorOrigin, const FVector WantToGetOutPos, FVector HalfBoxSize);

	/** If vehicle is in air set the bIsInAir variable to True*/
	void CheckIsStuck();

	bool CheckThreeSurfaceHit();

	void EngineStart();

	void LockCameraRoll();
	void CanLockedCam();
	void LockedCam();

	//Mandatory Clear All timer relative Locked Cam
	void ManClearAllTimer();
	//Clear Only FirstLockCamTimer
	void ClearFirstLockedCamTimer();
	//Clear only RotTimer
	void CanClearRotTimer();

	FVector GetWheelLocation(const FWheelSetup& WheelSetup);

	/** Need to enable collision in the Physics asset */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	
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

	void InOutCar();
	//this function called from AMainController::GetIntheCar
	void SetCharacter(ABaseCharacter* TakeSeatCharacter);
	void UnSetCharacter();

	void GetoutCar();
	void OpenTrunk();

	virtual void SetOutline() override;
	virtual void UnsetOutline() override;
	virtual void SetMotherSpawnVolume(class ASpawnVolume* Var_MotherVolume) override;


	void PressHandbrake();
	void ReleaseHandbrake();

	/* when player died player's vehicle need to back to the saved point. */
	void TeleportVehicle(FTransform& DesignatedLocation);
	

	UFUNCTION()
	void DoorOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DoorOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void TrunkOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void TrunkOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};

PRAGMA_ENABLE_DEPRECATION_WARNINGS
