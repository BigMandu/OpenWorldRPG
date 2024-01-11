// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/Equipment.h"

#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "DrawDebugHelpers.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> RifleDataTableObj(TEXT("/Game/GameData/Equip_RifleDataTable.Equip_RifleDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> HelmetDataTableObj(TEXT("/Game/GameData/Equip_HelmetDataTable.Equip_HelmetDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> VestDataTableObj(TEXT("/Game/GameData/Equip_VestDataTable.Equip_VestDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> BackPackDataTableObj(TEXT("/Game/GameData/Equip_BackPackDataTable.Equip_BackPackDataTable"));
	
	static ConstructorHelpers::FObjectFinder<UDataTable> AmmoDataTableObj(TEXT("/Game/GameData/Item_AmmoDataTable.Item_AmmoDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> FoodDataTableObj(TEXT("/Game/GameData/Item_FoodDataTable.Item_FoodDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> MedicalDataTableObj(TEXT("/Game/GameData/Item_MedicalDataTable.Item_MedicalDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> ScrapDataTableObj(TEXT("/Game/GameData/Item_ScrapDataTable.Item_ScrapDataTable"));

	static ConstructorHelpers::FObjectFinder<UDataTable> EnemyDataTableObj(TEXT("/Game/GameData/EnemyDataTable.EnemyDataTable"));



	if (RifleDataTableObj.Succeeded() && HelmetDataTableObj.Succeeded() && BackPackDataTableObj.Succeeded() && VestDataTableObj.Succeeded() &&
		FoodDataTableObj.Succeeded() && MedicalDataTableObj.Succeeded() && ScrapDataTableObj.Succeeded() && AmmoDataTableObj.Succeeded() &&
		EnemyDataTableObj.Succeeded())
	{	
		RifleDT = RifleDataTableObj.Object;
		HelmetDT = HelmetDataTableObj.Object;
		BackPackDT = BackPackDataTableObj.Object;
		VestDT = VestDataTableObj.Object;

		AmmoDT = AmmoDataTableObj.Object;
		FoodDT = FoodDataTableObj.Object;
		MedicalDT = MedicalDataTableObj.Object;
		ScrapDT = ScrapDataTableObj.Object;

		EnemyDT = EnemyDataTableObj.Object;
		
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

	TrySpawn();
}

void ASpawnVolume::TrySpawn()
{
	bool bNeedToSpawnItem = true;
	if (DestoryCount > 0)
	{
		//Load한 DestoryCount가 Spawncount이상이라면 더이상 spawn할 필요 없음. (player가 이미 다먹음)
		if (DestoryCount >= SpawnCount)
		{
			bNeedToSpawnItem = false;
			RemainingCount = 0;
		}
		else
		{
			//Player가 먹은개수를 뺀 숫자만큼 spawn한다.
			RemainingCount = SpawnCount - DestoryCount;
		}
	}
	else
	{
		RemainingCount = SpawnCount;
	}

	if (bNeedToSpawnItem)
	{
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
	const int32 DTCount = 9;
	int32 SaveMinDTTableindex = DTCount;
	int32 loop = 0;

	int32 ActualSpawnCount = 0;
	bool bWasSpawn = false;

	while (loop < DTCount)
	{
		//ActualSpawncount가 SpawnCount이상이면 while문을 빠져 나간다.
		if (ActualSpawnCount >= RemainingCount)
		{
			break;
		}

		loop %= DTCount;

		const FVector SpawnLocation = GetPointInVolume();
		DrawDebugPoint(GetWorld(), SpawnLocation, 5.f, FColor::Cyan, false, 3.f);

		AActor* WantToSpawn = GetSpawnItem(loop);
		bool bCanSpawn = false;		
		
		if(WantToSpawn)
		{
			if (bOnGround)
			{
			
				FVector SpawnItemSize = WantToSpawn->GetComponentsBoundingBox().GetSize();
				FVector SpawnVector = GetGroundVector(SpawnLocation, bCanSpawn, SpawnItemSize);
			}
			//OnGroud가 아닌경우
			else
			{
				//Spawn Item의 BoundingBox와 SpawnLocation 검증 필요.

				bCanSpawn = true;
			}
		}


		if (bCanSpawn)
		{
			++ActualSpawnCount;
			bWasSpawn = true;

			//bCanSpawn일때의 가장 최소 DTTable(loop)값을 갱신한다. -> 132 line
			SaveMinDTTableindex = (SaveMinDTTableindex > loop) ? loop : SaveMinDTTableindex;


			FinalStepSpawnActor(WantToSpawn, SpawnLocation);


			UE_LOG(LogTemp, Warning, TEXT("SpawnVolume::StartVolumeSpawn // ActualCount ; %d"), ActualSpawnCount);
		}
		//스폰 불가할 경우
		else
		{
			if (loop >= DTCount - 1 && ActualSpawnCount < RemainingCount)
			{
				//Spawn할 수 없을때, 0이 아닌 Spawn가능한 가장 최소 DT부터 시작하기 위해 대입한다.
				loop = SaveMinDTTableindex;
			}
			else
			{
				++loop;
			}

			if (WantToSpawn)
			{
				WantToSpawn->Destroy();
			}
		}


	}
}

void ASpawnVolume::StartPointSpawn()
{
	
}

void ASpawnVolume::FinalStepSpawnActor(AActor* SpawnActor, const FVector& SpawnLocation)
{
	//Adjust Item Location,
	if (IInteractive_Interface* Inter = Cast<IInteractive_Interface>(SpawnActor))
	{
		SpawnActor->SetActorLocation(SpawnLocation);
		Inter->SetMotherSpawnVolume(this);
		
		SpawnedActors.Add(SpawnActor);
	}
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


/* Loop index를 가져와서 switch문과 비교하게 되는데
* 이렇게 시간복잡도를 높게 잡은 이유는
* SpawnVolume에 spawn할 Item type이 여러개라면 여러 type의 item을 spawn하기 위함이다.
*/
AActor* ASpawnVolume::GetSpawnItem(const int32 TableTypeNumber)
{

	int32 DTRowNum = 0;
	int32 DTRandIndex = 0;

	UBasePDA* SpawnPDA = nullptr;
	AActor* ReturnItem = nullptr;

	switch (TableTypeNumber)
	{
		case 0:
			if (bSpawnFood)
			{
				DTRowNum = FoodDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);

				FFoodTable* FoodDTRow = FoodDT->FindRow<FFoodTable>(*FString::FromInt(DTRandIndex), FoodDT->GetName());
				if(FoodDTRow == nullptr) return nullptr;
				SpawnPDA = FoodDTRow->FoodDataAsset;
			}
		break;
		case 1:
			if (bSpawnMedical)
			{
				DTRowNum = MedicalDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);

				FMedicalTable* MedicalDTRow = MedicalDT->FindRow<FMedicalTable>(*FString::FromInt(DTRandIndex), MedicalDT->GetName());
				if (MedicalDTRow == nullptr) return nullptr;
				SpawnPDA = MedicalDTRow->MedicalDataAsset;
			}
		break;
		case 2:
			if (bSpawnScrap)
			{
				DTRowNum = ScrapDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);

				FScrapTable* ScrapDTRow = ScrapDT->FindRow<FScrapTable>(*FString::FromInt(DTRandIndex), ScrapDT->GetName());
				if (ScrapDTRow == nullptr) return nullptr;
				SpawnPDA = ScrapDTRow->ScrapDataAsset;
			}
		break;
		case 3:
			if (bSpawnAmmo)
			{
				DTRowNum = AmmoDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);

				FAmmoTable* AmmoDTRow = AmmoDT->FindRow<FAmmoTable>(*FString::FromInt(DTRandIndex), AmmoDT->GetName());
				if (AmmoDTRow == nullptr) return nullptr;
				SpawnPDA = AmmoDTRow->AmmoDataAsset;
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
				DTRowNum = VestDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				FVestTable* VestDTRow = VestDT->FindRow<FVestTable>(*FString::FromInt(DTRandIndex), VestDT->GetName());
				if (VestDTRow == nullptr) return nullptr;
				SpawnPDA = VestDTRow->VestDataAsset;
			}
		break;
		case 6:
			if (bSpawnBackPack)
			{
				DTRowNum = BackPackDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				FBackPackTable* BackPackDTRow = BackPackDT->FindRow<FBackPackTable>(*FString::FromInt(DTRandIndex), BackPackDT->GetName());
				if (BackPackDTRow == nullptr) return nullptr;
				SpawnPDA = BackPackDTRow->BackPackDataAsset;
			}
		break;
		case 7:
			if (bSpawnRifle)
			{
				DTRowNum = RifleDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				FRifleTable* RifleDTRow = RifleDT->FindRow<FRifleTable>(*FString::FromInt(DTRandIndex), RifleDT->GetName());
				if (RifleDTRow == nullptr) return nullptr;
				SpawnPDA = RifleDTRow->WeaponDataAsset;
			}
		break;
		case 8:
			if (bSpawnEnemyAI)
			{
				DTRowNum = EnemyDT->GetRowMap().Num();
				DTRandIndex = FMath::RandRange(1, DTRowNum);
				FEnemyDataTable* EnemyDTRow = EnemyDT->FindRow<FEnemyDataTable>(*FString::FromInt(DTRandIndex), EnemyDT->GetName());
				if(EnemyDTRow == nullptr) return nullptr;

				ReturnItem = UCustomSystemLibrary::SpawnAIChar(GetWorld(), *EnemyDTRow);

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

void ASpawnVolume::IncreaseDestroyCount()
{	
	++DestoryCount;
	UE_LOG(LogTemp, Warning, TEXT("ASpawnVolume::IncreaseDestroyCount //DestCnt : %d"), DestoryCount);
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

		/** 맞은 Actor의 가로, 세로, 높이를 고려해 각 길이를 반을 나눠 각 면으로 부터 수직하는 아래에 lay를 4방향으로 쏜다.
		 * ex) 만약 맞은 Actor의 Up이나 Down인 경우 해당하는 Actor가 바닥에서 얼마만큼 높이 있는가를 측정하기 위해서
		 * 박스의 X, Y길이를 반으로 나눈 값을 넘겨준다.
		 * 그리고 이 값들을 고려하지 않고 4번씩 lay를 쏘도록 한다.
		 * (이 Actor의 중앙에서 부터 아주 살짝 벗어난 경계지점이 x/2, y/2값이 되므로.)
		 * lay의 길이가 character의 높이보다 길다면 spawn불가. -> character가 해당 item을 먹을수 없으므로..
		 */
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


void ASpawnVolume::SetSpawnedActorVisibility(bool bVisible)
{
	for (TWeakObjectPtr<AActor> SpanwedActor : SpawnedActors)
	{
		if (SpanwedActor.IsValid())
		{
			if(bVisible)
			{
				SpanwedActor->GetRootComponent()->SetVisibility(true, true);
			}
			else
			{
				SpanwedActor->GetRootComponent()->SetVisibility(false, true);
			}
		}
	}
}

void ASpawnVolume::RemoveSpawnedActorAtList(AActor* ChildActor)
{
	if (ChildActor)
	{
		if (SpawnedActors.Contains(ChildActor))
		{
			SpawnedActors.RemoveSingle(ChildActor);
		}
	}
}