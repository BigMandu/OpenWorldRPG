// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
//#include "OpenWorldRPG/NewInventory/InventoryComponent.h"
#include "OpenWorldRPG/NewInventory/Library/ItemInterface.h"
#include "Item.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemUseEnd);
/**
 * 
 */
class UNewInventoryComponent;
class UNewItemObject;
class UMaterialInterface;
class UAIPerceptionStimuliSourceComponent;
class ABaseCharacter;
class AEquipment;

//UENUM(BlueprintType)
//enum class EEquipmentType : uint8
//{
//	EET_Rifle		UMETA(DisplayName = "Rifle"),
//	EET_Pistol		UMETA(DisplayName = "Pistol"),
//	EET_Helmet		UMETA(DisplayName = "Helmet"),
//	EET_Plate		UMETA(Displayname = "Plate Carrier"),
//	EET_Vest		UMETA(DisplayName = "Vest"),
//	EET_Backpack	UMETA(DisplayName = "Backpack"),
//
//	EET_MAX			UMETA(DisplayName = "DefaltMAX")
//};



UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Spawn	UMETA(DisplayName = "Spawn"),
	EIS_Pickup	UMETA(DisplayName = "Pickup"),

	EIS_MAX		UMETA(DisplayName = "DefaultsMAX")
};

UCLASS()
class OPENWORLDRPG_API AItem : public AInteractable //, public IItemInterface
{
	GENERATED_BODY()
	//GENERATED_UCLASS_BODY()
protected:

	TWeakObjectPtr<class AWeapon> BeforeEquipppedWeapon = nullptr;

public:

	AItem();
	
	FItemUseEnd ItemUseEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	UNewItemObject* ItemObj;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	EItemState ItemState;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UMaterialInterface* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UMaterialInterface* IconRotated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FIntPoint ItemSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bCanRotate;*/

	UNewInventoryComponent* OwningInventory; // Obj로 빼기

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bCanEquip;*/

	UPROPERTY(EditDefaultsOnly, Category = AI)
	UAIPerceptionStimuliSourceComponent* StimuliComp;	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	void RemoveCountAtIventory(ABaseCharacter* Char, int32 RemoveCount = 0);
	void SettingItemObj(UNewItemObject* Obj);
public:

	virtual void PostInitializeComponents() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE EItemState GetItemState() { return ItemState; }

	FORCEINLINE void SetItemState(EItemState State) { ItemState = State; }

	/* child class에서 override할 예정임*/
	//virtual UNewItemObject* GetDefaultItemObj();

	bool Pickup(AActor* Actor, UNewItemObject* obj = nullptr);

	bool AddAtEquip(AEquipment* Equipped);
	bool AddAtCharInv(ABaseCharacter* Character, UItemStorageObject* InvComp);


	
	UFUNCTION()
	virtual void Drop();

	/** Item을 Actor에 Attach하기 전 필수 작업을 하는 함수.
	 *	@Actor - Actor to which this Item will be attached.
	 *  @Obj - Attached Item's Object.
	 *  @bIsNeedToDestory - Must Setting TRUE, when creating an item to be attached. it to prevent duplicate.
	 *						Setting True list: UNewItemObject::UseItem
	 * 
	 *					  - Setting FALSE when re-attaching same item.
	 *						False list : AMainCharacter::VKeyDN
	 */
	virtual void AttachToHand(class ABaseCharacter* Actor, class UNewItemObject* Obj, bool bIsNeedToDestory = true);
	void AttachToHand_Step(ABaseCharacter* Actor);
	virtual void DetachFromHand(ABaseCharacter* Actor, bool bIsNeedToEquipBeforeWeapon);
	void EquipBeforeWeapon(ABaseCharacter* Actor);
	UFUNCTION()
	virtual void Use(ABaseCharacter* Actor, UNewItemObject* Obj = nullptr);

};