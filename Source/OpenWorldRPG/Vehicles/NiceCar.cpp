// Fill out your copyright notice in the Description page of Project Settings.


#include "NiceCar.h"
#include "FrontWheel.h"
#include "RearWheel.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/GameData/VehicleDataTable.h"
#include "OpenWorldRPG/WorldControlVolume/SpawnVolume.h"

#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"

//#include "WheeledVehicleMovementComponent.h"
//#include "WheeledVehicleMovementComponent4W.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS
ANiceCar::ANiceCar()
{
	
	//DataTable'/Game/GameData/VehicleDataTable.VehicleDataTable'
	FString VehicleDataTablePath = TEXT("/Game/GameData/VehicleDataTable.VehicleDataTable");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_VehicleData(*VehicleDataTablePath);
	if (DT_VehicleData.Succeeded())
	{
		VehicleDataTable = DT_VehicleData.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> Slippery(TEXT("/Game/Vehicles/Slippery.Slippery"));
	if (Slippery.Succeeded())
	{
		SlipperyMaterial = Slippery.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlippery(TEXT("/Game/Vehicles/NonSlippery.NonSlippery"));
	if (NonSlippery.Succeeded())
	{
		NonSlipperyMaterial = NonSlippery.Object;
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArm->SetRelativeLocation(FVector(0.f,0.f,200.f));
	SpringArm->SetupAttachment(GetRootComponent());

	SpringArm->TargetArmLength = 475.f;
	//camera smooth panning 
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 9.0f;
	SpringArm->CameraRotationLagSpeed = 10.0f;
	SpringArm->CameraLagMaxDistance = 115.f;

	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bUsePawnControlRotation = true;
	

	TPSCam = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamComp"));
	TPSCam->SetupAttachment(SpringArm);
	TPSCam->SetRelativeLocation(FVector(0.f,0.f,50.f));
	TPSCam->SetRelativeRotation(FRotator(-5.f,0.f,0.f));
	TPSCam->bUsePawnControlRotation = false;
	TPSCam->SetFieldOfView(90.f);


	FPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPSpringArmComp"));
	FPSpringArm->SetRelativeLocation(FVector(47.f, -40.f, 200.f));
	FPSpringArm->SetupAttachment(GetRootComponent());
	FPSpringArm->TargetArmLength = 0.f;
	FPSpringArm->bInheritPitch = true;
	FPSpringArm->bInheritYaw = true;
	FPSpringArm->bInheritRoll = true;
	FPSpringArm->bUsePawnControlRotation = true;

	FPScam = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamComp"));
	FPScam->SetupAttachment(FPSpringArm);
	FPScam->SetRelativeLocation(FVector(0.f));
	FPScam->bUsePawnControlRotation = false;	
	FPScam->SetFieldOfView(90.f);

	EngineSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundComp"));
	EngineSoundComp->SetupAttachment(GetRootComponent());
	EngineSoundComp->SetAutoActivate(false);

	SkidSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SkidSoundComp"));
	SkidSoundComp->SetupAttachment(GetRootComponent());
	SkidSoundComp->SetAutoActivate(false);
	TireRollingSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TireRollingSoundComp"));
	TireRollingSoundComp->SetupAttachment(GetRootComponent());
	TireRollingSoundComp->SetAutoActivate(false);


	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorCollision"));
	TrunkCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TrunkCollision"));


	DoorCollision->OnComponentBeginOverlap.AddDynamic(this,&ANiceCar::DoorOnOverlapBegin);
	DoorCollision->OnComponentEndOverlap.AddDynamic(this,&ANiceCar::DoorOnOverlapEnd);

	TrunkCollision->OnComponentBeginOverlap.AddDynamic(this,&ANiceCar::TrunkOnOverlapBegin);
	TrunkCollision->OnComponentEndOverlap.AddDynamic(this,&ANiceCar::TrunkOnOverlapEnd);

	LootWidgetComp = CreateDefaultSubobject<ULootWidgetComponent>(TEXT("LootWidgetComp"));
	LootWidgetComp->WidgetType = EWidgetType::EWT_LootBox;

	/* Wheels Setting*/
	//휠 할당 순서는 앞,뒤 상관없고 오직 Bone name과 wheel class에만 영향을 준다.
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	TSubclassOf<UVehicleWheel> FW = TSubclassOf<UVehicleWheel>(UVehicleWheel::StaticClass());

	Vehicle4W->WheelSetups[0].WheelClass = UFrontWheel::StaticClass(); //FW;	
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	//GetVehicleMovement()->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	//GetVehicleMovement()->WheelSetups[0].AdditionalOffset = FVector(0.f,-8.f,0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UFrontWheel::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	//GetVehicleMovement()->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = URearWheel::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	//GetVehicleMovement()->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = URearWheel::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	//GetVehicleMovement()->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);


	bIsLowFriction = false;	
}

void ANiceCar::Tick(float DeltaSeconds)
{
	if (!CurrentHaveSeatChar.IsValid()) return;

	Super::Tick(DeltaSeconds);
	
	/*float EngineRotSpd = GetVehicleMovement()->GetEngineRotationSpeed();
	EngineSoundComp->SetFloatParameter(FName("RPM"),EngineRotSpd);*/
	

	UpdatePhysicsMaterial();

	//Engine sound based on RPM
	float EngineRotSpd = 2500.f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComp->SetFloatParameter(FName("RPM"), GetVehicleMovement()->GetEngineRotationSpeed() * EngineRotSpd);

	/** Always lock the camera's roll. */
	LockCameraRoll();

	//if no longer input then, locked (hold) the camera in the forward direction.
	if (bPitchIsZero && bYawIsZero)
	{
		if (bIsStillNoInput)
		{
			if (!GetWorldTimerManager().IsTimerActive(CamRotTimer) && !GetWorldTimerManager().IsTimerActive(FirstLockedCamTimer))
			{
				//CamFixTime = 0.085f;
				LockedCam();
			}
			
		}
		else
		{
			CamFixTime = 2.5f;
			CanLockedCam();
		}
	}	

	CheckIsStuck();
	
}

void ANiceCar::SetupPlayerInputComponent(UInputComponent* InputComp)
{
	Super::SetupPlayerInputComponent(InputComp);
	check(InputComp)
	InputComp->BindAxis("Turn", this, &ANiceCar::CamLookYaw);
	InputComp->BindAxis("LookUp", this, &ANiceCar::CamLookPitch);
	InputComp->BindAxis("MoveForward",this,&ANiceCar::MoveForward);
	InputComp->BindAxis("MoveRight",this,&ANiceCar::MoveRight);
	
	InputComp->BindAction("Camera",IE_Pressed, this, &ANiceCar::ChangeCamera);
	

	InputComp->BindAction("Jump", IE_Pressed, this, &ANiceCar::PressHandbrake);
	InputComp->BindAction("Jump", IE_Released, this, &ANiceCar::ReleaseHandbrake);
	InputComp->BindAction("Reload", IE_Pressed, this, &ANiceCar::TrySave);

	InputComp->BindAction("Interactive", IE_Pressed, this, &ANiceCar::GetoutCar);
	//InputComp->BindAction("Interactive", IE_Pressed, this, &ANiceCar::InOutCar);

}

void ANiceCar::OnConstruction(const FTransform& Transform)
{
	if (VehicleDataTable)
	{
		/** VechieRowHandle이라는 TableRowHandle 구조체를 이용해 Row를 찾는 과정. */
		VehicleRowHandle.DataTable = VehicleDataTable;
		FName WantRow = VehicleRowHandle.RowName;

		FString TableName = VehicleDataTable->GetName();

		if(FVehicleDataTable* GetData = (VehicleDataTable->FindRow<FVehicleDataTable>(WantRow,TableName)))
		{
			VD = *GetData;
			VehicleSetting(VD);
			if(GetMesh())
			{
				GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			}
		}



		//아래코드 안됨.
		//UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
		//if(!Vehicle4W) return;
		//WheelPos.Empty();
		//for (int32 WheelCnt = 0; WheelCnt < 4; ++WheelCnt)
		//{
		//	WheelPos.Add(GetWheelLocation(Vehicle4W->WheelSetups[WheelCnt]));
		//	//WheelPos[WheelCnt] = /*U2PVector*/GetWheelLocation(Vehicle4W->WheelSetups[WheelCnt]);
		//}
	}
}

void ANiceCar::VehicleSetting(FVehicleDataTable Data)
{
	//SetMesh(Data->VehicleSKMesh);
	//SKMesh->SetSkeletalMesh(Data->VehicleSKMesh);

	GetMesh()->SetSkeletalMesh(Data.VehicleSKMesh);
	GetMesh()->SetAnimInstanceClass(Data.AnimInst);
	SpringArm->SetRelativeLocation(Data.SpringArmLocation);
	SpringArm->TargetArmLength = Data.SpringArmLength;
	FPSpringArm->SetRelativeLocation(Data.FPSCamLocation);

	DoorCollision->SetRelativeTransform(Data.DoorCollisionTF);
	TrunkCollision->SetRelativeTransform(Data.TrunkCollisionTF);

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	/* Wheel Setting */
	Vehicle4W->WheelSetups[0].WheelClass = Data.FrontWheel;
	Vehicle4W->WheelSetups[0].BoneName = Data.FRWheelBoneName;
	Vehicle4W->WheelSetups[1].WheelClass = Data.FrontWheel;
	Vehicle4W->WheelSetups[1].BoneName = Data.FLWheelBoneName;

	Vehicle4W->WheelSetups[2].WheelClass = Data.RearWheel;
	Vehicle4W->WheelSetups[2].BoneName = Data.RRWheelBoneName;
	Vehicle4W->WheelSetups[3].WheelClass = Data.RearWheel;
	Vehicle4W->WheelSetups[3].BoneName = Data.RLWheelBoneName;

	
	for (int32 WheelIndex = 0; WheelIndex < 4; ++WheelIndex)
	{
		if (UFrontWheel* FWheel = Cast<UFrontWheel>(Vehicle4W->WheelSetups[WheelIndex].WheelClass.GetDefaultObject()))
		{
			FWheel->SettingWheel(Data);
		}
		else if (URearWheel* RWheel = Cast<URearWheel>(Vehicle4W->WheelSetups[WheelIndex].WheelClass.GetDefaultObject()))
		{
			RWheel->SettingWheel(Data);
		}
	}

	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	
	Vehicle4W->Mass = Data.Mass;
	Vehicle4W->DragCoefficient = Data.Dragcoefficient;
	Vehicle4W->MaxEngineRPM = Data.MaxRPM;

	//관성 세팅
	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);


	/*Sound Settings */
	EngineSoundComp->SetRelativeLocation(Data.EngineSoundLocation);
	TireRollingSoundComp->SetRelativeLocation(Data.TireSoundLocation);
	SkidSoundComp->SetRelativeLocation(Data.SkidSoundLocation);	

	EngineSoundComp->SetSound(Data.EngineRPMSound);
	EngineSoundComp->Stop();
	
	SkidSoundComp->SetSound(Data.SkidSound);
	SkidSoundComp->Stop();

	//자체 메쉬 중심점 설정
	GetMesh()->SetCenterOfMass(Data.CenterOfMass);

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(-1.0f, 0.0f, -5.0f);
		//UpdatedPrimitive->BodyInstance.
	}
	

	/* Detail Settings */
	if (Data.bIs4WD)
	{
		//4WD setting
		FWDSetting(Data);
	}
	else
	{
		//2WD setting
		TWDSetting(Data);
	}
	

	/* Storage Settings */
	StorageSetting(Data);

}

//Two Wheel drive
void ANiceCar::TWDSetting(FVehicleDataTable Data)
{
	//auto VMC = GetVehicleMovement();
	
	//VMC->MaxEngineRPM = 5800.f;
	
}
//Four Wheel drive
void ANiceCar::FWDSetting(FVehicleDataTable Data)
{
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	//Engine & Torque Setup
	//Vehicle4W->MaxEngineRPM = 6000.f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	//Steering Setup
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

	//구동방식 4wd
	/*Open Diff = 필요시 서로다른 속도로 회전함. (spider gear)
	* 직선구간 회전X, 코너링시 회전O
	* 
	* Limited Slip Diff(LSD), 고성능 디퍼런셜
	* 디퍼런셜 스스로 Fully-open / Fully-locked 사이에서 조절함.
	* 
	* maximum Locking = Fully Lock-up -> 하나의 액슬 처럼 작동.
	* Minimum Locking = FullyOpen -> 오픈 디퍼런셜 처럼 작동
	* 
	* LSD는
	* 한 쪽 타이어 트랙션 잃으면, 다른쪽 바퀴로 힘을 전달함 (슬립->스핀 방지)
	*/
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;

	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;
}


void ANiceCar::StorageSetting(FVehicleDataTable Data)
{
	StorageObj = NewObject<UItemStorageObject>();
	StorageObj->InitializeStorage(Data.TrunkSizeColumn, Data.TrunkSizeRow, 65.f);
}


void ANiceCar::CheckIsStuck()
{
	bool bCheckAllWheelOnGround_localval = true;
	bool bCanNotMove_localval = false;
	bool bCarWasFlip_localval = false;
	

	FVector ActorUpVec = GetActorUpVector();
	FVector WorldUpVector = FVector(0.f,0.f, 1.f);
	float JudgeFlipValue = FVector::DotProduct(ActorUpVec, WorldUpVector);

	if (JudgeFlipValue < 0.5)
	{
		bCarWasFlip_localval = true;
	}

	for (int32 WheelCnt = 0; WheelCnt < 4; ++WheelCnt)
	{
		UPhysicalMaterial* GroundMat = GetVehicleMovement()->Wheels[WheelCnt]->GetContactSurfaceMaterial();
		if (GroundMat == nullptr)
		{
			bIsInAir = true;
			bCheckAllWheelOnGround_localval = false;
			break;
		}
		else
		{
			bIsInAir = false;
		}
	}

	if (GetVehicleMovement()->GetForwardSpeed() < 0.5)
	{
		bCanNotMove_localval = true;
	}

	/** 바로바로 variable을 변경하니까 stuck recovery code가 제대로 수행안됨
	 *  원복을 시도 했다가 말았다가 반복함.
	 * 새로운 val을 두고, timer를 이용해야 할듯.
	 * 그런데 문제는 timer가 작동하는 동안 원복이 됐다면 즉시 val을 off시켜야 하는데 어떻게?
	 * 
	 */

	if (bCarWasFlip_localval /* && bCanNotMove_localval*/)
	{
		//Set the timer or instantly set the 'canrotate'
		bIsStuck = true;

	}
	else if ((bCheckAllWheelOnGround_localval == false) && bCanNotMove_localval)
	{
		//Set the timer or instantly set the 'canrotate'
		bIsStuck = true;

	}
	else
	{
		bIsStuck = false;
	}


	//if (bCheckAllWheelOnGround_localval)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("ANiceCar::CheckIsInAir // On Ground..."));
	//	bIsStuck = false;
	//}
	//else
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("ANiceCar::CheckIsInAir // In Air..."));
	//	bIsStuck = true;
	//}

	//if (WheelPos.Num() != 4)
	//{
	//	UE_LOG(LogTemp,Warning,TEXT("ANiceCar::CheckIsInAir // something wrong..."));
	//}

	//FHitResult Hit;
	//FCollisionQueryParams Params(FName("Wheel"), false, this);
	//FCollisionResponseParams RParams(ECollisionResponse::ECR_Block);

	//bool bCheckAllWheelIsGround = true;

	//for (int32 WheelCnt = 0; WheelCnt < WheelPos.Num(); ++WheelCnt)
	//{
	//	FVector WheelLo = WheelPos[WheelCnt];
	//	FVector AdjustWheelLo = WheelLo + FVector::DownVector * VD.WheelRadius;		
	//	FVector EndLo = FVector(AdjustWheelLo.X, AdjustWheelLo.Y, AdjustWheelLo.Z * 30.f);

	//	DrawDebugLine(GetWorld(),AdjustWheelLo,EndLo, FColor::Blue, false, 1.f, 0, 2.f);
	//	//하나라도 떠있다면 공중에 있다고 판단한다.
	//	if (GetWorld()->LineTraceSingleByChannel(Hit, AdjustWheelLo, EndLo, ECollisionChannel::ECC_WorldStatic, Params, RParams) == false)
	//	{
	//		bCheckAllWheelIsGround = false;
	//		break;
	//	}
	//}

	/*if (bCheckAllWheelIsGround)
	{
		bIsInAir = true;
	}
	else
	{
		bIsInAir = false;
	}*/

}

void ANiceCar::LockCameraRoll()
{
	FRotator CurrentVehicleRot = GetActorRotation();
	
	FRotator CurrentFPArmRot = FPSpringArm->GetRelativeRotation();
	FRotator CurrentFPCamRot = FPScam->GetRelativeRotation();

	//FPSpringArm->SetRelativeRotation(FRotator(CurrentFPArmRot.Pitch, CurrentFPArmRot.Yaw, CurrentVehicleRot.Roll));
	FPScam->SetRelativeRotation(FRotator(CurrentFPCamRot.Pitch, CurrentFPCamRot.Yaw, CurrentVehicleRot.Roll));
	//SpringArm->SetRelativeRotation(FRotator(CurrentSpringArmRot.Pitch, CurrentSpringArmRot.Yaw, CurrentVehicleRot.Roll));
	
	//FPScam->SetRelativeRotation(FRotator(CurrentFPCamRot.Pitch, CurrentFPCamRot.Yaw, CurrentVehicleRot.Roll));

	//FPScam->SetWorldRotation(FRotator(CurrentFPCamRot.Pitch, CurrentFPCamRot.Yaw, CurrentVehicleRot.Roll));
}

void ANiceCar::CanLockedCam()
{
	//if, FirstLockedCam || ContinuousLockCam Timer is active then, return.
	if (GetWorldTimerManager().IsTimerActive(FirstLockedCamTimer)) return;

	GetWorldTimerManager().SetTimer(FirstLockedCamTimer, [=] {
		//UE_LOG(LogTemp, Warning, TEXT("ANiceCar : Set Still No Input, Set FirstLockedCamTimer "));
		bIsStillNoInput = true;
		bFirstFixed = true;
	}, 5.f, false);
}

void ANiceCar::LockedCam()
{
	if (Controller)
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		if (PC)
		{
			RotTime = 0.f;
			RotAlphaTime = 0.f;
			FRotator CurrentRotation = PC->GetControlRotation();
			GetWorldTimerManager().SetTimer(CamRotTimer, [ = ] {

				if (RotAlphaTime >= 0.999f)
				{
					CanClearRotTimer();
					return;
				}

				RotTime += GetWorld()->GetDeltaSeconds();				
				RotAlphaTime = RotTime / CamFixTime;				

				FRotator ForwRot = GetActorForwardVector().Rotation();

				//첫 고정이면 lerp하게 한다.
				if (bFirstFixed)
				{
					//UE_LOG(LogTemp, Warning, TEXT("ANiceCar : FirstLockedCamTimer is Active, Lerp Cam"));
					FRotator LerpRotation = FMath::Lerp(CurrentRotation, ForwRot, RotAlphaTime);
					PC->SetControlRotation(LerpRotation);					
				}
				//지속 고정이면 바로 고정되게 한다.
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("ANiceCar : FirstLockedCamTimer is Not Active, Fix Fast Cam"));
					PC->SetControlRotation(ForwRot);
				}
				

				//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 1000.f, FColor::Green, false, 1.f, 0, 3.f);
				//UE_LOG(LogTemp, Warning, TEXT("CurrentRot : %s"), *CurrentRotation.ToString());
				//UE_LOG(LogTemp, Warning, TEXT("LerpRotation : %s"), *LerpRotation.ToString());
		}, GetWorld()->GetDeltaSeconds(), true);

			//ClearFirstLockedCamTimer();
			/*PC->RotationInput = FRotator::ZeroRotator;*/

		}
	}
}

void ANiceCar::ManClearAllTimer()
{
	GetWorldTimerManager().ClearTimer(CamRotTimer);
	GetWorldTimerManager().ClearTimer(FirstLockedCamTimer);
	bIsStillNoInput = false;
}

void ANiceCar::ClearFirstLockedCamTimer()
{
	if (GetWorldTimerManager().IsTimerActive(FirstLockedCamTimer))
	{
		GetWorldTimerManager().ClearTimer(FirstLockedCamTimer);
	}

}

void ANiceCar::CanClearRotTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("ANiceCar : Clear FirstLockedCamTimer And CamRotTimer"));
	if (GetWorldTimerManager().IsTimerActive(CamRotTimer))
	{
		GetWorldTimerManager().ClearTimer(CamRotTimer);
	}

	bFirstFixed = false;
	if (GetWorldTimerManager().IsTimerActive(FirstLockedCamTimer))
	{
		GetWorldTimerManager().ClearTimer(FirstLockedCamTimer);
	}
}


void ANiceCar::EngineStart()
{
	bIsEngineStarting = false;
}







/******************************************************************/
/*****************        Input            ************************/
/******************************************************************/

void ANiceCar::CamLookPitch(float Val)
{
	if (Val == 0.f)
	{
		bPitchIsZero = true;
		return;
	}

	bPitchIsZero = false;
	ManClearAllTimer();	
	AddControllerPitchInput(Val * 45.f * GetWorld()->GetDeltaSeconds());
}

void ANiceCar::CamLookYaw(float Val)
{
	if (Val == 0.f)
	{
		bYawIsZero = true;
		return;
	}

	bYawIsZero = false;
	ManClearAllTimer();
	AddControllerYawInput(Val * 45.f * GetWorld()->GetDeltaSeconds());
}


void ANiceCar::MoveForward(float Val)
{
	if(bIsEngineStarting) return;

	/*if (Val == 0.f)
	{
		bIsInputThrottle = false;
	}
	else
	{
		bIsInputThrottle = true;
	}*/

	if (bIsInAir && Val != 0.f)
	{
		FVector CurrentLo = GetMesh()->GetRelativeLocation();
		FRotator CurrentRot = GetMesh()->GetRelativeRotation();

		FVector BeforeVec = CurrentRot.Vector().GetSafeNormal();

		CurrentRot.Pitch += Val*2;
		FVector AfterVec = CurrentRot.Vector().GetSafeNormal();

		FVector ForceDir = (AfterVec - BeforeVec).GetSafeNormal();

		GetMesh()->AddForce(ForceDir);
		GetMesh()->SetWorldLocationAndRotation(CurrentLo, CurrentRot, false, nullptr, ETeleportType::TeleportPhysics);
		
	}

	GetVehicleMovementComponent()->SetThrottleInput(Val);
}
void ANiceCar::MoveRight(float Val)
{
	if (bIsEngineStarting) return;

	if (bIsStuck && Val != 0.f)
	{	
		FVector CurrentLo = GetMesh()->GetRelativeLocation();
		FRotator CurrentRot = GetMesh()->GetRelativeRotation();

		FVector UpVector = FVector(0.f,0.f,1.f);
		FVector ReverseGravityForce = UpVector * GetVehicleMovement()->Mass* GetWorld()->GetGravityZ() *-1.3f;
		GetMesh()->AddForce(ReverseGravityForce / 1.5f);

		CurrentRot.Roll += Val * 3.f;

		GetMesh()->SetRelativeLocationAndRotation(CurrentLo, CurrentRot, false, nullptr, ETeleportType::TeleportPhysics);

		/*FVector DebugLo = GetActorLocation();
		DrawDebugLine(GetWorld(), DebugLo + UpVector, DebugLo + ReverseGravityForce / 2, FColor::Green, false, 1.f, 0, 4.f);*/	
	}
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ANiceCar::PressHandbrake()
{
	if (bIsEngineStarting) return;
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}
void ANiceCar::ReleaseHandbrake()
{
	if (bIsEngineStarting) return;
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ANiceCar::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void ANiceCar::ChangeCamera()
{
	bool bNewCamState = !bUsingFPSCam;
	if (bNewCamState != bUsingFPSCam)
	{
		bUsingFPSCam = bNewCamState;

		//FPS cam
		if (bNewCamState)
		{
			TPSCam->Deactivate();
			FPScam->Activate();
		}
		//TPScam
		else
		{
			FPScam->Deactivate();
			TPSCam->Activate();
		}
	}
}

//GetIn the Car
void ANiceCar::Interaction(AActor* Actor)
{
	InOutCar();
}

void ANiceCar::InOutCar()
{
	if(GetOwner())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ANiceCar::InOutCar"));
		//UE_LOG(LogTemp, Warning, TEXT("Did This call Twice? Owning Actor: %s | Component Name: %s"), *GetOwner()->GetName(), *GetName());
	}

	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	if (PlayerCon)
	{
		if (bIsOverlapDoor)
		{ 
			if (PlayerCon->ToggleCar(this))
			{
				UnsetOutline();
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), VD.EngineStartSound, GetActorLocation());//EngineSoundComp->GetComponentLocation());
				EngineIgnitionTime = VD.EngineStartSound->GetDuration();
				bIsEngineStarting = true;
				GetWorldTimerManager().SetTimer(EngineIgnitionTimer, this, &ANiceCar::EngineStart, EngineIgnitionTime, false);

				EngineSoundComp->Play();

				GetMesh()->SetSimulatePhysics(true);
				return;
			}			
		}
		else if (bIsOverlapTrunk)
		{
			OpenTrunk();
		}
	}
	return;
}

void ANiceCar::SetCharacter(ABaseCharacter* TakeSeatCharacter)
{
	if(!TakeSeatCharacter) return;

	CurrentHaveSeatChar = TakeSeatCharacter;
	CurrentHaveSeatChar->PlayAnimation(VD.CharacterSeatAnim, nullptr);

	CurrentHaveSeatChar->SetActorRelativeTransform(VD.SeatSocketTF);
}

void ANiceCar::UnSetCharacter()
{
	if (CurrentHaveSeatChar.IsValid())
	{
		CurrentHaveSeatChar->StopAnimation();
		CurrentHaveSeatChar = nullptr;
	}
}

void ANiceCar::OpenTrunk()
{
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	UNewInventory* MainInventory = Cast<UNewInventory>(PlayerCon->MainHud->NewInventoryWidget);
	if (MainInventory)
	{
		PlayerCon->bIsInteractLootBox = true;
		LootWidgetComp->CreateInteractionWidget(PlayerCon, this); //새로추가
	}
}

void ANiceCar::GetoutCar()
{
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	if (PlayerCon)
	{
		float CapsuleRad;
		float CapsuleHH;
		PlayerCon->PlayerChar->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRad, CapsuleHH);

		FVector CarRightVec = GetActorRightVector();
		FVector DoorBoxLo = DoorCollision->GetComponentLocation();
		float Tracelength = DoorCollision->GetScaledBoxExtent().Y;	

		FVector YOpSide = DoorBoxLo + CarRightVec * Tracelength;
		FVector YNeSide = DoorBoxLo + CarRightVec * Tracelength * -1.f;

		

		FVector CharacterHalfBounding = FVector(CapsuleRad, CapsuleRad, CapsuleHH);

		FVector GetoutLocation = FVector(0.f);

		bool bCanGetOut = true;

		//운전석 먼저 검증
		if (CanGetout(DoorBoxLo, YNeSide, CharacterHalfBounding))
		{
			GetoutLocation = YNeSide;
		}
		else if (CanGetout(DoorBoxLo, YOpSide, CharacterHalfBounding))
		{
			GetoutLocation = YOpSide;
		}
		else
		{
			bCanGetOut = false;
		}

		
		//나갈 수 있을때만 실행한다.
		if (bCanGetOut)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), VD.EngineOffSound, GetActorLocation());//EngineSoundComp->GetComponentLocation());
			EngineSoundComp->Stop();
			SkidSoundComp->Stop();
			ManClearAllTimer();
			UnSetCharacter();
			PlayerCon->ToggleCar(this,GetoutLocation);
		}		
	}
}

bool ANiceCar::CanGetout(const FVector DoorOrigin, const FVector WantToGetOutPos, FVector HalfBoxSize)
{
	bool bReturn = true;

	FHitResult Hit;

	FCollisionQueryParams Params(FName("door"), false, this);
	FCollisionResponseParams RParams(ECollisionResponse::ECR_Block);

	if (CurrentHaveSeatChar.IsValid())
	{
		Params.AddIgnoredActor(CurrentHaveSeatChar.Get());
	}
	
	DrawDebugLine(GetWorld(), DoorOrigin, WantToGetOutPos,FColor::Green,false, 5.f,0,3.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, DoorOrigin, WantToGetOutPos, ECollisionChannel::ECC_WorldStatic, Params, RParams))
	{
		bReturn = false;
	}
	
	//OldVersion... 간단한 방법으로 바꿈.
	/*
	FHitResult VerifyHit;
	
	FCollisionShape BoxShape;
	BoxShape.MakeBox(HalfBoxSize);
	FCollisionQueryParams QParams(TEXT("Vehicle Door"), false, this);

	FCollisionResponseContainer CollisionContainer;
	CollisionContainer.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	FCollisionResponseParams RParams(CollisionContainer);

	// little bit different location
	FVector AdjPos = FVector(WantToGetOutPos.X, WantToGetOutPos.Y, WantToGetOutPos.Z + 10.f);
	FVector AdjPosLittle = FVector(WantToGetOutPos.X, WantToGetOutPos.Y, WantToGetOutPos.Z + 11.f);

	DrawDebugSphere(GetWorld(),AdjPos,6.f,6,FColor::Green,false,6.f,0,4.f);
	DrawDebugBox(GetWorld(), AdjPos, HalfBoxSize,FQuat::Identity,FColor::Green,false, 6.f, 0, 4.f);


	if (GetWorld()->SweepSingleByChannel(VerifyHit, AdjPos, AdjPosLittle, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, BoxShape, QParams, RParams))
	{
		FString HitActorName;
		VerifyHit.GetActor()->GetName(HitActorName);
		bool bIsLandScape = HitActorName.Contains(TEXT("landscape"));
		bool bIsPlane = HitActorName.Contains(TEXT("plane"));

		if (!bIsLandScape || !bIsPlane)
		{
			bReturn = false;
		}
	}
	*/

	return bReturn;
}

void ANiceCar::TrySave()
{
	//움직임이 없을때만 저장을 시도함.
	if ( GetVelocity().Size() > 0.f )
	{
		UE_LOG(LogTemp,Warning,TEXT("ANiceCar::TrySave// 차량이 완전히 멈춰야 저장할 수 있습니다."));
	}
	else
	{
		if(CheckThreeSurfaceHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("ANiceCar::TrySave// Controller Name : %s"), *GetController()->GetFName().ToString());
			//Playercontroller를 불러와서 저장을 시작함.
			if (AMainController* PlayerCon = Cast<AMainController>(GetController()))
			{
				PlayerCon->SaveGame();
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ANiceCar::TrySave// 차량의 3면이상이 벽에 닿는곳에서만 주차할 수 있습니다."));
			UE_LOG(LogTemp, Warning, TEXT("ANiceCar::TrySave// 또는 너무 벽에 붙었습니다."));
		}

	}

}


bool ANiceCar::CheckThreeSurfaceHit()
{
	
	FVector CarLocation = GetActorLocation();
	FVector BoxExt = GetMesh()->GetPhysicsAsset()->CalcAABB(GetMesh(), FTransform::Identity).GetExtent();

	//CarLocation의 Z축을 살짝 올린다.
	FVector AdjustCarLocation(CarLocation.X, CarLocation.Y, CarLocation.Z + BoxExt.Z);
	
	FRotator CarRotation = GetActorRotation();
	FVector FowVector = GetActorForwardVector(); //CarRotation.RotateVector(FVector::ForwardVector);
	FVector RigVector = GetActorRightVector(); //CarRotation.RotateVector(FVector::RightVector);

	TArray<FVector> OriginLineTrace;
	OriginLineTrace.Add(AdjustCarLocation + FowVector * (BoxExt.X - 10.f));
	OriginLineTrace.Add(AdjustCarLocation + FowVector * (BoxExt.X - 10.f) * -1.f);
	OriginLineTrace.Add(AdjustCarLocation + RigVector * (BoxExt.Y - 10.f));
	OriginLineTrace.Add(AdjustCarLocation + RigVector * (BoxExt.Y - 10.f) * -1.f);

	int32 HitCount = 0;
	for (FVector Origin : OriginLineTrace)
	{
		FHitResult Hit;
		//FVector RotVec = (Origin - AdjustCarLocation).Rotation().Vector();
		FVector RotVec = (Origin - AdjustCarLocation).GetSafeNormal();
		FCollisionQueryParams Params(FName("CarTrace"), false, this);
		GetWorld()->LineTraceSingleByChannel(Hit, Origin, Origin+RotVec*200.f, ECollisionChannel::ECC_Visibility, Params);

		if ( Hit.IsValidBlockingHit() )
		{
			++HitCount;
		}

		if ( HitCount >= 3 )
		{
			break;
		}
		DrawDebugLine(GetWorld(),Origin, Origin + RotVec * 200.f, FColor::Green,false, 5.f, 0, 3.f);
		
	}

	if ( HitCount >= 3 )
	{
		return true;
	}
	return false;
}


void ANiceCar::DoorOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			bIsOverlapDoor = true;
		}
	}

}
void ANiceCar::DoorOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			bIsOverlapDoor = false;
		}
	}
}

void ANiceCar::TrunkOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			bIsOverlapTrunk = true;
		}
	}
}
void ANiceCar::TrunkOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{
			bIsOverlapTrunk = false;
		}
	}
}


void ANiceCar::TeleportVehicle(FTransform& DesignatedLocation)
{
	FVector Location = DesignatedLocation.GetTranslation();
	FQuat Rotation = DesignatedLocation.GetRotation();

	GetMovementComponent()->StopMovementImmediately();

	GetMesh()->SetAllPhysicsPosition(Location);
	GetMesh()->SetAllPhysicsRotation(Rotation);
}

void ANiceCar::SetOutline()
{
	GetMesh()->SetRenderCustomDepth(true);

}
void ANiceCar::UnsetOutline()
{
	GetMesh()->SetRenderCustomDepth(false);
}


void ANiceCar::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);
	if (!CurrentHaveSeatChar.IsValid()) return;

	/** 부딪힌 세기가 특정 값 이하면 effect를 발생시키지 않는다. */
	UE_LOG(LogTemp, Warning, TEXT("ANiceCar::NotifyHit was called"));
	float Force = NormalForce.SizeSquared(); //Size();
	if (Force <= FMath::Square(55000.f)) return;

	/** 뒤집혀서 멈춘 상태라면 effect를 발생시키지 않는다. */
	if(bIsStuck && GetVehicleMovement()->GetForwardSpeed() < 0.5f) return;

	UE_LOG(LogTemp, Warning, TEXT("ANiceCar::NotifyHit/ Force : %f"), Force);
	

	if (UMyGameInstance* GameInst = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		if(!GameInst->CommonImpactEffectTable) return;

		FTransform SpawnEffectTransform = FTransform(HitNormal.Rotation(), HitLocation);

		FName FindRowName = VD.ImpactEffectRowHandle.RowName;
		UCustomSystemLibrary::SpawnImpactEffect_Delayed(GetWorld(), Hit, NormalForce, GameInst->CommonImpactEffectTable, FindRowName, SpawnEffectTransform);

		/* came shake */

	}

		
	/** impact FX config */
	/*UPROPERTY(Category=Effects, EditDefaultsOnly)
	TSubclassOf<AVehicleImpactEffect> ImpactTemplate;*/

	/** camera shake on impact */
	/*UPROPERTY(Category=Effects, EditDefaultsOnly)
	TSubclassOf<UMatineeCameraShake> ImpactCameraShake;*/

	/*if (ImpactTemplate && NormalForce.SizeSquared() > FMath::Square(ImpactEffectNormalForceThreshold))
	{
		FTransform const SpawnTransform(HitNormal.Rotation(), HitLocation);
		AVehicleImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AVehicleImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			float DotBetweenHitAndUpRotation = FVector::DotProduct(HitNormal, GetMesh()->GetUpVector());
			EffectActor->HitSurface = Hit;
			EffectActor->HitForce = NormalForce;
			EffectActor->bWheelLand = DotBetweenHitAndUpRotation > 0.8f;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	if (ImpactCameraShake)
	{
		AVehiclePlayerController* PC = Cast<AVehiclePlayerController>(Controller);
		if (PC != nullptr && PC->IsLocalController())
		{
			PC->ClientStartCameraShake(ImpactCameraShake, 1);
		}
	}*/

}

void ANiceCar::SetMotherSpawnVolume(ASpawnVolume* Var_MotherVolume)
{
	
}

FVector ANiceCar::GetWheelLocation(const FWheelSetup& WheelSetup)
{
	//UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	FVector Offset = WheelSetup.WheelClass.GetDefaultObject()->Offset + WheelSetup.AdditionalOffset;

	if (WheelSetup.BoneName != NAME_None)
	{
		USkinnedMeshComponent* MeshCmp = GetMesh();
		if (MeshCmp && MeshCmp->SkeletalMesh)
		{
			const FVector BonePosition = MeshCmp->SkeletalMesh->GetComposedRefPoseMatrix(WheelSetup.BoneName).GetOrigin() * MeshCmp->GetRelativeScale3D();
			//BonePosition is local for the root BONE of the skeletal mesh - however, we are using the Root BODY which may have its own transform, so we need to return the position local to the root BODY
			if (MeshCmp->GetBodyInstance() && MeshCmp->GetBodyInstance()->BodySetup.IsValid())
			{
				const FMatrix RootBodyMTX = MeshCmp->SkeletalMesh->GetComposedRefPoseMatrix(MeshCmp->GetBodyInstance()->BodySetup->BoneName);
				const FVector LocalBonePosition = RootBodyMTX.InverseTransformPosition(BonePosition);
				Offset += LocalBonePosition;
			}
		}
	}
	return Offset;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS

