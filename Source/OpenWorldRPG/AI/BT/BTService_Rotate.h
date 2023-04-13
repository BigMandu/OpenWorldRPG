// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Rotate.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UBTService_Rotate : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_Rotate();

private:
	FTimerHandle RightHandle;
	FTimerHandle LeftHandle;

	bool IsReachedTargetRightRot;
	bool IsReachedTargetLeftRot;

	float TraceAlpha;
	float TraceDeltaTime;

	float RotateAlpha;
	float RotateDeltaTime;


	struct FBTRotateMemory
	{
		float ClearAlpha;
		float ClearDeltatime;
		float TraceAlpha;
		float TraceDeltatime;
	};
	

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
	void ClearFocusAndChangeRot(class AEnemyAIController* AICon, class AEnemyCharacter* AIChar);
	void TraceAndSetFocus(AEnemyAIController* AICon, AEnemyCharacter* AIChar);
};
