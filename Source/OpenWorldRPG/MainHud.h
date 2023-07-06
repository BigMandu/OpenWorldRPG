// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"

#include "MainHud.generated.h"

/**
 * 
 */

 USTRUCT()
struct FHitIndicatorIconData
{
	GENERATED_USTRUCT_BODY()
	UTexture2D* HitTexture;
	FVector2D TopLeft;
	FVector2D BottomRight;
	FVector2D Offset;
	FVector2D RenderScale;

	FHitIndicatorIconData() {
		HitTexture = nullptr;
		TopLeft = FVector2D::ZeroVector;
		BottomRight = FVector2D::ZeroVector;
		Offset = FVector2D::ZeroVector;
		RenderScale = FVector2D::ZeroVector;
	}

	FHitIndicatorIconData(UTexture2D* Var_hitTexture, FVector2D Var_topLeft, FVector2D Var_bottomRight, FVector2D Var_offset, FVector2D Var_RenderScale)
	{
		HitTexture = Var_hitTexture;
		TopLeft = Var_topLeft;
		BottomRight = Var_bottomRight;
		Offset = Var_offset;
		RenderScale = Var_RenderScale;
	}
 };

struct FHitIndicatorData
{
	float LastHitTime;
	float IndicatorStrength;

	FHitIndicatorData()
	{
		LastHitTime = 0.f;
		IndicatorStrength = 0.f;
	}
};

UCLASS()
class OPENWORLDRPG_API UMainHud : public UUserWidget
{
	GENERATED_BODY()
public:
	class AMainController* MainCon;
	
	bool bIsCompassWidgetVisible = false;
	bool bIsInventoryWindowShow = false;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* HudCanvas;

	UPROPERTY(meta = (BindWidget))
	class UNewInventory* NewInventoryWidget;

	UPROPERTY(meta = (BindWidget))
	class UResourceStatusWidget* ResourceWidget;
	UPROPERTY(meta = (BindWidget))
	class UWeaponStatusWidget* WeaponStatus;
	UPROPERTY(meta = (BindWidget))
	class UQuickSlotWidget* QuickSlot;

	UPROPERTY(meta = (BindWidget))
	class UCompassWidget* CompassWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	TSubclassOf<UUserWidget> WInteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	UUserWidget* InteractText;

	//for ZOrder
	int32 HighestZ = 2;


	/* Notify Indicator */
	UPROPERTY()
	UTexture2D* HitIndicatorTexture;
	//Hit indicator가 표시될때 얼마나 유지할지의 시간.
	float HitIndicatorDisplayTime;
	//마지막으로 Hit indicator가 표시된 시간.
	float LastShowingHitIndicatorTime;	
	//8방향에 대한 Indicator의 데이터.
	FHitIndicatorData HitIndicatorData[8];

	UPROPERTY()
	FHitIndicatorIconData HitIndicatorIconData[8];

	/*UPROPERTY()
	struct FCanvasIcon HitByNotifyIcon[8];

	FVector2D NotifyOffset[8];*/

	FTimerHandle HitByNotifyTimer;


private:
	void ShowOtherUI();
	void HideOtherUI();

	void DrawHitByNotify();
	void UnDrawHitByNotify();
protected:
	void SetFocus(UUserWidget* Focus);
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

public:
	UMainHud(const FObjectInitializer& ObjectInitializer);
	//Initialize보다 먼저 실행됨.
	virtual void NativeConstruct() override;

	void ShowInventoryWindow();
	void HideInventoryWindow();

	void ShowInteractText();
	void HideInteractText();

	void ShowCompassWidget();
	void HideCompassWidget();

	void SettingIsCompassWidgetVisible(bool bIsVisible);

	void NotifyHitSomething();
	void NotifyHitbySomething(class AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent);

	

};
