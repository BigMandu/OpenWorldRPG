// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "EnemyCharacter.generated.h"

class AEquipment;

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	EAS_Dead		UMETA(DisplayName = "Dead"),
	EAS_Patrol		UMETA(DisplayName = "Patrol"),//Patrol 상태 -> 최저 이동속도.
	EAS_Normal		UMETA(DisplayName = "Normal"), //Peace상태.
	EAS_Scout		UMETA(DisplayName = "Scout"), //뭔가를 듣거나, Player를 수색중일때 ->최저 이동속도
	EAS_Attack		UMETA(DisplayName = "Attack"), //Player가 시야에 보일때

	EAS_MAX		UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API AEnemyCharacter : public ABaseCharacter
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
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetAIStatus(EAIStatus Status);

	bool CheckEquipped(AActor* Actor);
};
