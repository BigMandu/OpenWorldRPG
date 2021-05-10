// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "OpenWorldRPG/MainCharacter.h" // "../MainCharacter.h" 해도 되지만, 절대경로로 사용한다.

void UFood::Use(class AMainCharacter* Character)
{
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Food"));
	}
}