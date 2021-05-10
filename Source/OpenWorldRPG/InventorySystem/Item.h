// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */

//�Ʒ� ��ũ�δ� Go do Definition���� ���� Ȯ�� �����ϴ�.
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class OPENWORLDRPG_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
	UItem();

	//����� item�� ���� �ؽ�Ʈ (Equip, food, etc..)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText UseActionText;

	//Item mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UStaticMesh* ItemMesh;

	//item thumbnail
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UTexture2D* Thumbnail;

	//inventory���� ǥ�õ� item �̸�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText ItemDisplayName;

	//item�� ���� �߰� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (Multiline = true))
	FText ItemDescription;

	//������� ��������.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	float Weight;



	/* �� item�� ���� inventory */
	UPROPERTY()
	class UInventoryComponent* OwningInventory;


	UPROPERTY(Transient)
	class UWorld* World; //item�� spawn particle���� ȿ���� �����Ұű� ������ ���� �������� �˾ƾ��Ѵ�
	 
	virtual class UWorld* GetWorld() const { return World; };

	//PURE VIRTUAL -> c++�� pure virutal function�� �����ϴ� ���, �� Ŭ�������� �Ļ��� Ŭ�������� ȣ���ϸ� ����ϴµ�  Abstract class���� ȣ���ϴ°� ���� �ȵǱ� ������ Ű���带 �����.
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Item) //Blueprint Native Event�� ����Ϸ� ������, �̷��� �ϸ� virtualŰ���带 ������� ���Ѵ�.
	virtual void Use(class AMainCharacter* Character) PURE_VIRTUAL(UItem, );

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AMainCharacter* Character);  //�� �Լ��� �������Ʈ ������. �� �Լ����� BNE�� ����ؼ� �غô�. ���� �Ǹ� �Ⱦ������̴�.
};
