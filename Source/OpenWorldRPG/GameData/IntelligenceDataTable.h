// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "IntelligenceDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FIntelligenceDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Intelligence)
	EIntelType IntelType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Intelligence)
	int32 PageCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Intelligence)
	UTexture2D* IntelLogo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
	struct FSlateFontInfo HeadlineFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
	FSlateFontInfo ContentFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
	FSlateColor TextColor;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeadLine)
	FText ClassificationStamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeadLine)
	FText AdditionalStamp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents)
	FText Date;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents)
	FText To;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents)
	FText From;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents)
	FText ReferenceORThrough;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents)
	FText Subject;

	//this is must same as PageCount.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Contents, meta = (MultiLine = true))
	TArray<FText> Context;
};
