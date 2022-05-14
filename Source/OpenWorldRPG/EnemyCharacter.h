// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	EAS_Dead		UMETA(DisplayName = "Dead"),
	EAS_Normal		UMETA(DisplayName = "Normal"), //Patrol, Peace상태
	EAS_Search		UMETA(DisplayName = "Search"), //뭔가를 듣거나, Player를 수색중일때
	EAS_Attack		UMETA(DisplayName = "Attack"), //Player가 시야에 보일때

	EAS_MAX		UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UPROPERTY(EditDefaultsOnly, Category = AI)
	class UBehaviorTree* BTAsset; //AIController의 BTComp에 넣어주기 위함.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enums")
	EAIStatus AIStatus;

	UPROPERTY(EditAnywhere, Category = AI)
	bool bHasPatrolPoints;

	UPROPERTY(EditAnywhere, Category = AI)
	TArray<class ATargetPoint*> TargetPoints; //정찰 루트

	UPROPERTY(EditAnywhere, Category = AI)
	float RandomPatrolRadius; //무작위 정찰 포인트를 얻는 범위(Origin Pos기준)

	UPROPERTY(EditAnywhere, Category = AI)
	float PatrolAcceptableRadius; //정찰 포인트의 도달 허용 범위

	UPROPERTY(EditAnywhere, Category = AI)
	float SightMaxAge; //시야 식별후 사라졌을때 최대 인내?시간

	UPROPERTY(EditAnywhere, Category = AI)
	float HearingMaxAge; //소음 식별후 사라졌을때 최대 인내?시간

	UPROPERTY(EditAnywhere, Category = Combat)
	float Range; // Enemy의 사거리 -> 무기의 Range로 교체 할 예정임.

	//Enemy의 최대, 최소 속도
	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxWalkSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinWalkSpeed = 300.f;
	


	/******* 디버깅용 ********/
	UPROPERTY(VisibleAnywhere, Category = AI)
	bool bSeePlayer;

	UPROPERTY(VisibleAnywhere, Category = AI)
	bool bHearPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
