// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "OpenWorldRPG/GameData/CommonImpactEffectTable.h"
#include "CustomSystemLibrary.generated.h"
/**
 * 
 */

 /** If you add another Physics material, you have to add
  * additional code in [ %ProjectDirectory%\Config\DefaultEngine.ini ]
  * below [/Script/Engine.PhysicsSettings] section.
  */
 #define PHYSICS_SURFACE_CONCRETE	SurfaceType1
 #define PHYSICS_SURFACE_METAL		SurfaceType2
 #define PHYSICS_SURFACE_SOIL		SurfaceType3
 #define PHYSICS_SURFACE_DIRT		SurfaceType4
 #define PHYSICS_SURFACE_GRASS		SurfaceType5
 #define PHYSICS_SURFACE_GLASS		SurfaceType6
 #define PHYSICS_SURFACE_TILE		SurfaceType7
 #define PHYSICS_SURFACE_CARPET		SurfaceType8
 #define PHYSICS_SURFACE_WOOD		SurfaceType9
 #define PHYSICS_SURFACE_WATER		SurfaceType10
 #define PHYSICS_SURFACE_SCRAP		SurfaceType11
 #define PHYSICS_SURFACE_FLESH		SurfaceType12


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


	static AActor* SpawnAIChar(UWorld* World, struct FEnemyDataTable SpawnAIData);

	static void GetChildNativeClass(UClass* BaseClass, TArray<UClass*> ChildClass);


	static void SpawnImpactEffect_Delayed(UWorld* World, FHitResult HitResult, FVector HitForce, UDataTable* DataTable, FName FineTableRowName, FTransform SpawnTransform);

	static class UNiagaraSystem* GetImpactFX(struct FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType);
	static class USoundCue* GetImpactSound(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType);
};
