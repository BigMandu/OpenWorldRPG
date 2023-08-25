// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Gizmo/ShootingTarget.h"
#include "OpenWorldRPG/Gizmo/ShootingTargetSMComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AShootingTarget::AShootingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Optionalmesh"));
	RotationMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotationmesh"));


	Body_10P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Body_10P"));
	Body_8P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Body_8P"));
	Body_6P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Body_6P"));
	Body_5P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Body_5P"));
	Head_10P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Head_10P"));
	Head_8P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Head_8P"));
	Head_5P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Head_5P"));
	LeftArm_4P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("LeftArm_4P"));
	RightArm_4P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("RightArm_4P"));
	Leg_4P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Leg_4P"));
	Leg_0P = CreateDefaultSubobject<UShootingTargetSMComponent>(TEXT("Leg_0P"));
	

	SetRootComponent(RootMeshComp);
	RotationMeshComp->SetupAttachment(GetRootComponent());

	//RotationOptionalMesh는 회전하는 mesh에 붙어있는 mesh임.
	Body_10P->SetupAttachment(RotationMeshComp);
	Body_8P->SetupAttachment(RotationMeshComp);
	Body_6P->SetupAttachment(RotationMeshComp);
	Body_5P->SetupAttachment(RotationMeshComp);
	Head_10P->SetupAttachment(RotationMeshComp);
	Head_8P->SetupAttachment(RotationMeshComp);
	Head_5P->SetupAttachment(RotationMeshComp);
	LeftArm_4P->SetupAttachment(RotationMeshComp);
	RightArm_4P->SetupAttachment(RotationMeshComp);
	Leg_4P->SetupAttachment(RotationMeshComp);
	Leg_0P->SetupAttachment(RotationMeshComp);
	

	//TestBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	//TestBoxComp->SetupAttachment(GetRootComponent());
}

void AShootingTarget::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitialRotation = RotationMeshComp->GetRelativeRotation();
	TargetRotation = FRotator(0.f);
	switch (RotateDirection)
	{
	case ERotateDirection::ERD_Pitch:
		TargetRotation = FRotator(InitialRotation.Pitch + RotateValue, InitialRotation.Yaw, InitialRotation.Roll);
		break;
	case ERotateDirection::ERD_Yaw:
		TargetRotation = FRotator(InitialRotation.Pitch, InitialRotation.Yaw + RotateValue, InitialRotation.Roll);
		break;
	case ERotateDirection::ERD_Roll:
		TargetRotation = FRotator(InitialRotation.Pitch, InitialRotation.Yaw, InitialRotation.Roll + RotateValue);
		break;

	}

	//TestBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AShootingTarget::StartPopUp);

}

// Called when the game starts or when spawned
void AShootingTarget::BeginPlay()
{
	Super::BeginPlay();

	MissionUniqueID_Interface = RelatedMissionUniqID;
}

void AShootingTarget::InitializeTarget()
{
	HitCount = 0;
	TotalScore = 0;
	bHitHead = false;
	bHitHead = false;
}

void AShootingTarget::LerpRotation(FRotator TRot)
{
	GetWorldTimerManager().SetTimer(PopupTimer, [ = ]{
		float DeltaSec = GetWorld()->GetDeltaSeconds();

		FRotator CurrentRot = RotationMeshComp->GetRelativeRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TRot, DeltaSec, PopupSpeed);
		RotationMeshComp->SetRelativeRotation(NewRot);

		if (CanClearTimer(CurrentRot, TRot))
		{
			ClearRotTimer();
		}

	}, GetWorld()->GetDeltaSeconds(), true);
}


//다른 class에서 호출할 예정
void AShootingTarget::StartPopUp() //UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	InitializeTarget();

	FTimerManager& WorldTimer = GetWorldTimerManager();
	{
		if (WorldTimer.IsTimerActive(PopupTimer))
		{
			WorldTimer.ClearTimer(PopupTimer);
		}

		LerpRotation(TargetRotation);
		bIsPopUpStatus = true;
	}
}

void AShootingTarget::ResetTarget()
{
	FTimerManager& WorldTimer = GetWorldTimerManager();
	{
		if (WorldTimer.IsTimerActive(PopupTimer))
		{
			WorldTimer.ClearTimer(PopupTimer);
		}

		LerpRotation(InitialRotation);
		bIsPopUpStatus = false;
	}
}

bool AShootingTarget::CanClearTimer(FRotator CurRot, FRotator TargetRot)
{
	return CurRot.Equals(TargetRot, 0.001f);

}

void AShootingTarget::ClearRotTimer()
{
	GetWorldTimerManager().ClearTimer(PopupTimer);
}


float AShootingTarget::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//popup상태가 아니라면 damage를 받지 않는다.
	if(bIsPopUpStatus == false) return DamageAmount;


	if (AWeapon* Weapon = Cast<AWeapon>(DamageCauser))
	{
		FHitResult HitInfo;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, EventInstigator, HitInfo, ImpulseDir);


		if (HitInfo.IsValidBlockingHit())
		{
			if (UShootingTargetSMComponent* ShootingSplitMesh = Cast<UShootingTargetSMComponent>(HitInfo.GetComponent()))
			{
				if (ShootingSplitMesh->TargetMeshType == ETargetMeshType::ETMT_Head)
				{
					bHitHead = true;
				}
				else if (ShootingSplitMesh->TargetMeshType == ETargetMeshType::ETMT_Body)
				{
					bHitBody = true;
				}

				TotalScore += ShootingSplitMesh->Score;
				++HitCount;

				if (CheckTripleTap())
				{
					int32 PerfectCount = 0;
					UE_LOG(LogTemp, Warning, TEXT("Total Score : %d"), TotalScore);
					if (bHitHead && bHitBody)
					{
						TotalScore += 5;
						PerfectCount = 1;
						UE_LOG(LogTemp, Warning, TEXT("Success Mozambique Drill Total Score : %d"), TotalScore);
					}
					ResetTarget();

					if (AOpenWorldRPGGameModeBase* Gmode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
					{
						if (Gmode->EvaluateMissionStatus(this))
						{
							Gmode->OnlyCQBMission(PerfectCount, TotalScore);
						}
					}

					return DamageAmount;
				}

			}
			UE_LOG(LogTemp,Warning,TEXT("ShootingTargetName : %s"), *HitInfo.GetComponent()->GetFName().ToString());
		}

	}
	return DamageAmount;
}

bool AShootingTarget::CheckTripleTap()
{
	if (HitCount == 3 || HitCount >= 3)
	{
		return true;
	}
	return false;
}

// Called every frame
//void AShootingTarget::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AShootingTarget::ShowNotifyIcon()
{
	
}
void AShootingTarget::HideNotifyIcon()
{
	
}