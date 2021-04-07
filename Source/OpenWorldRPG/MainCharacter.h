// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "MainCharacter.generated.h"

UCLASS()
class OPENWORLDRPG_API AMainCharacter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()
public:
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAccelerating;

	/**********   카메라 관련  *************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;


	class UMainAnimInstance* MainAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	class USoundCue* StepSoundCue;


	/**************    Perception Source 관련   ******************/
	class UAIPerceptionStimuliSourceComponent* StimuliSourceComp;
	TSubclassOf<class UAISense_Sight> Sight;
	TSubclassOf<class UAISense_Hearing> Hearing;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StepSound();

	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;

};
