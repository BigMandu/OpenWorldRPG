// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "VideoOption.generated.h"

/**
 * 
 */

 class UComboBoxString;
 class UCheckBox;

UCLASS()
class OPENWORLDRPG_API UVideoOption : public UUserWidget
{
	GENERATED_BODY()

	TArray<FString> ResolutionOptionStr;
	TArray<FString> MaxFPSOptionStr;

	/* Windowed Option Needed Localization */
	TArray<FString> WindowedOptionStr_EN;


	/* Video Detail Five Option Needed Localization */
	//English
	TArray<FString> FiveOptionStr;

	/*
	//Simplified Chinese
	TArray<FString> FiveOptionStr_SIM_CN;
	//Russian
	TArray<FString> FiveOptionStr_RU;
	//Spanish-Spain
	TArray<FString> FiveOptionStr_SP_SP;
	//Portuguese-Brazil
	TArray<FString> FiveOptionStr_PT_BR;
	//German
	TArray<FString> FiveOptionStr_GR;
	//Japanese
	TArray<FString> FiveOptionStr_JP;
	//French
	TArray<FString> FiveOptionStr_FR;
	//Polish
	TArray<FString> FiveOptionStr_PL;
	//Turkish
	TArray<FString> FiveOptionStr_TU;
	//Korean
	TArray<FString> FiveOptionStr_KR;
	*/


public:
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* ResolutionCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* WindowModeCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* MaxFPSCombo;

	UPROPERTY(meta = (bindwidget))
	UCheckBox* VerticalSyncCheck;
	UPROPERTY(meta = (bindwidget))
	UCheckBox* MotionBlurCheck;

	/* Five Option */
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* TextureCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* EffectCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* ShadowCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* FoliageCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* ViewDistCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* AntiAliasingCombo;
	UPROPERTY(meta = (bindwidget))
	UComboBoxString* PostProcessCombo;
	
	

private:
	void AddOptionToComboBoxBasedOnLang();
	void SettingOptionStringArray();

	void AddOptionToCombo(UComboBoxString& Combo, TArray<FString>& StringArray);

	void SetDefaultValueOfComboBasedOnSavedOption();

	/* Get Selected Combobox value */
	float GetSelectedResolutionValue();
	enum EWindowMode::Type GetSelectedWindowMode();
	float GetSelectedFrameRateLimit();
	int32 GetSelectedFiveOption(UComboBoxString& FiveOptionCombo);

	FString GetSavedResolution(UGameUserSettings& Settings);
	FString GetSavedWindowMode(UGameUserSettings& Settings);
	FString GetSavedFPSLimit(UGameUserSettings& Settings);
	FString GetSavedFiveOption(int32 QualityValue);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void SetOption();


};
