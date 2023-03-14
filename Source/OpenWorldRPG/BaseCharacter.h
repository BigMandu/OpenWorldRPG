// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OpenWorldRPG/NewInventory/Library/Interactive_Interface.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "Perception/AISightTargetInterface.h"
#include "BaseCharacter.generated.h"


//WeaponStatusWidget에서 receive한다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeWeapon, AWeapon*, EquipWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetAmmo, AWeapon*, EquipWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeMode, AWeapon*, EquipWeapon);



class USoundCue;
class UMainAnimInstance;
class UCharacterLootWidget;
class UItemStorageObject;

class UAudioComponent;
class UNewInventoryComponent;
class UEquipmentComponent;
class ULootWidgetComponent;
class UStatManagementComponent;
class USpawnItemEquipComponent;

class AItem;
class AEquipment;
class AContainer;
class AWeapon;

UENUM()
enum class ETeamType : uint8
{
	ETT_Blue	UMETA(DisplayName = "Blue"),
	ETT_Red		UMETA(DisplayName = "Red"),

	ETT_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UENUM()
enum class ECharacterStatus : uint8
{
	EPS_Dead		UMETA(DisplayName = "Dead"),
	EPS_Crouch		UMETA(DisplayName = "Crouch"),
	EPS_Normal		UMETA(DisplayName = "Normal"),
	EPS_Walk		UMETA(DisplayName = "Walk"),
	EPS_Sprint		UMETA(DisplayName = "Sprint"),
	 
	EPS_MAX			UMETA(DisplayName = "DefaultsMAX")
};

UENUM(BlueprintType)
enum class EAimMode : uint8
{
	EAM_Aim				UMETA(DisplayName = "Aim"),
	EAM_NotAim			UMETA(DisplayName = "NotAim"),

	EAM_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class OPENWORLDRPG_API ABaseCharacter : public ACharacter, public IAISightTargetInterface, public IInteractive_Interface
{
	GENERATED_BODY()

	

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	FOnChangeWeapon OnChangeWeapon;
	FOnGetAmmo OnGetAmmo;
	FOnChangeMode OnChangeMode;


	/* Socket Name */
	const FName HeadSocketName = FName("headsocket");
	const FName GripSocketName = FName("WeaponGrip");
	//Item을 잡을 LeftHand Socket

	const FName LeftHandGripSocketName = FName("handLeftGripPos");

	//Weapon을 잡을 LeftHand의 Position (Weapon's socket)
	const FName WeaponLeftHandSocketName = FName("LeftHandPos");

	/* Enums */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECharacterStatus ChracterStatus;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	ETeamType TeamType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EAimMode AimMode;


	bool bIsDie;

	/* Movement */
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MaxWalkSpeed = 600.f; //뛰는 속도
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MinWalkSpeed = 300.f; //걷는 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	bool bIsAim;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
	USoundCue* StepSoundCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	AContainer* InteractLootBox;
	
	/* Fall Damage */
	FHitResult FallHit;
	float FallingHighestZ;

	TWeakObjectPtr<class AItem> HoldingItem;
	/* Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* SubWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon")
	AWeapon* PistolWeapon;

	/*Consume*/
	float RecoveryTickTime = 0.f;

	//아래 Widget들을 대신해 Widgetcomponent를 사용함.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LootWidget")
	//TSubclassOf<UCharacterLootWidget> WCharLootWidget;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LootWidget")
	//UCharacterLootWidget* CharLootWidget;


	///* Spawn Items */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems")
	//bool bHasSpawnItems;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bHasSpawnItems"))
	//TArray<TSubclassOf<AItem>> SpawnItemList;

	///* Spawn Equipments */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems")
	//bool bHasSpawnEquipments;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnItems", meta = (EditCondition = "bHasSpawnEquipments"))
	//TArray<TSubclassOf<AEquipment>> SpawnEquipmentList;


	/* Storage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 PocketSizeX = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 PocketSizeY = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	float PocketTileSize = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 SecureSizeX = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 SecureSizeY = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	float SecureTileSize = 60.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	UItemStorageObject* PocketStorage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	UItemStorageObject* SecureBoxStorage;

	/*  Components */
	UPROPERTY(VisibleAnywhere, Category = "Component | DataComponent")
	UAudioComponent* AudioComp;

	UPROPERTY(VisibleAnywhere, Category = "Component | DataComponent")
	UStatManagementComponent* StatManagementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component | Item")
	UNewInventoryComponent* BaseInventoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component | Item")
	USpawnItemEquipComponent* SpawnItemEquipComp;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	//UNewInventoryComponent* PocketInventoryComp;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	//UNewInventoryComponent* SecureBoxInventoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component | Item")
	UEquipmentComponent* Equipment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component | Item")
	ULootWidgetComponent* LootWidgetComp;

	/* Anim Instance */
	UMainAnimInstance* TPAnimInstance;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	
	void SetCharacterStatus(ECharacterStatus Type);
	void SetTeamType(ETeamType Team);
	FORCEINLINE ETeamType GetTeamType() { return TeamType; }
	virtual void SetAimMode(EAimMode Mode);

	///**** Spawn items *******/
	//void SpawnItems();
	//void SpawnEquipments();

	/* Set Storage */
	void SettingStorage();

	void SetEquippedWeapon(AWeapon* Weapon);

	/****************** Weapon ******************************/

	/*Weapon의 Pri, Sub, Pistol을 지정한다.
	* RifleSlot이 MAX인 경우, Pistol로 지정한다.
	*/
	UFUNCTION()
	void SetWeaponAssign(AWeapon* Weapon, ERifleSlot RifleSlot);

	void ReloadWeapon();
	void ChangePrimaryWeapon();
	void ChangeSubWeapon();
	void ChangePistolWeapon();

	/* Change Weapon Firing Mode*/
	void ChangeSafetyLever();

	virtual bool ChangeWeapon(int32 index);	

	/* Weapon Ammo */
	bool CheckAmmo();
	bool CheckAmmoStep(UItemStorageObject* Storage, int32& AmmoCnt, bool bIsCheckAmmo = false);

	/* AmmoPerMag에서 남은 잔탄을 뺀, 장전 가능한 탄약 개수를 리턴한다. */
	int32 GetNumberofCanReload();

	/* GetNumberofCanReload함수에서 사용,
	* Storage에서 ammo를 찾아 장전시 필요한 탄약 개수를 구하고,
	* 모든 탄약을 찾지 못한 경우 남은 탄약의 개수를 리턴한다.
	*/
	int32 CollectRemainAmmo(UItemStorageObject* Storage, int32 NumberofAmmoReq);

	/* Pocket, Vest에 있는 Equipped Weapon에 Reload가능한 Type의 Ammo의 개수를 리턴한다.*/
	int32 GetTotalNumberofSameTypeAmmo();

	UFUNCTION()
	void UseItem(AActor* Item);

	// AIController::ItemFarming함수에서 사용됨
	UNewInventoryComponent* GetAllInvComp(int32 index);

	UFUNCTION(BlueprintCallable)
	FTransform LeftHandik();

	void SetHoldingItem(AItem* WantToHolding);

	//called from AnimMontage Notify (UMainAnim)
	UFUNCTION()
	void DetachThrowingObject();//ABaseCharacter* Actor);

	virtual void PlayUseItemAnim(AItem* Item);
	UFUNCTION()
	virtual void StopUseItemAnim();

	void DetachCoreUsableItem();

	void StepSound();
	void SpeakSound(USoundCue* Sound);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void RecoveryHealthDelegate(const float Time, const float RecoveryAmount, TWeakObjectPtr<AItem> UsingItemInfo);
	void RecoveryStaminaDelegate(const float Time, const float RecoveryAmount, TWeakObjectPtr<AItem> UsingItemInfo);

	//void RecoveryHealth(const FTimerHandle RecTimer, const float RecoveryTime, const float RecoveryTickAmount, float RecoveryTickTime);
	void RecoveryStamina(const float Time, const float RecoveryAmount);

	void HaltRecoveryTimer(FTimerHandle WantToStop);

	UFUNCTION()
	void Die();

	void CalcFallingDistance(FHitResult& _FallHit);
	void ApplyFallingDamage(FHitResult& _FallHit);

	/*******************************/
	virtual void Interaction(AActor* Actor) override;
	virtual void SetOutline() override;
	virtual void UnsetOutline() override;
	/********** Perception ********/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const;


};
