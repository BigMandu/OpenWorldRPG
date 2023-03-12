// Fill out your copyright notice in the Description page of Project Settings.

#include "NorthPoint.h"
#include "Components/ArrowComponent.h"

// Sets default values
ANorthPoint::ANorthPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NorthArrow"));
}

// Called when the game starts or when spawned
void ANorthPoint::BeginPlay()
{
	Super::BeginPlay();
	
}


