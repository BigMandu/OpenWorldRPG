// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomSystemLibrary.generated.h"
/**
 * 
 */
 UCLASS()
class OPENWORLDRPG_API UCustomSystemLibrary : public UObject
{
	 GENERATED_BODY()

public:
	/** 
	 * Custom ProjectWorldToScreen functions.
	 * Different between this and origin function is that it returns a value even when WorldLocation is behind the player (camera).
	 * @param Controller		Current player controller.
	 * @param WorldLocation		World Location to project.
	 * @param ScreenPosition	(out) Corresponding 2D position in screen space.
	 * @param bIsBehind			(out) WorldLocation(parameter) is behind Controller.
	 */
	static void CustomProjectionWorldToScreen(class APlayerController* const Controller, const FVector& WorldLocation, FVector2D& ScreenPos, bool& bIsBehind);

	/**
	 * This function used after calling CustomProjectionWorldToScreen.
	 * It adjust the screen position of the widget. (use ScreenPos).
	 * @param ScreenPos		(out) 2D position in screen space. (Pass in CustomProjectionWorldToScreen's ScreenPos).
	 * @param ScreenSize	Viewport size (Use GetViewportsize and create vector2D).
	 * @param WidgetSize	Widget size that want to render at ScreenPos.
	 * @param bIsbehind		Pass CustomProjectionWorldToScreen's bIsbehind.
	 */
	static void AdjustProjectionScreenPosition(FVector2D& ScreenPos, const FVector2D& ScreenSize, const FVector2D& WidgetSize,  const bool& bIsBehind);

	/**
	 * This function determines whether to display or hide a Hint Widget for related mission objects in the world.
	 * based on bool variable.
	 * 
	 * @param World					world.
	 * @param bIsShow				if this true show widget, false Hide widget.
	 * @param MissionObj			mission require object type.
	 * @param MissionUniqueID		mission unique ID.
	 * @param ActionType			if this mission is not a Choice type, you have to pass in mission's Action type.
	 *								if this mission is choice type, don't pass in.
	 */
	static void ToggleDisplayHintsWidget(UWorld* World, bool bIsShow, TSubclassOf<AActor>MissionObj, FString MissionUniqueID, FString CharacterUniqueID, EMissionActionType ActionType = EMissionActionType::EMAT_MAX);

	
};
