// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"




//UENUM(BlueprintType)
//enum class EInteractType : uint8
//{
//	EIT_Gizmo		UMETA(DisplayName = "EIT_Gizmo"),
//	EIT_Item		UMETA(DisplayName = "EIT_Item"),
//	EIT_Equipment	UMETA(DisplayName = "EIT_Equipment"),
//	EIT_LootBox		UMETA(DisplayName = "EIT_LootBox"),
//
//	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
//};


UENUM()
enum class ESpawnItemType : uint8 
{
	ESIT_Overall	UMETA(DisplayName = "Overall"),
	ESIT_Food		UMETA(DisplayName = "Food"),
	ESIT_Medical	UMETA(DisplayName = "Medical"),
	ESIT_Scrap		UMETA(DisplayName = "Scrap"),
	ESIT_Ammo		UMETA(DisplayName = "Ammo"),

	ESIT_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class ESpawnType : uint8 
{
	EST_Volume		UMETA(DisplayName = "Volume Type"),
	EST_Point		UMETA(DisplayName = "Point Type"),

	EST_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EWhichSide : uint8 
{
	EWS_RightLeft,
	EWS_UpDown,
	EWS_FrontRear,

	EWS_MAX
};


UCLASS()
class OPENWORLDRPG_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

private:

	/** If you add an additional DataTable type, you have to make changes or additions to the following: 
	 * Change variable 'DTCount', add boolean variable like 'bSpawnFood', add switch case in 'GetSpawnItem' function.
	 */
	
	class UDataTable* FoodDT;
	UDataTable* MedicalDT;
	UDataTable* ScrapDT;
	UDataTable* AmmoDT;

	UDataTable* RifleDT;
	UDataTable* HelmetDT;	
	UDataTable* VestDT;
	UDataTable* BackPackDT;

	UDataTable* EnemyDT;


	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
	
public:	
	ASpawnVolume();

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | Settings")
	ESpawnType SpawnType;

	/* SpawnType이 VolumeType일 때만 작동한다. // true - 땅에 붙어서 스폰, false - 공중에서 스폰 */
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | Settings")
	bool bOnGround;

	/* SpawnType이 VolumeType일 때, bOnGround가 true일 때만 작동한다.// true - 캐릭터 키 이상으로는 스폰 x, false - 제한없이 스폰*/
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | Settings", meta = (EditCondition = "bOnGround"))
	bool bLimitZ;

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | Settings", meta = (EditCondition = "bLimitZ"))
	bool bNeedDoubleCheck;


	//UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	//ESpawnItemType MustSpawnItemType;

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnEnemyAI;	

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnFood;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnMedical;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnScrap;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnAmmo;

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnHelmet;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnVest;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnBackPack;
	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SelectSpawnItem")
	bool bSpawnRifle;


	UPROPERTY(EditAnywhere, Category = "SpawnVolume | Settings")
	int32 SpawnCount = 8;

	UPROPERTY(VisibleAnywhere, Category = "SpawnVolume | Settings")
	FVector SpawnPoint;

	UPROPERTY(VisibleAnywhere, Category = "SpawnVolume | Settings")
	class UBoxComponent* SpawnVolume;

	
	UPROPERTY(VisibleAnywhere, Category = "SpawnVolume | Settings")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, Category = "SpawnVolume | SpawnActorManager")
	bool bNeedToHideWhenFarAway = false;

	/*This Array is for a 'Save And Load' System.
	//StartVolumeSpawn일 때 DestoryCount는 ActualspawnCount값을 가지게 된다.
	//이후 게임세션이 살아있을 동안, 해당 spawnvolume에서 spawn된 item을
	//player가 습득하게 된 경우 DestoryCount를 증가시킨다.
	//저장할때는 이 DestroyCount를 저장하게되고, load시에 이 DestoryCount를 비교하여
	해당 volume의 spawn 여부와 spawn개수를 결정한다.
	*/
	UPROPERTY(VisibleAnywhere, Category = "SpawnVolume | SpawnActorManager")
	int32 DestoryCount = 0;

	UPROPERTY(VisibleAnywhere, Category = "SpawnVolume | SpawnActorManager")
	int32 RemainingCount;
	


private:
	void TrySpawn();

	FVector GetPointInVolume();
	bool VerifyCanSpawn(const FVector WantToSpawn, const FVector SpawnItemSize);

	FVector GetGroundVector(const FVector OriginVector, bool& bCanSpawn, const FVector ItemSize);

	bool CheckLimitZ(FHitResult HitResult);
	bool StepCheckLimit_Loop1(FHitResult ParentHit, float CharHeight, FVector SpawnLocation, float Num1, FColor Color);
	//bool StepCheckLimit_Loop2(float CharHeight, FVector SpawnLocation, float Num1);

	void StartVolumeSpawn();
	void StartPointSpawn();

	EWhichSide GetWhichSide(FVector ImpactNormal, AActor* HitActor);

	AActor* GetSpawnItem(const int32 TableTypeNumber);

	void FinalStepSpawnActor(AActor* SpawnActor, const FVector& SpawnLocation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void RemoveSpawnedActorAtList(AActor* ChildActor);

	void IncreaseDestroyCount();

	void SetSpawnedActorVisibility(bool bVisible);
	

	
};
