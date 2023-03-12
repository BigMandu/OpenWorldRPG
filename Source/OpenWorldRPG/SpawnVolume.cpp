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
* 실제 while문의 반복은 표면상 loop변수를 이용하지만
* 0~DTCount-1 번 돌면서 DTCount만큼의 횟수를 채우기 위해 //spawn 실패시 무한loop, 방지하기 위함.
* Spawn성공시 ActualSpawnCount변수를 증가시키고, loop에 대입해준다.
* 
* 실패시에는 loop를 증가시켜 GetSpawnItem에서 다음 항목(case)의 Item을 가져오도록하며,
* 계속 실패하여 DTCount-1만큼 왔을때 ActualSpawnCount가 목표(DTCount)에 이르지 못한 경우 loop를 강제 0으로 맞춰
* 시작부터 다시 시작하도록 한다.
*/
void ASpawnVolume::StartVolumeSpawn()
{
	//DTCount는 DataTable의 개수.
	//만일 이걸 수정하게 된다면, GetSpawnItem의 Switch문에도 case를 추가 또는 삭제를 해야한다.
	const int32 DTCount = 8;
	int32 SaveMinDTTableindex = DTCount;
	int32 loop = 0;

	int32 ActualSpawnCount = 0;
	bool bWasSpawn = false;

	while (loop < DTCount)
	{
		//ActualSpawncount가 SpawnCount이상이면 while문을 빠져 나간다.
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

				//bCanSpawn일때의 가장 최소 DTTable(loop)값을 갱신한다. -> 132 line
				SaveMinDTTableindex = (SaveMinDTTableindex > loop) ? loop : SaveMinDTTableindex;

				//Adjust Item Location,
				WantToSpawn->SetActorLocation(SpawnVector);
				UE_LOG(LogTemp, Warning, TEXT("SpawnVolume::StartVolumeSpawn // ActualCount ; %d"), ActualSpawnCount);
			}
			//스폰 불가할 경우
			else
			{
				if (loop >= DTCount -1 && ActualSpawnCount < SpawnCount)
				{
					//Spawn할 수 없을때, 0이 아닌 Spawn가능한 가장 최소 DT부터 시작하기 위해 대입한다.
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
		//OnGroud가 아닌경우
		else
		{
			
		}
	}
}

void ASpawnVolume::StartPointSpawn()
{
	
}



//나중에는 Spawn할 Item의 사이즈를 가져와야 한다.
//Spawn을 검증한다.
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

	//맞은게 있다면, ItmeSize를 넣을수 없으므로 스폰할수 없다.
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
		//TableTypeNumber가 4~7사이 (EquipmentType)인 경우 SpawnEquip함수를 호출한다.
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


//아래 방향으로 Ray를 쏘고 Vector를 리턴한다.
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
		//Z제한이 있다면
		FString HitActorName;
		Hit.GetActor()->GetName(HitActorName);
		bool bIsLandScape = HitActorName.Contains(TEXT("landscape"));
		bool bIsPlane = HitActorName.Contains(TEXT("plane"));
		
		//LandScape, Plane을 때리지 않은 경우만 CheckLimitZ를 검증한다.
		if (!bIsLandScape && !bIsPlane)
		{			
			if (bLimitZ && CheckLimitZ(Hit))
			{
				bIsLimitAllow = true;
			}
		}
		//landscape나 plane을 때린경우엔, 아래 verifyCanSpawn함수 호출을 위해 LimitAllow를 true로 변경해준다.
		else if (bIsLandScape || bIsPlane)
		{
			bIsLimitAllow = true;
		}
		
		
		if ((bLimitZ == true && bIsLimitAllow == true) || bLimitZ == false)
		{
			//HitLocation에 그대로 Item을 Spawn하게 되면 아래로 묻히는 현상이 발생
			//ItemSize의 높이 만큼 올려서 Verify를 하고, 해당 위치를 return한다.
			AdustHitLocation = Hit.Location + FVector(0.f,0.f,ItemSize.Z+10.f);

			if (VerifyCanSpawn(AdustHitLocation, ItemSize))
			{
				bCanSpawn = true;
			}
		}
		
	}

	return AdustHitLocation;
}


//4방향 아래로 Ray를 쏴 맞은 Vector의 높이를 계산한다.
bool ASpawnVolume::CheckLimitZ(FHitResult HitResult)
{
	bool bCanSpawn = false;
	if (HitResult.Actor.IsValid())//   && HitResult.Actor->getf) //이름에 LandScape가 들어갔으면 if문을 안들어가게한다.
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
			/* Up||Down Side면, X와 Y 선분을 리턴한다.
			* 정면,뒤쪽 side면 X와 Z,,등등
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
	/** Num1에 대한 SpawnLocation의 X, -X, Y, -Y와
	*  Num2에 대한 SpawnLocation의 X, -X, Y, -Y를 계산하기 위해 4번씩 loop를 돌린다.
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
		Point = FVector(Point.X,Point.Y,Point.Z + 10.f); //같은 Actor를 Hit하는걸 방지하기위해 살짝 올려서 Ray를 때린다.

		//살짝 올린만큼 더해서 Ray의 끝지점을 맞춘다.
		DrawDebugLine(World,SpawnLocation,Point, Color,false,3.f,0,3.f);
		DrawDebugLine(World,Point, Point + Point.DownVector * (CharHeight+10.f), Color,false,3.f,0,3.f);

		//SpawnVolume Ray가 때린 Actor는 검증에서 제외한다.
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