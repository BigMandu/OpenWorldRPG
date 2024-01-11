// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CommonImpactEffectTable.generated.h"

/**
 * 
 */
//class OPENWORLDRPG_API CommonImpactEffectTable
//{
//public:
//	CommonImpactEffectTable();
//	~CommonImpactEffectTable();
//};

USTRUCT(BlueprintType)
struct FCommonImpactEffectTable: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
		/** default impact FX used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	class UNiagaraSystem* DefaultFX;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* ConcreteFX;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* MetalFX;

	/** impact FX on soil */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* SoilFX;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* DirtFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GrassFX;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GlassFX;

	/** impact FX on tile */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* TileFX;

	/** impact FX on carpet */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* CarpetFX;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WoodFX;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WaterFX;

	/** impact FX on scrap */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* ScrapFX;
	
	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* FleshFX;

	/////////////////////////////////////////////////////
	/*                   Sound                         */
	/////////////////////////////////////////////////////

	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	class USoundCue* DefaultSound;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ConcreteSound;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* MetalSound;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* SoilSound;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* DirtSound;
		
	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GrassSound;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GlassSound;

	/** impact FX on tile */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* TileSound;

	/** impact FX on carpet */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* CarpetSound;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WoodSound;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WaterSound;

	/** impact FX on scrap */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ScrapSound;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FleshSound;

};