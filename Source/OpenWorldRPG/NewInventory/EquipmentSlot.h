// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/Item/ItemInterface.h"
#include "EquipmentSlot.generated.h"


/* 
* 단순히, Image와 Border색상만을 변경한다.

*/
class UImage;
class UBorder;
class UNewItemObject;

class ABaseCharacter;

UENUM(BlueprintType)
enum class ERifleSlot :uint8
{
	ERS_Primary		UMETA(DisplayName = "Primary"),
	ERS_Sub			UMETA(DisplayName = "Sub"),

	ERS_MAX			UMETA(DisplayName = "defaultsMAX")
};

UCLASS()
class OPENWORLDRPG_API UEquipmentSlot : public UUserWidget, public IItemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetVariable")
	EEquipmentType SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetVariable")
	ERifleSlot RifleSlotType;

	//UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	//UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;


	ABaseCharacter* LootedChar_Owner;

	void PaintBGBorder(UNewItemObject* Obj = nullptr);


	bool IsSupportedEquip(UNewItemObject* Obj);
	bool TrySlotEquip(UNewItemObject* Var_ItemObj);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
