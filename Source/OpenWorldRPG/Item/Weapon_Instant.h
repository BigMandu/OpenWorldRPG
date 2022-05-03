// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Weapon_Instant.generated.h"

/**
 * 
 */
USTRUCT()
struct FBulletData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Stat")
	int32 Damage;


	/* FBulletData Defaults */
	FBulletData()
	{
		Damage = 10;
	}
};

UCLASS()
class OPENWORLDRPG_API AWeapon_Instant : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeapon_Instant();

	UPROPERTY(EditDefaultsOnly, Category = "WeaponStat")
	FBulletData BulletStat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* BulletHitEffect;
	
	
	
protected:
	float PitchValue;
	float YawValue;
	float WorldTime;
	
	
public:

	
	virtual void New_BulletOut() override;
	void ApplyRecoil();
	void CheckHit(FHitResult& Hit);



	/********************************************/
	/************* 아래는 사용하지 않음 ***********/
	/********************************************/

	//1~3발 동안 탄착지점을 올린뒤, 이후 탄착점을 위함.
	//float LastZpos;

	/*FVector PreviousSpread;
	FVector NextSpread;*/

	
	//virtual void Old_BulletOut() override;
	//FVector BulletSpread(FVector Vec);
	//void CalcRecoilNApply(FVector *PreSpread, FVector *NexSpread);

	//float PitchRecoilValue(float Zvalue);
	//float YawRecoilValue(FVector Vec);
	
};
