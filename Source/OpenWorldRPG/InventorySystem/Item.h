// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */

//아래 매크로는 Go do Definition으로 정의 확인 가능하다.
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class OPENWORLDRPG_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
	UItem();

	//사용할 item에 대한 텍스트 (Equip, food, etc..)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText UseActionText;

	//Item mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UStaticMesh* ItemMesh;

	//item thumbnail
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;

	//inventory에서 표시될 item 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText ItemDisplayName;

	//item에 대한 추가 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (Multiline = true))
	FText ItemDescription;

	//사용하지 않을거임.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	float Weight;



	/* 이 item이 속한 inventory */
	UPROPERTY()
	class UInventoryComponent* OwningInventory;


	UPROPERTY(Transient)
	class UWorld* World; //item에 spawn particle등의 효과를 적용할거기 때문에 무슨 월드인지 알아야한다
	 
	virtual class UWorld* GetWorld() const { return World; };

	//PURE VIRTUAL -> c++이 pure virutal function을 정의하는 방법, 이 클래스에서 파생된 클래스에서 호출하며 사용하는데  Abstract class에서 호출하는게 말이 안되기 때문에 키워드를 사용함.
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Item) //Blueprint Native Event를 사용하려 했지만, 이렇게 하면 virtual키워드를 사용하지 못한다.
	virtual void Use(class AMainCharacter* Character) PURE_VIRTUAL(UItem, );

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AMainCharacter* Character);  //위 함수의 블루프린트 버전임. 위 함수에서 BNE를 사용해서 해봤다. 저게 되면 안쓸예정이다.
};
