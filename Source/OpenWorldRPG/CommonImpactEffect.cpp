// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonImpactEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"


// Sets default values
ACommonImpactEffect::ACommonImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
	SetRootComponent(DefaultRootComponent);

	SetAutoDestroyWhenFinished(true);

}

void ACommonImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//SetActorTransform(ActorTF);
	//SetActorLocation(FVector(100.f,100.f,100.f));

	UE_LOG(LogTemp,Warning,TEXT("CIE lo : %s"), *GetActorLocation().ToString());

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SetEffect, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, SetSoundCue, GetActorLocation());
}


void ACommonImpactEffect::SetImpactEffect(struct FCommonImpactEffectTable EffectDT, FHitResult HitInfo, FVector HitForce, FTransform CIETF)
{
	UPhysicalMaterial* HitPhysMat = HitInfo.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	ActorTF = CIETF;

	if (UNiagaraSystem* ImpactEffect = UCustomSystemLibrary::GetImpactFX(EffectDT, HitSurfaceType))
	{
		SetEffect = ImpactEffect;
	}

	if (USoundCue* ImpactSound = UCustomSystemLibrary::GetImpactSound(EffectDT, HitSurfaceType))
	{
		SetSoundCue = ImpactSound;
	}

	
}

/*
UNiagaraSystem* ACommonImpactEffect::GetImpactFX(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType)
{
	UNiagaraSystem* ReturnEffect = nullptr;

	switch (MaterialType)
	{
		case PHYSICS_SURFACE_CONCRETE:
			ReturnEffect = EffectData.ConcreteFX;
			break;
		case PHYSICS_SURFACE_METAL:
			ReturnEffect = EffectData.MetalFX;
			break;
		case PHYSICS_SURFACE_SOIL:
			ReturnEffect = EffectData.SoilFX;
			break;
		case PHYSICS_SURFACE_DIRT:
			ReturnEffect = EffectData.DirtFX;
			break;
		case PHYSICS_SURFACE_GRASS:
			ReturnEffect = EffectData.GrassFX;
			break;
		case PHYSICS_SURFACE_GLASS:
			ReturnEffect = EffectData.GlassFX;
			break;
		case PHYSICS_SURFACE_TILE:
			ReturnEffect = EffectData.TileFX;
			break;
		case PHYSICS_SURFACE_CARPET:
			ReturnEffect = EffectData.CarpetFX;
			break;
		case PHYSICS_SURFACE_WOOD:
			ReturnEffect = EffectData.WoodFX;
			break;
		case PHYSICS_SURFACE_WATER:
			ReturnEffect = EffectData.WaterFX;
			break;
		case PHYSICS_SURFACE_SCRAP:
			ReturnEffect = EffectData.ScrapFX;
			break;
		case PHYSICS_SURFACE_FLESH:
			ReturnEffect = EffectData.FleshFX;
			break;
	}

	return ReturnEffect ? ReturnEffect : EffectData.DefaultFX;
}

USoundCue* ACommonImpactEffect::GetImpactSound(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType)
{
	USoundCue* ReturnSound = nullptr;

	switch (MaterialType)
	{
	case PHYSICS_SURFACE_CONCRETE:
		ReturnSound = EffectData.ConcreteSound;
		break;
	case PHYSICS_SURFACE_METAL:
		ReturnSound = EffectData.MetalSound;
		break;
	case PHYSICS_SURFACE_SOIL:
		ReturnSound = EffectData.SoilSound;
		break;
	case PHYSICS_SURFACE_DIRT:
		ReturnSound = EffectData.DirtSound;
		break;
	case PHYSICS_SURFACE_GRASS:
		ReturnSound = EffectData.GrassSound;
		break;
	case PHYSICS_SURFACE_GLASS:
		ReturnSound = EffectData.GlassSound;
		break;
	case PHYSICS_SURFACE_TILE:
		ReturnSound = EffectData.TileSound;
		break;
	case PHYSICS_SURFACE_CARPET:
		ReturnSound = EffectData.CarpetSound;
		break;
	case PHYSICS_SURFACE_WOOD:
		ReturnSound = EffectData.WoodSound;
		break;
	case PHYSICS_SURFACE_WATER:
		ReturnSound = EffectData.WaterSound;
		break;
	case PHYSICS_SURFACE_SCRAP:
		ReturnSound = EffectData.ScrapSound;
		break;
	case PHYSICS_SURFACE_FLESH:
		ReturnSound = EffectData.FleshSound;
		break;
	}

	return ReturnSound ? ReturnSound : EffectData.DefaultSound;
}
*/