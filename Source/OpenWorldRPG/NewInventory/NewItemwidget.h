// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "NewItemwidget.generated.h"

/**
 * 
 */
class UUserWidget;
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
	UPROPERTY(BlueprintReadOnly)
	UNewItemObject* ItemObj;
	UPROPERTY(BlueprintReadOnly)
	float Tilesize;
	UPROPERTY(BlueprintReadOnly)
	FVector2D widgetsize;

	/* delegate, NewItemwidget::GetIconIamge에서 broad cast, 
	* NewInventoryGrid::RefreshInventory에서 NewInventoryGrid::OnItemRemove
	* 와 bind시킴.
	* Item을 삭제했을때 호출되도록.
	*/
	FOnRemoved OnRemoved;

	USizeBox* testsizebox;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	USizeBox* BGSizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UBorder* BGBorder;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetVariable", meta = (BindWidget))
	UImage* ItemIcon;

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	//virtual void SynchronizeProperties() override;
	
	//UFUNCTION(BlueprintNativeEvent)
	void Refresh();

	UFUNCTION(BlueprintCallable)
	FSlateBrush GetIconImage();
	
	/*FORCEINLINE_DEBUGGABLE WidgetT* ConstructWidget(TSubclassOf<UWidget>WidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None)
	{
		static_assert(TIsDerivedFrom<WidgetT, UWidget>::IsDerived, "WidgetTree::ConstructWidget can only create UWidget objects.");

		if (WidgetClass->IsChildOf<UUserWidget>())
		{
			return Cast<WidgetT>(CreateWidget(this, *WidgetClass, WidgetName));
		}

		return NewObject<WidgetT>(this, WidgetClass, WidgetName, RF_Transactional);
	}*/
};
