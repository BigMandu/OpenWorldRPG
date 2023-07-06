// Copyright Epic Games, Inc. All Rights Reserved.


//Flicker 기능을 추가하려다가 실패.
/*
#include "OpenWorldRPGLoadingScreen/Public/OpenWorldRPGLoadingScreen.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "Widgets/Images/SThrobber.h"

#define LOCTEXT_NAMESPACE "OpenWorldRPGLoadingScreen"

struct FOpenWorldRPGLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FOpenWorldRPGLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if ( UObject* CachedResourceObject = GetResourceObject() )
		{
			Collector.AddReferencedObject(CachedResourceObject);
		}
	}
};

class SOpenWorldRPGLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOpenWorldRPGLoadingScreen) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		static const FName LoadingScreenName(TEXT("/Game/UI/IMG/Fix_SAC_LOGO_Black.Fix_SAC_LOGO_Black"));

		LoadingScreenBrush = MakeShareable(new FOpenWorldRPGLoadingScreenBrush(LoadingScreenName, FVector2D(500, 500)));

		ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(&BGBrush)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SAssignNew(LoadingScreenLogo, SImage)
				.Image(LoadingScreenBrush.Get())
				.ColorAndOpacity_Lambda([ this ] () { return GetLogoColorAndOpacity(); })
				//.ColorAndOpacity(this, &SOpenWorldRPGLoadingScreen::GetLogoColorAndOpacity)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(FMargin(1.f))
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::Black)
					.ShadowColorAndOpacity(FLinearColor::White)
					.ShadowOffset(FIntPoint(-1, 1))
					.Text(LOCTEXT("LoadingText", "Loading ..."))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(FMargin(10.0f))
				[
					SNew(SCircularThrobber)
					.Visibility(this, &SOpenWorldRPGLoadingScreen::GetLoadIndicatorVisibility)
				]
			]
		];

		// Start the fading animation
		FadingAnimation->Play(this->AsShared());
	}

private:
	TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
	TSharedPtr<SImage> LoadingScreenLogo;
	FSlateBrush BGBrush;
	TSharedPtr<FCurveSequence> FadingAnimation;

private:
	//Logo의 Opacity를 조절함.
	FLinearColor GetLogoColorAndOpacity() const
	{
		// Get the current alpha value from the fading animation
		FadingAnimation.Reset(new FCurveSequence());
		FadingAnimation->Reset();
		FadingAnimation->AddCurve(0.f, 0.5f, ECurveEaseFunction::QuadInOut);
		FadingAnimation->AddCurve(0.5f, 0.f, ECurveEaseFunction::QuadInOut);

		FadingAnimation->Play(this->AsShared(), true);
		TAttribute<float> OpacityAttribute = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([ this ] ()
			{
				return FadingAnimation->GetLerp();
			}));

		float Alpha = FadingAnimation->GetLerp();
		FLinearColor ColorAndOpacity();
		ColorAndOpacity.A = OpacityAttribute;

		//// Apply the alpha value to the logo color and opacity
		//FLinearColor ColorAndOpacity = FLinearColor::White;
		//ColorAndOpacity.A = Alpha;

		return ColorAndOpacity;
	}

	//Indicator를 보여줌.
	EVisibility GetLoadIndicatorVisibility() const
	{
		bool IsLoadingFinished = GetMoviePlayer()->IsLoadingFinished();
		return IsLoadingFinished ? EVisibility::Collapsed : EVisibility::Visible;
	}
};

class FOpenWorldRPGLoadingScreenModule : public IOpenWorldRPGLoadingScreenModule
{
public:
	virtual void StartupModule() override
	{
		// Force load for cooker reference
		LoadObject<UObject>(nullptr, TEXT("/Game/UI/IMG/Fix_SAC_LOGO_Black.Fix_SAC_LOGO_Black"));

		if ( IsMoviePlayerEnabled() )
		{
			CreateScreen();
		}
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) override
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = !bPlayUntilStopped;
		LoadingScreen.bWaitForManualStop = bPlayUntilStopped;
		LoadingScreen.bAllowEngineTick = bPlayUntilStopped;
		LoadingScreen.MinimumLoadingScreenDisplayTime = PlayTime;
		LoadingScreen.WidgetLoadingScreen = SNew(SOpenWorldRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

	virtual void StopInGameLoadingScreen() override
	{
		GetMoviePlayer()->StopMovie();
	}

	virtual void CreateScreen()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 3.f;
		LoadingScreen.WidgetLoadingScreen = SNew(SOpenWorldRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
};

IMPLEMENT_GAME_MODULE(FOpenWorldRPGLoadingScreenModule, OpenWorldRPGLoadingScreen);

#undef LOCTEXT_NAMESPACE

*/












#include "OpenWorldRPGLoadingScreen/Public/OpenWorldRPGLoadingScreen.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "Widgets/Images/SThrobber.h"

#define LOCTEXT_NAMESPACE "LoadingScreen"

// This module must be loaded "PreLoadingScreen" in the .uproject file, otherwise it will not hook in time!
struct FOpenWorldRPGLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FOpenWorldRPGLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (UObject* CachedResourceObject = GetResourceObject())
		{
			Collector.AddReferencedObject(CachedResourceObject);
		}
	}
};

class SOpenWorldRPGLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOpenWorldRPGLoadingScreen) {}
	//SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)
	SLATE_END_ARGS()

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SOpenWorldRPGLoadingScreen::tick"));
		DeltaTime += InDeltaTime;
	}

	FSlateColor Fliker() const
	{
		const float MinAlpha = 0.1;
		const float MaxAlpha = 1.f;
		const float FlikerAlpha = 1.5f;
		float FlikerPercent = 0.f;	
		
		//UE_LOG(LogTemp, Warning, TEXT("SOpenWorldRPGLoadingScreen::Flicker / Delta : %f"), DeltaTime/*Delta*/);
		//Sin의 -1~1 변화주기 값을 Abs로 0~1값만 가져오도록함.
		FlikerPercent = FMath::Abs(FMath::Sin(DeltaTime *FlikerAlpha));
		const float FlikerAlphaValue = FMath::Lerp(MinAlpha, MaxAlpha, FlikerPercent);

		return FLinearColor(1.f, 1.f, 1.f, FlikerAlphaValue);
	}

	void Construct(const FArguments& InArgs)
	{
		// Load version of the logo with text baked in, path is hardcoded because this loads very early in startup
		static const FName LoadingScreenName(TEXT("/Game/UI/IMG/Fix_SAC_LOGO_Black.Fix_SAC_LOGO_Black"));

		LoadingScreenBrush = MakeShareable(new FOpenWorldRPGLoadingScreenBrush(LoadingScreenName, FVector2D(500, 500)));
		
		FSlateBrush *BGBrush = new FSlateBrush();
		BGBrush->TintColor = FLinearColor(0.005f, 0.005f, 0.005f, 1.0f);

		FSlateFontInfo MyFont = FCoreStyle::Get().GetFontStyle("EmbossedText"); //Roboto
		MyFont.Size = 40.f;
		
		//ULocalPlayer* PlayerOwner = InArgs._PlayerOwner;

		ChildSlot
			[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)	
				.BorderImage(BGBrush)
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SOpenWorldRPGLoadingScreen::Fliker)
				.Image(LoadingScreenBrush.Get())
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(FMargin(1.f))
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::White)
					//.ShadowColorAndOpacity(FLinearColor::White)
					//.ShadowOffset(FIntPoint(-2, 2))
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 40))
					//.Font(MyFont)
					.Text(LOCTEXT("LoadingText", "Loading ..."))//FText::FromString("Loading..."))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(FMargin(2.0f))
				[
					SNew(SCircularThrobber)//SThrobber)
					.Visibility(this, &SOpenWorldRPGLoadingScreen::GetLoadIndicatorVisibility)
				]
			]
		];
	}

private:
	//** Rather to show the ... indicator
	EVisibility GetLoadIndicatorVisibility() const
	{
		bool Vis =  GetMoviePlayer()->IsLoadingFinished();
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}
	
	//** Loading screen image brush 
	TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
	float DeltaTime = 0.f;
	//TWeakObjectPtr<class ULocalPlayer> PlayerOwner;
	//FSlateColor Fliker() const;
	//FSlateFontInfo MyFont = FCoreStyle::Get().GetFontStyle("EmbossedText");
	//MyFont.Size = 40.f;
};

class FOpenWorldRPGLoadingScreenModule : public IOpenWorldRPGLoadingScreenModule
{
public:
	virtual void StartupModule() override
	{
		// Force load for cooker reference
		LoadObject<UObject>(nullptr, TEXT("/Game/UI/IMG/Fix_SAC_LOGO_Black.Fix_SAC_LOGO_Black") );

		if (IsMoviePlayerEnabled())
		{
			CreateScreen();
		}
	}
	
	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) override
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = !bPlayUntilStopped;
		LoadingScreen.bWaitForManualStop = bPlayUntilStopped;
		LoadingScreen.bAllowEngineTick = bPlayUntilStopped;
		LoadingScreen.MinimumLoadingScreenDisplayTime = PlayTime;
		LoadingScreen.WidgetLoadingScreen = SNew(SOpenWorldRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

	virtual void StopInGameLoadingScreen() override
	{
		GetMoviePlayer()->StopMovie();
	}

	virtual void CreateScreen()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 3.f;
		LoadingScreen.WidgetLoadingScreen = SNew(SOpenWorldRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

};

IMPLEMENT_GAME_MODULE(FOpenWorldRPGLoadingScreenModule, OpenWorldRPGLoadingScreen);

#undef LOCTEXT_NAMESPACE

