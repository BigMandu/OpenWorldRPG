// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "BaseCharacter.generated.h"

class UMainAnimInstance;
class UNewInventoryComponent;
class UEquipmentComponent;
class USoundCue;


class ALootBox;
class AWeapon;

UENUM()
enum class ETeamType : uint8
{
	ETT_Blue	UMETA(DisplayName = "Blue"),
	ETT_Red		UMETA(DisplayName = "Red"),

	ETT_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
	EPS_Dead		UMETA(DisplayName = "Dead"),
	EPS_Crouch		UMETA(DisplayName = "Crouch"),
	EPS_Normal		UMETA(DisplayName = "Normal"),
	EPS_Walk		UMETA(DisplayName = "Walk"),
	EPS_Sprint		UMETA(DisplayName = "Sprint"),

	EPS_MAX		UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class OPENWORLDRPG_API ABaseCharacter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	

	/* Socket Name */
	const FName HeadSocketName = FName("headsocket");
	const FName GripSocketName = FName("WeaponGrip");
	const FName WeaponLeftHandSocketName = FName("LeftHandPos");

	/* Enums */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECharacterStatus ChracterStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	ETeamType TeamType;


	/* Movement */
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MaxWalkSpeed = 600.f; //¶Ù´Â ¼Óµµ
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MinWalkSpeed = 300.f; //°È´Â ¼Óµµ

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bIsAim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	USoundCue* StepSoundCue;


	/* Weapon */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* SubWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PistolWeapon;

	/*  Components */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UNewInventoryComponent* InventoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UEquipmentComponent* Equipment;

	/* Anim Instance */
	UMainAnimInstance* TPAnimInstance;
	UMainAnimInstance* FPAnimInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	
	void SetCharacterStatus(ECharacterStatus Type);
	void SetTeamType(ETeamType Team);
	FORCEINLINE ETeamType GetTeamType() { return TeamType; }


	void SetEquippedWeapon(AWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	FTransform LeftHandik();

	void StepSound();

	/********** Perception ********/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const;


};
