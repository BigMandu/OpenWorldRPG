// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class OPENWORLDRPG_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UPROPERTY(EditDefaultsOnly, Category = AI)
	class UBehaviorTree* BTAsset; //AIController�� BTComp�� �־��ֱ� ����.

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
	float Range; //������ Range�� ��ü �� ������.

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
