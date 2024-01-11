// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AdditionalWidget.generated.h"

/**
 *  NOT USED
 */

UCLASS()
class OPENWORLDRPG_API UAdditionalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<class UDraggInventoryWindow> WStorageWindow;
	
	UDraggInventoryWindow* StorageWindow;


	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ContentCanvas;
	

	UFUNCTION()
	void BindingOpenWidgetFunction(class UNewItemObject* ItemObj);

	void OpenAdditionalWidget(class UItemStorageObject* StorageObj);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
