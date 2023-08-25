// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAILibrary.h"
#include "EnemyCharacter.h"


//8방향 linetrace
bool UCustomAILibrary::CheckIsInside(const AEnemyCharacter& AIChar)
{
	bool bReturnBool = false;
	const FVector FwVector = AIChar.GetActorForwardVector();

	TArray<bool> HitArray;
	int32 HitCount = 0;
	//FVector DirectionVector = FwVector;
	for (int32 dir = 0; dir < 8; ++dir)
	{
		float AngleDegree = dir*45.f;

		FRotator RotationYaw = FRotator(0.f, AngleDegree, 0.f);

		FVector DirectionVector = RotationYaw.RotateVector(FwVector);

		FVector LineTraceEndVector = DirectionVector + AIChar.GetActorLocation() * 888.f;

		FHitResult Hit;
		FCollisionQueryParams CParams = FCollisionQueryParams(FName(TEXT("AI_Chk_Inside_LineTrace")),false, &AIChar);
		AIChar.GetWorld()->LineTraceSingleByChannel(Hit, DirectionVector, LineTraceEndVector, ECollisionChannel::ECC_Visibility, CParams);

		HitArray.Add(Hit.IsValidBlockingHit());
	}

	for (bool bHit : HitArray)
	{
		if (bHit)
		{
			++HitCount;
		}
	}

	if (HitCount > 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("UCustomInventoryLibrary::CheckIsInside// Inside!! "));
		
		bReturnBool = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::CheckIsInside// Outside!! "));
	}

	return bReturnBool;
}