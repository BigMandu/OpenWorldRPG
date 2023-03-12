// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnCount = 8;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> RifleDataTableObj(TEXT("/Game/GameData/Equip_RifleDataTable.Equip_RifleDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> HelmetDataTableObj(TEXT("/Game/GameData/Equip_HelmetDataTable.Equip_HelmetDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> BackPackDataTableObj(TEXT("/Game/GameData/Equip_BackPackDataTable.Equip_BackPackDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> VestDataTableObj(TEXT("/Game/GameData/Equip_VestDataTable.Equip_VestDataTable"));

	
	static ConstructorHelpers::FObjectFinder<UDataTable> FoodDataTableObj(TEXT("/Game/GameData/Item_FoodDataTable.Item_FoodDataTable"));


	if (RifleDataTableObj.Succeeded() && HelmetDataTableObj.Succeeded() && BackPackDataTableObj.Succeeded() && VestDataTableObj.Succeeded())
	{
		if (FoodDataTableObj.Succeeded())
		{
			RifleDT = RifleDataTableObj.Object;
			HelmetDT = HelmetDataTableObj.Object;
			BackPackDT = BackPackDataTableObj.Object;
			VestDT = VestDataTableObj.Object;

			FoodDT = FoodDataTableObj.Object;
		}
	}



	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(SpawnVolume);
	Billboard->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();




	switch (SpawnType)
	{
	case ESpawnType::EST_Volume:
		StartVolumeSpawn();
	break;
	case ESpawnType::EST_Point:
		StartPointSpawn();
	break;
	}
	
}


/*
* ���� while���� �ݺ��� ǥ��� loop������ �̿�������
* 0~DTCount-1 �� ���鼭 DTCount��ŭ�� Ƚ���� ä��� ���� //spawn ���н� ����loop, �����ϱ� ����.
* Spawn������ ActualSpawnCount������ ������Ű��, loop�� �������ش�.
* 
* ���нÿ��� loop�� �������� GetSpawnItem���� ���� �׸�(case)�� Item�� �����������ϸ�,
* ��� �����Ͽ� DTCount-1��ŭ ������ ActualSpawnCount�� ��ǥ(DTCount)�� �̸��� ���� ��� loop�� ���� 0���� ����
* ���ۺ��� �ٽ� �����ϵ��� �Ѵ�.
*/
void ASpawnVolume::StartVolumeSpawn()
{
	//DTCount�� DataTable�� ����.
	//���� �̰� �����ϰ� �ȴٸ�, GetSpawnItem�� Switch������ case�� �߰� �Ǵ� ������ �ؾ��Ѵ�.
	const int32 DTCount = 8;
	int32 SaveMinDTTableindex = DTCount;
	int32 loop = 0;

	int32 ActualSpawnCount = 0;
	bool bWasSpawn = false;

	while (loop < DTCount)
	{
		//ActualSpawncount�� SpawnCount�̻��̸� while���� ���� ������.
		if (ActualSpawnCount >= SpawnCount)
		{
			break;
		}


		loop %= DTCount;

		const FVector SpawnLocation = GetPointInVolume();
		if (bOnGround)
		{
			DrawDebugPoint(GetWorld(), SpawnLocation, 5.f, FColor::Cyan, false, 3.f);

			bool bCanSpawn = false;
			AItem* WantToSpawn = GetSpawnItem(loop);
			FVector SpawnVector = FVector(0.f);
			if(WantToSpawn)
			{
				FVector SpawnItemSize = WantToSpawn->GetComponentsBoundingBox().GetSize();
				SpawnVector = GetGroundVector(SpawnLocation, bCanSpawn, SpawnItemSize);
			}
			
			if (bCanSpawn == true)
			{
				++ActualSpawnCount;
				bWasSpawn = true;

				//bCanSpawn�϶��� ���� �ּ� DTTable(loop)���� �����Ѵ�. -> 132 line
				SaveMinDTTableindex = (SaveMinDTTableindex > loop) ? loop : SaveMinDTTableindex;

				//Adjust Item Location,
				WantToSpawn->SetActorLocation(SpawnVector);
				UE_LOG(LogTemp, Warning, TEXT("SpawnVolume::StartVolumeSpawn // ActualCount ; %d"), ActualSpawnCount);
			}
			//���� �Ұ��� ���
			else
			{
				if (loop >= DTCount -1 && ActualSpawnCount < SpawnCount)
				{
					//Spawn�� �� ������, 0�� �ƴ� Spawn������ ���� �ּ� DT���� �����ϱ� ���� �����Ѵ�.
					loop = SaveMinDTTableindex;
				}
				else
				{
					++loop;
				}

				if(WantToSpawn)
				{
					WantToSpawn->Destroy();
				}
			}

		}
		//OnGroud�� �ƴѰ��
		else
		{
			
		}
	}
}

void ASpawnVolume::StartPointSpawn()
{
	
}



//���߿��� Spawn�� Item�� ����� �����;� �Ѵ�.
//Spawn�� �����Ѵ�.
//if return true : can Spawn, false : can't spawn.
bool ASpawnVolume::VerifyCanSpawn(const FVector WantToSpawn, const FVector SpawnItemSize)
{
	bool bReturnBool = true;
	FHitResult VerifyHit;

	/*FHitResult DynamicHit;
	FHitResult PhysicsHit;*/

	FCollisionShape BoxShape = FCollisionShape::MakeBox(SpawnItemSize);
	FCollisionQueryParams QParams(TEXT("SpawnVolume"), false, this);

	FCollisionResponseContainer CollisionContainer;
	CollisionContainer.SetResponse(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Ignore);
	
	FCollisionResponseParams RParams(CollisionContainer);

	
	bool bStaticHit = GetWorld()->SweepSingleByChannel(VerifyHit,WantToSpawn,WantToSpawn,FQuat::Identity,ECollisionChannel::ECC_WorldStatic, BoxShape,QParams, RParams);
	bool bDynamicHit = GetWorld()->SweepSingleByChannel(VerifyHit,WantToSpawn,WantToSpawn,FQuat::Identity,ECollisionChannel::ECC_WorldDynamic, BoxShape, QParams, RParams);
	bool bPhysicsHit = GetWorld()->SweepSingleByChannel(VerifyHit,WantToSpawn,WantToSpawn,FQuat::Identity,ECollisionChannel::ECC_PhysicsBody, BoxShape, QParams, RParams);

	TArray<AActor*> IgnoreArray;

	//DrawDebugBox(GetWorld(),WantToSpawn, BoxShape.GetBox(),FQuat::Identity,FColor::Green, false, 3.f,0, 3.f);
	UKismetSystemLibrary::BoxTraceSingle(this,WantToSpawn,WantToSpawn,SpawnItemSize/2,FRotator::ZeroRotator,ETraceTypeQuery::TraceTypeQuery1,true, IgnoreArray,EDrawDebugTrace::ForDuration,VerifyHit,true);
	

	if (bStaticHit || bDynamicHit || bPhysicsHit)
	{
		bReturnBool = false;
		UE_LOG(LogTemp, Warning, TEXT("SpawnVol::VerifyCanSpawn // Can't Spawn"));
		/*if(bStaticHit)
		{
			if(VerifyHit.IsValidBlockingHit() && VerifyHit.GetActor())
			UE_LOG(LogTemp,Warning,TEXT("SpawnVol::VerifyCanSpawn // HitActor : %s "), *VerifyHit.Actor->GetFName().ToString());
		}
		else if (bDynamicHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnVol::VerifyCanSpawn // HitActor : %s "), *DynamicHit.Actor->GetFName().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnVol::VerifyCanSpawn // HitActor : %s "), *PhysicsHit.Actor->GetFName().ToString());
		}*/
	}

	//������ �ִٸ�, ItmeSize�� ������ �����Ƿ� �����Ҽ� ����.
	return bReturnBool;// = bIsHit? false : true;
}


AItem* ASpawnVolume::GetSpawnItem(const int32 TableTypeNumber)
{

	int32 DTRowNum = 0;
	int32 DTRandIndex = 0;

	UBasePDA* SpawnPDA = nullptr;
	AItem* ReturnItem = nullptr;

	switch (TableTypeNumber)
	{
		case 0:
			if (bSpawnFood)
			{
				DTRowNum = FoodDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				
				FFoodTable* FoodDTRow = FoodDT->FindRow<FFoodTable>(*FString::FromInt(DTRandIndex), RifleDT->GetName());
				if(FoodDTRow == nullptr) return nullptr;
				SpawnPDA = FoodDTRow->FoodDataAsset;
			}
		break;
		case 1:
			if (bSpawnMedical)
			{
				DTRowNum = MedicalDT->GetRowMap().Num();
			}
		break;
		case 2:
			if (bSpawnScrap)
			{
				DTRowNum = ScrapDT->GetRowMap().Num();
			}
		break;
		case 3:
			if (bSpawnAmmo)
			{
				DTRowNum = AmmoDT->GetRowMap().Num();
			}
		break;
		case 4:
			if (bSpawnHelmet)
			{
				DTRowNum = HelmetDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				FHelmetTable* HelmetDTRow = HelmetDT->FindRow<FHelmetTable>(*FString::FromInt(DTRandIndex), HelmetDT->GetName());
				if (HelmetDTRow == nullptr) return nullptr;
				SpawnPDA = HelmetDTRow->HelmetDataAsset;
			}
		break;
		case 5:
			if (bSpawnVest)
			{

			}
		break;
		case 6:
			if (bSpawnBackPack)
			{

			}
		break;
		case 7:
			if (bSpawnRifle)
			{

			}
		break;
	}

	if(SpawnPDA != nullptr)
	{
		//TableTypeNumber�� 4~7���� (EquipmentType)�� ��� SpawnEquip�Լ��� ȣ���Ѵ�.
		if (TableTypeNumber >= 4 && TableTypeNumber <= 7)
		{
			
			ReturnItem = UCustomInventoryLibrary::SpawnEquipment(GetWorld(),Cast<UCustomPDA>(SpawnPDA));
		}
		else
		{ 
			ReturnItem = UCustomInventoryLibrary::SpawnItem(GetWorld(), SpawnPDA);
		}
		
	}



	return ReturnItem;
}


/************************************************************************/
/*                        for Volume Spawn                              */
/************************************************************************/

FVector ASpawnVolume::GetPointInVolume()
{
	FVector VolumeLo = SpawnVolume->GetComponentLocation();
	FVector VolumeEx = SpawnVolume->GetScaledBoxExtent();

	 return UKismetMathLibrary::RandomPointInBoundingBox(VolumeLo,VolumeEx);
}


//�Ʒ� �������� Ray�� ��� Vector�� �����Ѵ�.
FVector ASpawnVolume::GetGroundVector(const FVector OriginVector, bool& bCanSpawn, const FVector ItemSize)
{
	FVector AdustHitLocation = FVector(0.f); //return Vector

	FVector ToGround = FVector(OriginVector.X, OriginVector.Y, -1000.f);
	bool bIsLimitAllow = false;

	FHitResult Hit;
	FCollisionQueryParams params(FName("SpawnLineTrace"), true, GetInstigator());	
	
	DrawDebugLine(GetWorld(),OriginVector,ToGround,FColor::Red,false,3.f,0,2.f);
	GetWorld()->LineTraceSingleByChannel(Hit, OriginVector, ToGround, ECollisionChannel::ECC_WorldStatic, params);

	if (Hit.IsValidBlockingHit())
	{
		DrawDebugPoint(GetWorld(), Hit.Location, 5.f, FColor::Red, false, 3.f);	
		//Z������ �ִٸ�
		FString HitActorName;
		Hit.GetActor()->GetName(HitActorName);
		bool bIsLandScape = HitActorName.Contains(TEXT("landscape"));
		bool bIsPlane = HitActorName.Contains(TEXT("plane"));
		
		//LandScape, Plane�� ������ ���� ��츸 CheckLimitZ�� �����Ѵ�.
		if (!bIsLandScape && !bIsPlane)
		{			
			if (bLimitZ && CheckLimitZ(Hit))
			{
				bIsLimitAllow = true;
			}
		}
		//landscape�� plane�� ������쿣, �Ʒ� verifyCanSpawn�Լ� ȣ���� ���� LimitAllow�� true�� �������ش�.
		else if (bIsLandScape || bIsPlane)
		{
			bIsLimitAllow = true;
		}
		
		
		if ((bLimitZ == true && bIsLimitAllow == true) || bLimitZ == false)
		{
			//HitLocation�� �״�� Item�� Spawn�ϰ� �Ǹ� �Ʒ��� ������ ������ �߻�
			//ItemSize�� ���� ��ŭ �÷��� Verify�� �ϰ�, �ش� ��ġ�� return�Ѵ�.
			AdustHitLocation = Hit.Location + FVector(0.f,0.f,ItemSize.Z+10.f);

			if (VerifyCanSpawn(AdustHitLocation, ItemSize))
			{
				bCanSpawn = true;
			}
		}
		
	}

	return AdustHitLocation;
}


//4���� �Ʒ��� Ray�� �� ���� Vector�� ���̸� ����Ѵ�.
bool ASpawnVolume::CheckLimitZ(FHitResult HitResult)
{
	bool bCanSpawn = false;
	if (HitResult.Actor.IsValid())//   && HitResult.Actor->getf) //�̸��� LandScape�� ������ if���� �ȵ����Ѵ�.
	{
		float CharHeight = 180.f; //for Simulate
		//float CharHeight = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetDefaultHalfHeight() * 2;

		FVector SpawnLocation = HitResult.Location;
		FVector HitActorSize = HitResult.Actor->GetComponentsBoundingBox().GetSize();

		
		EWhichSide Side = GetWhichSide(HitResult.ImpactNormal, HitResult.GetActor());
		
		float Calc1 = 0.f;
		float Calc2 = 0.f;
		switch (Side)
		{
			/* Up||Down Side��, X�� Y ������ �����Ѵ�.
			* ����,���� side�� X�� Z,,���
			*/
			case EWhichSide::EWS_UpDown: // X && Y
				Calc1 = HitActorSize.X/2;
				Calc2 = HitActorSize.Y/2;
			break;
			case EWhichSide::EWS_FrontRear: // X && Z
				Calc1 = HitActorSize.X / 2;
				Calc2 = HitActorSize.Z / 2;
			break;
			case EWhichSide::EWS_RightLeft: // Y && Z
				Calc1 = HitActorSize.Y / 2;
				Calc2 = HitActorSize.Z / 2;
			break;
		}


		bCanSpawn = StepCheckLimit_Loop1(HitResult, CharHeight, SpawnLocation, Calc1, FColor::Green);
		
		if (bCanSpawn == false)
		{
			bCanSpawn = StepCheckLimit_Loop1(HitResult, CharHeight, SpawnLocation, Calc2, FColor::Blue);
		}	
	}


	return bCanSpawn;
}

bool ASpawnVolume::StepCheckLimit_Loop1(FHitResult ParentHit, float CharHeight, FVector SpawnLocation, float Num1, FColor Color)
{
	/** Num1�� ���� SpawnLocation�� X, -X, Y, -Y��
	*  Num2�� ���� SpawnLocation�� X, -X, Y, -Y�� ����ϱ� ���� 4���� loop�� ������.
	*/
	
	bool bReturn = false;

	TArray<FVector> CheckPoint;
	CheckPoint.Add(SpawnLocation + SpawnLocation.RightVector * Num1);
	CheckPoint.Add(SpawnLocation + SpawnLocation.RightVector * Num1 * -1);
	CheckPoint.Add(SpawnLocation + SpawnLocation.ForwardVector * Num1);
	CheckPoint.Add(SpawnLocation + SpawnLocation.ForwardVector * Num1 * -1);

	FHitResult Hit;
	UWorld* World = GetWorld();

	for (FVector Point : CheckPoint)
	{
		Point = FVector(Point.X,Point.Y,Point.Z + 10.f); //���� Actor�� Hit�ϴ°� �����ϱ����� ��¦ �÷��� Ray�� ������.

		//��¦ �ø���ŭ ���ؼ� Ray�� �������� �����.
		DrawDebugLine(World,SpawnLocation,Point, Color,false,3.f,0,3.f);
		DrawDebugLine(World,Point, Point + Point.DownVector * (CharHeight+10.f), Color,false,3.f,0,3.f);

		//SpawnVolume Ray�� ���� Actor�� �������� �����Ѵ�.
		FCollisionQueryParams Params(NAME_Name, false, ParentHit.GetActor());

		if (GetWorld()->LineTraceSingleByChannel(Hit, Point, Point+ Point.DownVector * (CharHeight + 10.f), ECollisionChannel::ECC_WorldStatic,Params))
		{
			if (Hit.IsValidBlockingHit() && Hit.Actor.IsValid())
			{
				if (bNeedDoubleCheck)
				{
					bReturn = CheckLimitZ(Hit);
				}
				else
				{ 
					bReturn = true;
				}
				break;								
			}
		}
	}

	return bReturn;
}





EWhichSide ASpawnVolume::GetWhichSide(FVector ImpactNormal, AActor* HitActor)
{

	EWhichSide ReturnSide = EWhichSide::EWS_MAX;

	for (int32 i = 0; i < 3; ++i)
	{
		float DotResult = 0.f;
		FVector UnitVec = FVector(0.f);

		if (i == 0)
		{
			ReturnSide = EWhichSide::EWS_UpDown;
			UnitVec = HitActor->GetActorUpVector();
		}
		else if (i == 1)
		{
			ReturnSide = EWhichSide::EWS_FrontRear;
			UnitVec = HitActor->GetActorForwardVector();
		}
		else
		{
			ReturnSide = EWhichSide::EWS_RightLeft;
			UnitVec = HitActor->GetActorRightVector();
		}

		DotResult = FVector::DotProduct(ImpactNormal, UnitVec);

		UE_LOG(LogTemp, Warning, TEXT("SpawnVol::GetWhichSide : %f"), DotResult);

		if((DotResult >= -0.9f && DotResult <= -1.1f) || (DotResult >= 0.9f &&DotResult <= 1.1f))
		{
			break;
		}

	}
	
	return ReturnSide;

}