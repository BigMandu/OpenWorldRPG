#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VehicleDataTable.generated.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS

class USkeletalMesh;
class UVehicleWheel;
class USoundCue;

USTRUCT(BlueprintType)
struct FVehicleDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DefaultSetting)
	struct FDataTableRowHandle ImpactEffectRowHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	FTransform DoorCollisionTF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	FTransform TrunkCollisionTF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpringArmLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FPSCamLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform SeatSocketTF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail option")
	bool bIs4WD;
	//질량 kg단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail option")
	float Mass;
	//항력계수 f1 = 0.81, 양산차 0.27~9
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail option")
	float Dragcoefficient;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail option")
	float MaxRPM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail option")
	FVector CenterOfMass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	TSubclassOf<class UFrontWheel> FrontWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	TSubclassOf<class URearWheel> RearWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	FName FRWheelBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	FName FLWheelBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	FName RRWheelBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	FName RLWheelBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float WheelRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float WheelWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float WheelMass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float SteerAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CharacterSeatAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> AnimInst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* VehicleSKMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EngineStartSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EngineOffSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EngineRPMSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* TireRollingSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* SkidSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FVector EngineSoundLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FVector TireSoundLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FVector SkidSoundLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trunk")
	int32 TrunkSizeRow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trunk")
	int32 TrunkSizeColumn;
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS