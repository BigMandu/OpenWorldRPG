// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Option/VideoOption.h"
#include "Components/ComboBoxString.h"
//#include "GameFramework/GameUserSettings.h"

void UVideoOption::NativePreConstruct()
{
	//ComboBox에 Option name을 세팅한다.
	AddOptionToComboBoxBasedOnLang();	
}

void UVideoOption::NativeConstruct()
{
	Super::NativeConstruct();

	SetOption();
	
}


void UVideoOption::AddOptionToComboBoxBasedOnLang()
{
	//use switch statement based on get culture
	SettingOptionStringArray();

	AddOptionToCombo(*ResolutionCombo, ResolutionOptionStr);
	AddOptionToCombo(*WindowModeCombo, WindowedOptionStr_EN);
	AddOptionToCombo(*MaxFPSCombo, MaxFPSOptionStr);

	AddOptionToCombo(*TextureCombo, FiveOptionStr);
	AddOptionToCombo(*EffectCombo, FiveOptionStr);
	AddOptionToCombo(*ShadowCombo, FiveOptionStr);
	AddOptionToCombo(*FoliageCombo, FiveOptionStr);
	AddOptionToCombo(*ViewDistCombo, FiveOptionStr);
	AddOptionToCombo(*AntiAliasingCombo, FiveOptionStr);
	AddOptionToCombo(*PostProcessCombo, FiveOptionStr);
}

void UVideoOption::SettingOptionStringArray()
{
	ResolutionOptionStr.Empty();
	MaxFPSOptionStr.Empty();
	WindowedOptionStr_EN.Empty();
	FiveOptionStr.Empty();

	ResolutionOptionStr.Add(TEXT("720p"));
	ResolutionOptionStr.Add(TEXT("1080p"));
	ResolutionOptionStr.Add(TEXT("1440p"));
	ResolutionOptionStr.Add(TEXT("2160p"));

	MaxFPSOptionStr.Add(TEXT("60"));
	MaxFPSOptionStr.Add(TEXT("120"));
	MaxFPSOptionStr.Add(TEXT("144"));
	MaxFPSOptionStr.Add(TEXT("Unlimited"));

	//아래는 culture lang에 따라 변동되어야함.

	WindowedOptionStr_EN.Add(TEXT("Window mode"));
	WindowedOptionStr_EN.Add(TEXT("Borderless Windowed Mode"));
	WindowedOptionStr_EN.Add(TEXT("FullScreen Mode"));

	FiveOptionStr.Add(TEXT("Low"));
	FiveOptionStr.Add(TEXT("Medium"));
	FiveOptionStr.Add(TEXT("High"));
	FiveOptionStr.Add(TEXT("Epic"));
	FiveOptionStr.Add(TEXT("Cinematic"));
}

void UVideoOption::AddOptionToCombo(UComboBoxString& Combo, TArray<FString>& StringArray)
{
	Combo.ClearOptions();

	for (auto Str : StringArray)
	{
		Combo.AddOption(Str);
	}
}

void UVideoOption::SetDefaultValueOfComboBasedOnSavedOption()
{
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		ResolutionCombo->SetSelectedOption(GetSavedResolution(*GameSettings));
		WindowModeCombo->SetSelectedOption(GetSavedWindowMode(*GameSettings));
		MaxFPSCombo->SetSelectedOption(GetSavedFPSLimit(*GameSettings));

		TextureCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetTextureQuality()));
		EffectCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetVisualEffectQuality()));
		ShadowCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetShadowQuality()));
		FoliageCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetFoliageQuality()));
		ViewDistCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetViewDistanceQuality()));
		AntiAliasingCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetAntiAliasingQuality()));
		PostProcessCombo->SetSelectedOption(GetSavedFiveOption(GameSettings->GetPostProcessingQuality()));
	}
}

void UVideoOption::SetOption()
{
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetResolutionScaleValueEx(GetSelectedResolutionValue());
		GameSettings->SetFullscreenMode(GetSelectedWindowMode());
		GameSettings->SetFrameRateLimit(GetSelectedFrameRateLimit());

		GameSettings->SetTextureQuality(GetSelectedFiveOption(*TextureCombo));
		GameSettings->SetVisualEffectQuality(GetSelectedFiveOption(*EffectCombo));
		GameSettings->SetShadowQuality(GetSelectedFiveOption(*ShadowCombo));
		GameSettings->SetFoliageQuality(GetSelectedFiveOption(*FoliageCombo));
		GameSettings->SetViewDistanceQuality(GetSelectedFiveOption(*ViewDistCombo));
		GameSettings->SetAntiAliasingQuality(GetSelectedFiveOption(*AntiAliasingCombo));
		GameSettings->SetPostProcessingQuality(GetSelectedFiveOption(*PostProcessCombo));		
	}	
}






////////////////////////////////////////////////////////
/*                 Get Selected Value                 */
////////////////////////////////////////////////////////

float UVideoOption::GetSelectedResolutionValue()
{
	FString ResStr = ResolutionCombo->GetSelectedOption();
	float SetResolutionVal = 1080.f;

	if (ResStr.Compare(TEXT("720p")) == 0)
	{
		SetResolutionVal = 720.f;
	}
	else if (ResStr.Compare(TEXT("1080p")) == 0)
	{
		SetResolutionVal = 1080.f;
	}
	else if (ResStr.Compare(TEXT("1440p")) == 0)
	{
		SetResolutionVal = 1440.f;
	}
	else if (ResStr.Compare(TEXT("2160p")) == 0)
	{
		SetResolutionVal = 2160.f;
	}

	return SetResolutionVal;
}

EWindowMode::Type UVideoOption::GetSelectedWindowMode()
{
	FString WindowStr = WindowModeCombo->GetSelectedOption();
	EWindowMode::Type Winmode = EWindowMode::Fullscreen;

	if (WindowStr.Compare(TEXT("Windowed")) == 0)
	{
		Winmode = EWindowMode::Windowed;
	}
	else if (WindowStr.Compare(TEXT("WindowedFullscreen")) == 0)
	{
		Winmode = EWindowMode::WindowedFullscreen;
	}
	else if (WindowStr.Compare(TEXT("FullScreen")) == 0)
	{
		Winmode = EWindowMode::Fullscreen;
	}

	return Winmode;
}

float UVideoOption::GetSelectedFrameRateLimit()
{
	float FrameLimitfloat = 60.f;

	FString FrameLimitStr = MaxFPSCombo->GetSelectedOption();
	//const FString FrameLimitWithoutQuotes = FrameLimitStr.TrimQuotes();
	FrameLimitfloat = FCString::Atof(*FrameLimitStr.TrimQuotes());
	
	return FrameLimitfloat;
}

int32 UVideoOption::GetSelectedFiveOption(UComboBoxString& FiveOptionCombo)
{
	FString SelectedOptionStr = FiveOptionCombo.GetSelectedOption();
	int32 option = 2;

	for ( int32 index = 0; index < FiveOptionStr.Num(); ++index)
	{
		//if equal
		if (FiveOptionStr[index].Compare(SelectedOptionStr) == 0)
		{
			option = index;
			break;
		}
	}
	return option;
}

////////////////////////////////////////////////////////
/*                  Get  Saved  Value                 */
////////////////////////////////////////////////////////

FString UVideoOption::GetSavedResolution(UGameUserSettings& Settings)
{
	FString ReturnResolVal = TEXT("Error");
	float CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue;
	Settings.GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue);

	int32 NeedConfirmindex = 1;

	if (CurrentScaleValue == 720.f)
	{
		NeedConfirmindex = 0;
	}
	else if (CurrentScaleValue == 1080.f)
	{
		NeedConfirmindex = 1;
	}
	else if (CurrentScaleValue == 1440.f)
	{
		NeedConfirmindex = 2;
	}
	else if (CurrentScaleValue == 2160.f)
	{
		NeedConfirmindex = 3;
	}

	//Array의 숫자보다 작아야 정상.
	if (NeedConfirmindex < ResolutionOptionStr.Num())
	{
		ResolutionOptionStr.Find(ReturnResolVal, NeedConfirmindex);
	}
	return ReturnResolVal;
}

FString UVideoOption::GetSavedWindowMode(UGameUserSettings& Settings)
{
	EWindowMode::Type CurrentWinMode = Settings.GetDefaultWindowMode();
	FString WindowModeReturnString = TEXT("Error");
	switch (CurrentWinMode)
	{
		case EWindowMode::Windowed:
			if (WindowedOptionStr_EN.IsValidIndex(0))
			{
				WindowModeReturnString = WindowedOptionStr_EN[0];
			}
		break;
		case EWindowMode::WindowedFullscreen:
			if (WindowedOptionStr_EN.IsValidIndex(1))
			{
				WindowModeReturnString = WindowedOptionStr_EN[1];
			}
		break;
		case EWindowMode::Fullscreen:
			if (WindowedOptionStr_EN.IsValidIndex(2))
			{
				WindowModeReturnString = WindowedOptionStr_EN[2];
			}
		break;
	}

	return WindowModeReturnString;
}

FString UVideoOption::GetSavedFPSLimit(UGameUserSettings& Settings)
{
	float CurrentFPSLimit = Settings.GetFrameRateLimit();
	FString ReturnFrameLimitString = TEXT("Error");

	if (CurrentFPSLimit == 0)
	{
		//unlimited
		if (MaxFPSOptionStr.IsValidIndex(3))
		{
			ReturnFrameLimitString = MaxFPSOptionStr[3];
		}
	}
	else if (CurrentFPSLimit == 60)
	{
		if (MaxFPSOptionStr.IsValidIndex(0))
		{
			ReturnFrameLimitString = MaxFPSOptionStr[0];
		}
	}
	else if (CurrentFPSLimit == 120)
	{
		if (MaxFPSOptionStr.IsValidIndex(1))
		{
			ReturnFrameLimitString = MaxFPSOptionStr[1];
		}
	}
	else if (CurrentFPSLimit == 144)
	{
		if (MaxFPSOptionStr.IsValidIndex(2))
		{
			ReturnFrameLimitString = MaxFPSOptionStr[2];
		}
	}
	return ReturnFrameLimitString;
}

FString UVideoOption::GetSavedFiveOption(int32 QualityValue)
{
	FString ReturnString = TEXT("Error");

	if (FiveOptionStr.IsValidIndex(QualityValue))
	{
		ReturnString = FiveOptionStr[QualityValue];
	}

	return ReturnString;
}