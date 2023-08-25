// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntelAcquiredListWidget.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UIntelAcquiredListWidget : public UUserWidget
{
	GENERATED_BODY()
	TMap<int32, UUserWidget*> PageWidgetsMap;
	int32 CurrentViewPageNumber;
public:
	UPROPERTY(meta = (bindwidget))
	class UListView* IntelList;

	UPROPERTY()
	TWeakObjectPtr<UDataTable> IntelDataTable;

	/*UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntelListEntryWidget>W_IntelListEntryWidget;*/
	//class UUserWidget* W_IntelListEntryWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntelPageBaseWidget>W_IntelPageBaseWidget;

	UPROPERTY()
	UIntelPageBaseWidget* WidgetIntelPageBase;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntelPageOneWidget>W_IntelPageOneWidget;
	//UIntelPageOneWidget* WidgetIntelPageOne;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntelPageOtherWidget>W_IntelPageOtherWidget;


private:
	void SetListView();
	void CreateBasePageWidget();

	void SettingIntelPage(const struct FIntelligenceDataTable& RowData);

	void DataIntoBasePage(const FIntelligenceDataTable& RowData);
	void DataIntoWidget(class UUserWidget* Widget, const FIntelligenceDataTable& RowData, const int32& pageindex);

	

protected:
	virtual void NativeConstruct() override;

public:
	//UFUNCTION()
	void ReadIntel(EIntelType ClickIntelType);
	
	UFUNCTION()
	void ExitIntelPage();
	UFUNCTION()
	void ToNextIntelPage();
	UFUNCTION()
	void ToPrevIntelPage();
};
