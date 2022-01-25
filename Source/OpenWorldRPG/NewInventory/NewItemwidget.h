// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewItemwidget.generated.h"

/**
 * 
 */
class UNewItemObject;
class USizeBox;
class UBorder;
class UImage;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoved, UObject*);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoved, TWeakPtr<UNewItemObject>, ItemObj);

UCLASS()
class OPENWORLDRPG_API UNewItemwidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UNewItemObject* ItemObj;
	float Tilesize;
	FVector2D widgetsize;

	/* delegate, NewItemwidget::GetIconIamge���� broad cast, 
	* NewInventoryGrid::RefreshInventory���� NewInventoryGrid::OnItemRemove
	* �� bind��Ŵ.
	* Item�� ���������� ȣ��ǵ���.
	*/
	FOnRemoved OnRemoved;

	UPROPERTY(EditAnywhere, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BackgroundSizeBox;

	UPROPERTY(EditAnywhere, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BackgroundBorder;

	UPROPERTY(EditAnywhere, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void Refresh();

	UFUNCTION()
	FSlateBrush GetIconImage();
};
