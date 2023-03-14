// Fill out your copyright notice in the Description page of Project Settings.


#include "NiceCar.h"
#include "FrontWheel.h"
#include "RearWheel.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/GameData/VehicleDataTable.h"

#include "OpenWorldRPG/NewInventory/LootWidgetComponent.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

#include "WheeledVehicleMovementComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

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
	/*SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VehicleSKMesh"));
	SetRootComponent(SKMesh);*/

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArm->SetRelativeLocation(FVector(0.f,0.f,200.f));
	SpringArm->SetupAttachment(GetRootComponent());

	SpringArm->TargetArmLength = 400.f;
	//camera smooth panning 
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 9.0f;
	SpringArm->CameraLagMaxDistance = 115.f;

	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bUsePawnControlRotation = true;	

	TPSCam = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamComp"));
	TPSCam->SetupAttachment(SpringArm);
	TPSCam->SetRelativeLocation(FVector(0.f,0.f,50.f));
	TPSCam->SetRelativeRotation(FRotator(-5.f,0.f,0.f));
	TPSCam->bUsePawnControlRotation = false;
	TPSCam->SetFieldOfView(90.f);

	FPScam = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamComp"));
	FPScam->SetupAttachment(GetRootComponent());
	FPScam->SetRelativeLocation(FVector(47.f,-40.f,200.f));
	FPScam->bUsePawnControlRotation = true;	
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

	LootWidgetComp = CreateDefaultSubobject<ULootWidgetComponent>(TEXT("LootWidgetComp"));
	LootWidgetComp->WidgetType = EWidgetType::EWT_LootBox;
	
}

void ANiceCar::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float EngineRotSpd = GetVehicleMovement()->GetEngineRotationSpeed();
	EngineSoundComp->SetFloatParameter(FName("RPM"),EngineRotSpd);
	

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
	InputComp->BindAction("Interactive", IE_Pressed, this, &ANiceCar::GetoutCar);

	InputComp->BindAction("Jump", IE_Pressed, this, &ANiceCar::PressHandbrake);
	InputComp->BindAction("Jump", IE_Released, this, &ANiceCar::ReleaseHandbrake);

}

void ANiceCar::OnConstruction(const FTransform& Transform)
{
	if (VehicleDataTable)
	{
		VehicleRowHandle.DataTable = VehicleDataTable;
		FName WantRow = VehicleRowHandle.RowName;

		FString TableName = VehicleDataTable->GetName();

		if(FVehicleDataTable* GetData = (VehicleDataTable->FindRow<FVehicleDataTable>(WantRow,TableName)))
		{
			VD = *GetData;
			VehicleSetting(VD);
		}
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
	FPScam->SetRelativeLocation(Data.FPSCamLocation);

	DoorCollision->SetRelativeTransform(Data.DoorCollisionTF);
	TrunkCollision->SetRelativeTransform(Data.TrunkCollisionTF);
	auto VMC = GetVehicleMovement();
	VMC->WheelSetups[0].WheelClass = Data.FrontWheel;
	VMC->WheelSetups[0].BoneName = Data.FRWheelBoneName;
	VMC->WheelSetups[1].WheelClass = Data.FrontWheel;
	VMC->WheelSetups[1].BoneName = Data.FLWheelBoneName;

	VMC->WheelSetups[2].WheelClass = Data.RearWheel;
	VMC->WheelSetups[2].BoneName = Data.RRWheelBoneName;
	VMC->WheelSetups[3].WheelClass = Data.RearWheel;
	VMC->WheelSetups[3].BoneName = Data.RLWheelBoneName;

	VMC->Mass = Data.Mass;
	VMC->DragCoefficient = Data.Dragcoefficient;
	VMC->MaxEngineRPM = Data.MaxRPM;


	/*Sound Settings */
	EngineSoundComp->SetRelativeLocation(Data.EngineSoundLocation);
	TireRollingSoundComp->SetRelativeLocation(Data.TireSoundLocation);
	SkidSoundComp->SetRelativeLocation(Data.SkidSoundLocation);	

	EngineSoundComp->SetSound(Data.EngineRPMSound);
	EngineSoundComp->Stop();
	
	SkidSoundComp->SetSound(Data.SkidSound);
	SkidSoundComp->Stop();
	
	GetMesh()->SetCenterOfMass(Data.CenterOfMass);

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

/******************************************************************/
/*****************        Input            ************************/
/******************************************************************/
void ANiceCar::CamLookPitch(float Val)
{
	AddControllerPitchInput(Val * 45.f * GetWorld()->GetDeltaSeconds());
}

void ANiceCar::CamLookYaw(float Val)
{
	AddControllerYawInput(Val * 45.f * GetWorld()->GetDeltaSeconds());
}


void ANiceCar::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}
void ANiceCar::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ANiceCar::PressHandbrake()
{	
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}
void ANiceCar::ReleaseHandbrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
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
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	if (PlayerCon)
	{
		if (bIsOverlapDoor)
		{			
			PlayerCon->ToggleCar(this);
			UnsetOutline();

			UGameplayStatics::PlaySoundAtLocation(GetWorld(), VD.EngineStartSound, GetActorLocation());//EngineSoundComp->GetComponentLocation());
			EngineSoundComp->Play();
		}
		else if (bIsOverlapTrunk)
		{
			OpenTrunk();
		}
	}
}

void ANiceCar::OpenTrunk()
{
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	UNewInventory* MainInventory = Cast<UNewInventory>(PlayerCon->MainHud->NewInventoryWidget);
	if (MainInventory)
	{
		PlayerCon->bIsInteractLootBox = this;
		LootWidgetComp->CreateInteractionWidget(PlayerCon, this); //새로추가
	}

}

void ANiceCar::GetoutCar()
{
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	if (PlayerCon)
	{

		FVector CarRightVec = GetActorRightVector();
		FVector DoorBoxLo = DoorCollision->GetComponentLocation();
		float Ylength = DoorCollision->GetScaledBoxExtent().Y;	

		FVector YOpSide = DoorBoxLo + CarRightVec * Ylength;
		FVector YNeSide = DoorBoxLo + CarRightVec * Ylength * -1.f;

		float CapsuleRad;
		float CapsuleHH;
		PlayerCon->Main->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRad, CapsuleHH);

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


void ANiceCar::SetOutline()
{
	GetMesh()->SetRenderCustomDepth(true);

}
void ANiceCar::UnsetOutline()
{
	GetMesh()->SetRenderCustomDepth(false);
}


PRAGMA_ENABLE_DEPRECATION_WARNINGS

