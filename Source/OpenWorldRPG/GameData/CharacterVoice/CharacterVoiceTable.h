// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterVoiceTable.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharacterVoiceTable: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	FString MissionUniqueID;
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	int32 VoiceNum;
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	int32 MaxVoiceNum;
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	class USoundCue* Voice;
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	FText VoiceText;
	UPROPERTY(EditAnywhere, Category = CharacterVoice)
	class UAnimMontage* Act;


};