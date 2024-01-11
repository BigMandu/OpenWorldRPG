// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/GrenadeSmoke.h"


AGrenadeSmoke::AGrenadeSmoke() : Super()
{
	SmokeEffectVisibleBlock = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBlockSphereComp"));
	SmokeEffectVisibleBlock->SetupAttachment(GetRootComponent());	
	
	SmokeEffectVisibleBlock->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SmokeEffectVisibleBlock->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SmokeEffectVisibleBlock->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	SmokeEffectVisibleBlock->SetBoxExtent(FVector(3.f));
	SmokeEffectVisibleBlock->SetGenerateOverlapEvents(false);
	SmokeEffectVisibleBlock->SetHiddenInGame(false);


	SpriteSize_MIN = FName("Maximum_Sprite Size_Value");
	SpriteSize_MAX = FName("Minimum_Sprite Size_Value");
	SpriteColorRGBfloat = FName("Scale RGB");
	SpriteSpawnRate = FName("SpawnRate");
	SpriteSphereRadius = FName("Sphere Radius");
}

void AGrenadeSmoke::BeginEffect()
{
	Super::BeginEffect();

	UGrenadePDA* GPDA = Cast<UGrenadePDA>(this->ItemSetting.DataAsset);
	if (GPDA == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("AGrenadeBase:: SmokeEffect"));

	
	if (SmokeEffectVisibleBlock)
	{
		if (GPDA->Ni_GrenadeEffect)
		{
			Ni_ParticleComp->SetFloatParameter(SpriteSize_MIN, GPDA->SmokeSpriteSizeMin);
			Ni_ParticleComp->SetFloatParameter(SpriteSize_MAX, GPDA->SmokeSpriteSizeMax);
			Ni_ParticleComp->SetFloatParameter(SpriteSpawnRate, GPDA->SmokeSpawnRate);
			Ni_ParticleComp->SetFloatParameter(SpriteSphereRadius, GPDA->SmokeRadius);
			Ni_ParticleComp->SetVectorParameter(SpriteColorRGBfloat, GPDA->SmokeColorRGBFloat);

			VisibleBlock();

		}
		else if (GPDA->Ca_GrenadeEffect)
		{

		}
		//SmokeEffectVisibleBlockSphere->SetSphereRadius(gPDA->)
	}



	GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &AGrenadeBase::EndEffect, GPDA->SmokeDuration, false);
	
}

void AGrenadeSmoke::VisibleBlock()
{
	FBox BoundBox = Ni_ParticleComp->GetAsset()->GetFixedBounds();
	
	FVector NiLo = Ni_ParticleComp->GetComponentLocation();
	FVector BoxExtent = BoundBox.GetExtent();

	FVector BoxLocation = FVector(NiLo.X + BoxExtent.X / 1.2, NiLo.Y, NiLo.Z + BoxExtent.Z / 2);

	//DrawDebugBox(GetWorld(), BoxLocation, BoundBox.GetExtent(), FRotator::ZeroRotator.Quaternion(), FColor::Green, false, GPDA->SmokeDuration, 0, 4.f);
		
	SmokeEffectVisibleBlock->SetBoxExtent(BoxExtent);
	SmokeEffectVisibleBlock->SetWorldLocationAndRotation(BoxLocation, FRotator::ZeroRotator);

	//StimuliComp->RegisterForSense()
	//FAISightEvent SightEvt = FAISightEvent(,)
	//UAISense_Sight::RegisterEvent()
}