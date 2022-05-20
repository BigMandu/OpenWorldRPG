// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	EAS_Dead		UMETA(DisplayName = "Dead"),
	EAS_Patrol		UMETA(DisplayName = "Patrol"),//Patrol ���� -> ���� �̵��ӵ�.
	EAS_Normal		UMETA(DisplayName = "Normal"), //Peace����.
	EAS_Scout		UMETA(DisplayName = "Scout"), //������ ��ų�, Player�� �������϶� ->���� �̵��ӵ�
	EAS_Attack		UMETA(DisplayName = "Attack"), //Player�� �þ߿� ���϶�

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
	class UBehaviorTree* BTAsset; //AIController�� BTComp�� �־��ֱ� ����.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enums")
	EAIStatus AIStatus;

	UPROPERTY(EditAnywhere, Category = AI)
	bool bHasPatrolPoints;

	UPROPERTY(EditAnywhere, Category = AI)
	TArray<class ATargetPoint*> TargetPoints; //���� ��Ʈ

	UPROPERTY(EditAnywhere, Category = AI)
	float RandomPatrolRadius; //������ ���� ����Ʈ�� ��� ����(Origin Pos����)

	UPROPERTY(EditAnywhere, Category = AI)
	float PatrolAcceptableRadius; //���� ����Ʈ�� ���� ��� ����

	UPROPERTY(EditAnywhere, Category = AI)
	float SightMaxAge; //�þ� �ĺ��� ��������� �ִ� �γ�?�ð�

	UPROPERTY(EditAnywhere, Category = AI)
	float HearingMaxAge; //���� �ĺ��� ��������� �ִ� �γ�?�ð�

	UPROPERTY(EditAnywhere, Category = Combat)
	float Range; // Enemy�� ��Ÿ� -> ������ Range�� ��ü �� ������.


	/******* ������ ********/
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
};
