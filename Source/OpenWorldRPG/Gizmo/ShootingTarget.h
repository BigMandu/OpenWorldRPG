// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "ShootingTarget.generated.h"

UENUM(BlueprintType)
enum class ERotateDirection: uint8
{
	ERD_Pitch		UMETA(DisplayName = "Pitch"),
	ERD_Yaw			UMETA(DisplayName = "Yaw"),
	ERD_Roll		UMETA(DisplayName = "Roll"),
};

UCLASS()
class OPENWORLDRPG_API AShootingTarget : public AActor, public IMissionInterface
{
	GENERATED_BODY()

	FTimerHandle PopupTimer;

	FRotator InitialRotation;
	FRotator TargetRotation;

	bool bHitHead;
	bool bHitBody;

	int32 TotalScore;
	int32 HitCount;

public:	
	// Sets default values for this actor's properties
	AShootingTarget();

	UPROPERTY(EditAnywhere, Category = "Target")
	class UStaticMeshComponent* RootMeshComp;
	UPROPERTY(EditAnywhere, Category = "Target")
	UStaticMeshComponent* RotationMeshComp;

	UPROPERTY(EditAnywhere, Category = "Target")
	FString RelatedMissionUniqID;

	/* Rotation Optional Mesh */
	UPROPERTY(EditAnywhere, Category = "Target")
	class UShootingTargetSMComponent* Body_10P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Body_8P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Body_6P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Body_5P;

	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Head_10P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Head_8P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Head_5P;

	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* LeftArm_4P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* RightArm_4P;

	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Leg_4P;
	UPROPERTY(EditAnywhere, Category = "Target")
	UShootingTargetSMComponent* Leg_0P;


	UPROPERTY(EditAnywhere, Category = "Target")
	ERotateDirection RotateDirection;
	UPROPERTY(EditAnywhere, Category = "Target")
	float RotateValue = 90.f;

	UPROPERTY(EditAnywhere, Category = "Target")
	float PopupSpeed = 7.f;


	bool bIsPopUpStatus;

	/* for Score*/
	UPROPERTY(EditAnywhere, Category = "ScoreCollision")
	class UBoxComponent* SkullBox;

	UPROPERTY(EditAnywhere, Category = "ScoreCollision")
	UBoxComponent* ChestBox;

	UPROPERTY(EditAnywhere, Category = "ScoreCollision")
	UBoxComponent* StomachBox;
	
	



private:
	bool CanClearTimer(FRotator CurRot, FRotator TargetRot);
	void ClearRotTimer();

	void LerpRotation(FRotator TRot);

	void ResetTarget();

	bool CheckTripleTap();

	void SetScore(int32 CurScore);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	/*virtual void Tick(float DeltaTime) override;*/
	virtual void PostInitializeComponents() override;

	void InitializeTarget();

	//UFUNCTION()
	void StartPopUp();//UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void ShowNotifyIcon();
	virtual void HideNotifyIcon();
};
