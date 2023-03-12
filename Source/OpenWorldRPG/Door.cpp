// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ADoor::ADoor()
{
	bIsOpen = false;

	//for collision door
	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	//collisionBox->SetupAttachment(GetRootComponent());
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnDoorBeginOverlap);
	collisionBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnDoorEndOverlap);	
	
	//for physics door
	Topconstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("UpPhysicsConstraint"));
	Topconstraint->SetupAttachment(GetRootComponent());

	Bottomconstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("DownPhysicsConstraint"));
	Bottomconstraint->SetupAttachment(GetRootComponent());
}


void ADoor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (DoorType == EDoorType::EDT_PhysicsDoor)
	{
		SettingPhysicsDoor();
	}
	else
	{
		MeshInitRotation = Mesh->GetRelativeRotation();
		MeshInitLocation = Mesh->GetRelativeLocation();
		CBInitRotation = collisionBox->GetComponentRotation();
		CBInitLocation = collisionBox->GetComponentLocation();

		if (TargetRotation == FRotator(0.f) || TargetLocation == FVector(0.f))
		{
			UE_LOG(LogTemp, Warning, TEXT("TargetRotation Or Location is 0"));
		}
		
		if (RotationLerpSpeed == 0.f || LocationLerpSpeed == 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("LerpSpeed is 0"));
		}

	}	
	
}


void ADoor::Interaction(AActor* Actor)
{
	if (DoorType != EDoorType::EDT_InteractDoor) return;
	ToggleDoor(Actor);
}

void ADoor::ToggleDoor(AActor* Actor)
{
	if (bIsOpen)
	{		
		CloseDoor();
	}
	else
	{
		bool bIsbehind = IsPlayerBehind(Actor);
		OpenDoor(bIsbehind);
	}
}

void ADoor::OnDoorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player == nullptr || DoorType != EDoorType::EDT_CollisionDoor) return;

	if (GetWorldTimerManager().IsTimerActive(CallCloseTimerHandle))
	{
		ClearTimer(CallCloseTimerHandle);
	}
	UE_LOG(LogTemp, Warning, TEXT("ADoor::BeginOverlap"));
	bool bIsbehind = IsPlayerBehind(OtherActor);
	OpenDoor(bIsbehind);
}

void ADoor::OnDoorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player == nullptr || DoorType != EDoorType::EDT_CollisionDoor) return;	

	if (GetWorldTimerManager().IsTimerActive(CallCloseTimerHandle))
	{
		ClearTimer(CallCloseTimerHandle);
	}
	UE_LOG(LogTemp, Warning, TEXT("ADoor::End Overlap"));

	GetWorldTimerManager().SetTimer(CallCloseTimerHandle, this, &ADoor::CloseDoor, AutoCloseTime, false);
}

void ADoor::OpenDoor(bool bNeedToReverse)
{	
	if (bIsOpen) return;

	UE_LOG(LogTemp,Warning,TEXT("ADoor::OpenDoor"));
		
	bIsOpen = true;

	if (GetWorldTimerManager().IsTimerActive(CloseTimer))
	{
		ClearTimer(CloseTimer);
	}	

	FRotator AdjRotation = FRotator(MeshInitRotation.Pitch + TargetRotation.Pitch, MeshInitRotation.Yaw + TargetRotation.Yaw, MeshInitRotation.Roll + TargetRotation.Roll);
	if (bNeedToReverse)
	{
		AdjRotation = FRotator(AdjRotation.Pitch, AdjRotation.Yaw - 180.f, AdjRotation.Roll);
	}

	GetWorldTimerManager().SetTimer(OpenTimer,[=]{
		float DeltaSec = GetWorld()->GetDeltaSeconds();
		bool bRotationReached = false;
		bool bLocationReached = false;
		bool bHasRot = false;
		bool bHasLo = false;

		//CollisionBox는 움직이면 안된다.
		collisionBox->SetWorldLocationAndRotation(CBInitLocation, CBInitRotation);

		if(TargetRotation != FRotator(0.f))
		{
			bHasRot = true;
			FRotator NewRot = FMath::Lerp(Mesh->GetRelativeRotation(), AdjRotation, RotationLerpSpeed * DeltaSec);
			Mesh->SetRelativeRotation(NewRot);
			
			//UE_LOG(LogTemp, Warning, TEXT("ADoor::OpenDoor / Cur Ro : %s, Target Ro : %s"), *GetActorRotation().ToString(), *TargetRotation.ToString());
			if (NewRot.Equals(AdjRotation,0.1f))
			{
				bRotationReached = true;
			}
		}
		if(TargetLocation != FVector(0.f))
		{
			bHasLo = true;
			FVector NewLo = FMath::Lerp(Mesh->GetRelativeLocation(), TargetLocation, LocationLerpSpeed * DeltaSec);
			Mesh->SetRelativeLocation(NewLo);

			if (NewLo.Equals(TargetLocation,0.1f))
			{
				bLocationReached = true;

			}

		}

		//ClearTime할수 있는지 검증
		bool bCanClearTime = CanClearTimer(bHasRot,bHasLo,bLocationReached,bRotationReached);

		if (bCanClearTime)
		{
			ClearTimer(OpenTimer);
		}
		

	

	}, GetWorld()->GetDeltaSeconds(), true);

}

void ADoor::CloseDoor()
{
	if (bIsOpen == false) return;

	if (GetWorldTimerManager().IsTimerActive(OpenTimer))
	{
		ClearTimer(OpenTimer);
	}

	bIsOpen = false;

	
	UE_LOG(LogTemp, Warning, TEXT("ADoor::CloseDoor"));

	GetWorldTimerManager().SetTimer(CloseTimer, [&] {
		float DeltaSec = GetWorld()->GetDeltaSeconds();
		bool bRotationReached = false;
		bool bLocationReached = false;
		bool bHasRot = false;
		bool bHasLo = false;

		//CollisionBox는 움직이면 안된다.
		collisionBox->SetWorldLocationAndRotation(CBInitLocation,CBInitRotation);

		if (TargetRotation != FRotator(0.f))
		{
			bHasRot = true;

			FRotator NewRot = FMath::Lerp(Mesh->GetRelativeRotation(), MeshInitRotation, RotationLerpSpeed * DeltaSec);
			Mesh->SetRelativeRotation(NewRot);
			if (NewRot.Equals(MeshInitRotation,0.1f))
			{
				bRotationReached = true;
			}
		}
		if (TargetLocation != FVector(0.f))
		{
			bHasLo = true;
			FVector NewLo = FMath::Lerp(Mesh->GetRelativeLocation(), MeshInitLocation, LocationLerpSpeed * DeltaSec);
			Mesh->SetRelativeLocation(NewLo);
			if (NewLo.Equals(MeshInitLocation,0.1f))
			{
				bLocationReached = true;

			}

		}

		bool bCanClearTime = CanClearTimer(bHasRot, bHasLo, bLocationReached, bRotationReached);

		if (bCanClearTime)
		{
			ClearTimer(CloseTimer);
		}

		}, GetWorld()->GetDeltaSeconds(), true);
}



void ADoor::ClearTimer(FTimerHandle& ClearTimerHandle)
{
	GetWorldTimerManager().ClearTimer(ClearTimerHandle);
}

bool ADoor::CanClearTimer(bool& bHasRot, bool& bHasLoc, bool& bReachedLoc, bool& bReachedRot)
{
	bool bCanClearTimer = false;
	
	if (bHasRot && !bHasLoc)
	{
		if (bReachedRot)
		{
			bCanClearTimer = true;
		}
	}
	else if (!bHasRot && bHasLoc)
	{
		if (bReachedLoc)
		{
			bCanClearTimer = true;
		}
	}
	else if (bHasRot && bHasLoc)
	{
		if (bReachedRot && bReachedLoc)
		{
			bCanClearTimer = true;
		}
	}
	else if (!bHasLoc && !bHasRot)
	{
		bCanClearTimer = true;
	}

	return bCanClearTimer;
}


void ADoor::SettingPhysicsDoor()
{
	UStaticMesh* SettedMesh = Mesh->GetStaticMesh();
	if(!SettedMesh) return;

	Mesh->SetConstraintMode(EDOFMode::Default);
	FConstrainComponentPropName PropName;
	PropName.ComponentName = FName("Mesh");


	Topconstraint->SetConstrainedComponents(Mesh, NAME_None, nullptr, NAME_None);
	//Topconstraint->ComponentName1 = PropName;
	Topconstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 80.f);
	Topconstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 0.f);
	Topconstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 0.f);


	Bottomconstraint->SetConstrainedComponents(Mesh, NAME_None, nullptr, NAME_None);
	//Bottomconstraint->ComponentName1 = PropName;
	Bottomconstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 80.f);
	Bottomconstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 0.f);
	Bottomconstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 0.f);		
	
	Mesh->SetSimulatePhysics(true);
	FVector MeshSize = SettedMesh->GetBoundingBox().GetSize();
	FVector MeshLocation = this->GetActorLocation();

	FVector TopCorner = MeshLocation + FVector(0.f, 0.f, MeshSize.Z);
	FVector MeshRelativeLo = Mesh->GetRelativeLocation();


	/**RefPos 파라미터는 Physics Constraint component가 부착된 Mesh의 현재 위치를 기준으로 하여, Constraint의 상대적 위치를 지정한다.
	* Frame1 : SetConstrainedComponents에서 지정한 Component의 RootComponent 좌표를 기준으로 한다 (Relative Location)
	* Frame2 : SetConstrainedComponents에서 지정한 Component의 World좌표를 기준으로 한다. (World Location)
	*
	* Frame1, 2를 이용해 Constraint의 상대적, 절대적 위치를 구해 지정한다.
	*/
	Topconstraint->SetConstraintReferencePosition(EConstraintFrame::Frame1, FVector(0.f, 0.f, MeshSize.Z));
	Topconstraint->SetConstraintReferencePosition(EConstraintFrame::Frame2, TopCorner);
	Bottomconstraint->SetConstraintReferencePosition(EConstraintFrame::Frame1, FVector(0.f, 0.f, 0.f));
	Bottomconstraint->SetConstraintReferencePosition(EConstraintFrame::Frame2, MeshLocation);


	//Angular motor를 적용한다. 아래는 순서를 지켜야한다.
	Topconstraint->SetAngularDriveMode(EAngularDriveMode::SLERP);
	Topconstraint->SetOrientationDriveSLERP(true);
	Topconstraint->SetAngularVelocityDriveSLERP(true);
	Topconstraint->SetAngularOrientationTarget(FRotator(0.f));
	Topconstraint->SetAngularVelocityTarget(FVector(0.f));
	Topconstraint->SetAngularDriveParams(50.f, 5.f, 0.f);

	Bottomconstraint->SetAngularDriveMode(EAngularDriveMode::SLERP);
	Bottomconstraint->SetOrientationDriveSLERP(true);
	Bottomconstraint->SetAngularVelocityDriveSLERP(true);
	Bottomconstraint->SetAngularOrientationTarget(FRotator(0.f));
	Bottomconstraint->SetAngularVelocityTarget(FVector(0.f));
	Bottomconstraint->SetAngularDriveParams(50.f, 5.f, 0.f);


}


//if true, Door를 Interact한 Player는 Door의 뒤에 위치한것.
bool ADoor::IsPlayerBehind(AActor* Actor)
{
	bool bReturn = false;
	//양방향 문이 아니면 바로 false를 리턴해 단방향 문임을 알린다.
	if(bIsDuplexDoor == false) return bReturn;


	//Door(this)의 Location을 Player(Actor)의 Transform객체에 변환함.
	float Direction = Actor->GetActorTransform().InverseTransformPosition(this->GetActorLocation()).Y;

	//변환된 Y값이 1보다 크면 앞, 작으면 뒤 
	//이게 원래 X값이 맞는데 희한하게 Y값이 기준으로 되버림.원래 Y하면 Right/Left인데. 이상함 암튼 좀 그럼
	if (Direction >= 1.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADoor::IsPlayerBehind // Front!"));
	}
	else
	{
		bReturn = true;
		UE_LOG(LogTemp, Warning, TEXT("ADoor::IsPlayerBehind // behind!"));
	}


	//FVector DoorLoc = this->GetActorLocation();
	//FVector DoorFwdVec = this->GetActorForwardVector();

	//FVector ActorLoc = Actor->GetActorLocation();

	////Door에서 Actor로의 방향 벡터
	//FVector DirToActor = (ActorLoc - DoorFwdVec).GetSafeNormal();

	//float DotResult = FVector::DotProduct(DoorFwdVec,DirToActor);
	////behind	
	//if (DotResult < 0.f)
	//{
	//	bReturn = true;
	//	UE_LOG(LogTemp, Warning, TEXT("ADoor::IsPlayerBehind // behind!"));
	//}
	////front
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("ADoor::IsPlayerBehind // Front!"));	
	//}


	return bReturn;
}