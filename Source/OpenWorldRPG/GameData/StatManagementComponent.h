// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatManagementComponent.generated.h"

/*
* 필요한것들.
* Load Exp
* Save Exp
* 
* Add Exp
* Level Up
* Update Widget
* 
* 음..여기서 CurrentMaxStamina는 MainHud의 StatWidget에서 사용해야됨
* CurrentMAXJumpZ는 MainChar의 MovementComponent에 넘겨줘야됨.
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPZero);

class AMainController;
class ABaseCharacter;

class USoundCue;
struct FStrengthStats;


USTRUCT(BlueprintType)
struct FCharacterCurrentStat
{
	GENERATED_BODY()
public:
	// Stats
	//체력은 일단 100고정이고/ 체력을 제외한 스텟은 DataTable에서 가져온다.

	float Health = 100.f;

	float Stamina = 100.f;
	float StaminaDrainRate = 70.f;
	float StaminaRecoveryRate = 70.f;
	float StaminaDrainLine = 75.f;
	/*float RunningSpeed = 300.f;
	float SprintingSpeed = 500.f;
	float JumpMaximum = 400.f;*/
};



UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal				UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum		UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted			UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovery	UMETA(DisplayName = "ExhaustedRecovery"),
	ESS_Recovery			UMETA(DisplayName = "Recovery"),

	ESS_MAX					UMETA(DisplayName = "DefaultsMAX")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UStatManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatManagementComponent();
	
	FOnStaminaChange OnStaminaChange;
	FOnHPChange	OnHPChange;
	FOnHPZero OnHPZero;

	UPROPERTY(EditInstanceOnly, Category = "Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Sound | Breath")
	USoundCue* Stam_BelowMinimumSound;
	UPROPERTY(EditAnywhere, Category = "Sound | Breath")
	USoundCue* Stam_ExhaustedSound;
	UPROPERTY(EditAnywhere, Category = "Sound | Breath")
	USoundCue* Stam_ExhaustedRecoverySound;
	UPROPERTY(EditAnywhere, Category = "Sound | Breath")
	USoundCue* Stam_RecoverySound;
	UPROPERTY(EditAnywhere, Category = "Sound | Damage")
	USoundCue* HurtSound;


	FStrengthStats* CurrentMAXStrengthStats;
private:
	
	FCharacterCurrentStat CurrentStat;
	UPROPERTY(EditInstanceOnly, Category = "Stats | Strength")
	int32 StrengthLevel = 1;
	//UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stats | Strength")
	//float CurrentMAXStamina;
	//UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stats | Strength")
	//float CurrentMAXJumpZ;

	/* Enums */
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	EStaminaStatus StaminaStatus;


protected:
	virtual void BeginPlay() override;

public:	
	/*Initializes the component.Occurs at level startup or actor spawn.This is before BeginPlay(Actor or Component).
	* All Components in the level will be Initialized on load before any Actor / Component gets BeginPlay
	* Requires component to be registered, and bWantsInitializeComponent to be true. */
	virtual void InitializeComponent() override;

	void StrengthLevelUp(int32 SetLevel);
	FORCEINLINE EStaminaStatus GetStaminaStatus() {return StaminaStatus;}
	void StaminaManage(bool bIsSprintKeyDown);
	float GetStaminaRatio();
	float GetHealthRatio();

	void DamageApply(float Damage);

private:
	//FORCEINLINE void SetStaminaStatus(EStaminaStatus SetStatus) { StaminaStatus = SetStatus; }
	void SetStaminaStatus(EStaminaStatus NewStatus);
		
	void StaminaBreathSoundManager(EStaminaStatus CurrentStatus);
	void TakeDamageSound();
};
