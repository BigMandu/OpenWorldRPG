// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "OpenWorldRPG/MainCharacter.h" // "../MainCharacter.h" �ص� ������, �����η� ����Ѵ�.

void UFood::Use(class AMainCharacter* Character)
{
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Food"));
	}
}