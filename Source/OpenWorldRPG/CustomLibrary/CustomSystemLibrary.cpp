// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/CustomLibrary/CustomSystemLibrary.h"
#include "OpenWorldRPG/Mission/MissionInterface.h"
#include "OpenWorldRPG/Mission/MissionVolume.h"
#include "OpenWorldRPG/CommonImpactEffect.h"


#include "Kismet/GameplayStatics.h"


void UCustomSystemLibrary::CustomProjectionWorldToScreen(APlayerController* const Controller, const FVector& WorldLocation, FVector2D& ScreenPos, bool& bIsBehind)
{
	ULocalPlayer* const LP = Controller ? Controller->GetLocalPlayer() : nullptr;
	if (!LP) return;

	FSceneViewProjectionData ProjectionData;

	if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
	{
		FMatrix const ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
		const FIntRect& ViewRect = ProjectionData.GetConstrainedViewRect();
		FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldLocation, 1.f));


		if (Result.W == 0.f)
		{
			Result.W = 1.f;
		}

		//Result.W will negative value when Target is behind the camera.
		if (Result.W < 0.f)
		{
			bIsBehind = true;
			//ScreenPos = FVector2D::ZeroVector;
		}

		//else if (Result.W > 0.0f)
		{
			// the result of this will be x and y coords in -1..1 projection space
			const float RHW = 1.0f / Result.W;
			FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

			// Move from projection space to normalized 0..1 UI space
			const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
			const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

			FVector2D RayStartViewRectSpace(
				(NormalizedX * (float)ViewRect.Width()),
				(NormalizedY * (float)ViewRect.Height())
			);

			ScreenPos = RayStartViewRectSpace + FVector2D(static_cast<float>(ViewRect.Min.X), static_cast<float>(ViewRect.Min.Y));
		}
	}
}

void UCustomSystemLibrary::AdjustProjectionScreenPosition(FVector2D& ScreenPos, const FVector2D& ScreenSize, const FVector2D& WidgetSize, const bool& bIsBehind)
{
	FVector2D RenderMAX = ScreenSize;


	/* prevent rendering outside of viewport. view port 밖으로 랜더링 되는걸 막기 위함.*/
	RenderMAX.X += -(int32)WidgetSize.X / 2;
	RenderMAX.Y += -(int32)WidgetSize.Y / 2;

	//UE_LOG(LogTemp, Warning, TEXT("UCustomSystemLibrary:: widget Screen pos : %s"), *ScreenPos.ToString());

	if (bIsBehind)
	{
		ScreenPos.X = FMath::Clamp(ScreenPos.X, WidgetSize.X / 2, RenderMAX.X);


		//스크린의 Y축을 기준으로 중간값보다 크면(중간아래면) 최대값으로, 반대면 최소값으로 지정한다.
		ScreenPos.Y = (RenderMAX.Y / 2.f >= ScreenPos.Y) ? RenderMAX.Y : WidgetSize.Y / 2;

		//Actor가 뒤에 있다면, X축을 뒤바꾸기 위함. 이게 뭔가 상식적으로 올바름.

		/* 여기 ScreexX가 이미 alignment를 하기위해 Widgetsize.X/2 만큼 빼둔거라
		여기서 adjust하지 않고 그대로 진행하면 절반에 절반이 더 빠진상태로 비게됨
		그래서 다시 adjust한걸 원복하고 하기로함. */

		//ScreenPos.X = ScreenX - ScreenPos.X;
		ScreenPos.X = (RenderMAX.X + WidgetSize.X / 2) - ScreenPos.X;

		//Screen 중앙을 기준으로 왼쪽에 있다면 0~half까지 clamp를한다.
		if (ScreenPos.X <= RenderMAX.X / 2)
		{
			ScreenPos.X = FMath::Clamp(ScreenPos.X, WidgetSize.X / 2, RenderMAX.X / 2);
		}
		//오른쪽 이라면 half~max 범위로 clamp한다.
		else
		{
			ScreenPos.X = FMath::Clamp(ScreenPos.X, RenderMAX.X / 2, RenderMAX.X);
		}

	}
	else
	{
		//최소값도 widget의 size만큼 주자.
		ScreenPos.X = FMath::Clamp(ScreenPos.X, WidgetSize.X / 2.f, RenderMAX.X);
		ScreenPos.Y = FMath::Clamp(ScreenPos.Y, WidgetSize.Y / 2.f, RenderMAX.Y);
		//UE_LOG(LogTemp, Warning, TEXT("widget Screen pos CLAMP: %s"), *ScreenPos.ToString());
	}

}


void UCustomSystemLibrary::ToggleDisplayHintsWidget(UWorld* World, bool bIsShow, TSubclassOf<AActor>MissionObj, FString MissionUniqueID, FString CharacterUniqueID, EMissionActionType ActionType)
{
	if(!World) return;

	TArray<AActor*> OutActors;

	if(ActionType != EMissionActionType::EMAT_MAX)
	{
		switch (ActionType)
		{
		case EMissionActionType::EMAT_Reach:
			UGameplayStatics::GetAllActorsOfClass(World, AMissionVolume::StaticClass(), OutActors);
			break;
		case EMissionActionType::EMAT_Kill:
		case EMissionActionType::EMAT_Interact:
		case EMissionActionType::EMAT_Collect:
			UGameplayStatics::GetAllActorsOfClass(World, MissionObj, OutActors);
			break;
		}
	}
	//ActionType이 기본값인 MAX값으로 온다면, choice미션이다.
	else
	{
		UGameplayStatics::GetAllActorsOfClass(World, MissionObj, OutActors);
	}
	



	if (OutActors.Num() > 0)
	{
		for (auto Obj : OutActors)
		{
			if (IMissionInterface* MissionI = Cast<IMissionInterface>(Obj))
			{
				if (CharacterUniqueID.IsEmpty() == false)
				{
					if (MissionI->CharacterUniqueID_Interface.Equals(CharacterUniqueID))
					{
						if(bIsShow)
						{
							MissionI->ShowNotifyIcon();
						}
						else
						{
							MissionI->HideNotifyIcon();
						}
					}
				}
				else if (MissionI->MissionUniqueID_Interface.Equals(MissionUniqueID))
				{
					if (bIsShow)
					{
						MissionI->ShowNotifyIcon();
					}
					else
					{
						MissionI->HideNotifyIcon();
					}
					
				}
			}
		}
	}
}


AActor* UCustomSystemLibrary::SpawnAIChar(UWorld* World, struct FEnemyDataTable SpawnAIData)
{
	AEnemyCharacter* SpawnAI = nullptr;
	if(World)
	{
		SpawnAI = Cast<AEnemyCharacter>(World->SpawnActor<AEnemyCharacter>(AEnemyCharacter::StaticClass()));

		if (SpawnAI)
		{
			SpawnAI->SetAIData(SpawnAIData);
			//SpawnAI->ItemSetting = FItemSetting(ItemDA, 1, 0);
			//SpawnAI->SetMesh();
			//SpawnAI->SetItemState(EItemState::EIS_Spawn);
			return SpawnAI;
		}
	}	

	return SpawnAI;
}

void UCustomSystemLibrary::GetChildNativeClass(UClass* BaseClass, TArray<UClass*> ChildClass)
{
	if (BaseClass)
	{
		ChildClass.Empty();

		for (TObjectIterator<UClass> Iter; Iter; ++Iter)
		{
			if (!Iter->IsNative())
			{
				continue;
			}

			if (Iter->HasAnyClassFlags(EClassFlags::CLASS_Abstract))
			{
				continue;
			}

			if (Iter->IsChildOf(BaseClass))
			{
				ChildClass.Add(*Iter);
			}

		}

		/*
		for (const UObject* Obj : TObjectRange<UObject>())
		{
			if (UClass* Cla = Obj->GetClass())
			{
				if (Cla->IsChildOf(AGrenadeBase::StaticClass()))
				{
					ChildClass.Add(Cla);
				}
			}
		}*/
	}
}


void UCustomSystemLibrary::SpawnImpactEffect_Delayed(UWorld* World, FHitResult HitResult, FVector HitForce, UDataTable* DataTable, FName FineTableRowName, FTransform SpawnTransform)
{
	if(!World || !DataTable) return;

	FString DataTableName = DataTable->GetName();
	FCommonImpactEffectTable* EffectData = DataTable->FindRow<FCommonImpactEffectTable>(FineTableRowName, DataTableName);
	ACommonImpactEffect* EffectActor = World->SpawnActorDeferred<ACommonImpactEffect>(ACommonImpactEffect::StaticClass(), SpawnTransform);

	if(!EffectActor || !EffectData) return;
	EffectActor->SetImpactEffect(*EffectData, HitResult, HitForce, SpawnTransform);
	//EffectActor->SetActorTransform(SpawnTransform);
	UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);

	
}

UNiagaraSystem* UCustomSystemLibrary::GetImpactFX(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType)
{
	UNiagaraSystem* ReturnEffect = nullptr;

	switch (MaterialType)
	{
	case PHYSICS_SURFACE_CONCRETE:
		ReturnEffect = EffectData.ConcreteFX;
		break;
	case PHYSICS_SURFACE_METAL:
		ReturnEffect = EffectData.MetalFX;
		break;
	case PHYSICS_SURFACE_SOIL:
		ReturnEffect = EffectData.SoilFX;
		break;
	case PHYSICS_SURFACE_DIRT:
		ReturnEffect = EffectData.DirtFX;
		break;
	case PHYSICS_SURFACE_GRASS:
		ReturnEffect = EffectData.GrassFX;
		break;
	case PHYSICS_SURFACE_GLASS:
		ReturnEffect = EffectData.GlassFX;
		break;
	case PHYSICS_SURFACE_TILE:
		ReturnEffect = EffectData.TileFX;
		break;
	case PHYSICS_SURFACE_CARPET:
		ReturnEffect = EffectData.CarpetFX;
		break;
	case PHYSICS_SURFACE_WOOD:
		ReturnEffect = EffectData.WoodFX;
		break;
	case PHYSICS_SURFACE_WATER:
		ReturnEffect = EffectData.WaterFX;
		break;
	case PHYSICS_SURFACE_SCRAP:
		ReturnEffect = EffectData.ScrapFX;
		break;
	case PHYSICS_SURFACE_FLESH:
		ReturnEffect = EffectData.FleshFX;
		break;
	}

	return ReturnEffect ? ReturnEffect : EffectData.DefaultFX;
}

USoundCue* UCustomSystemLibrary::GetImpactSound(FCommonImpactEffectTable EffectData, TEnumAsByte<EPhysicalSurface> MaterialType)
{
	USoundCue* ReturnSound = nullptr;

	switch (MaterialType)
	{
	case PHYSICS_SURFACE_CONCRETE:
		ReturnSound = EffectData.ConcreteSound;
		break;
	case PHYSICS_SURFACE_METAL:
		ReturnSound = EffectData.MetalSound;
		break;
	case PHYSICS_SURFACE_SOIL:
		ReturnSound = EffectData.SoilSound;
		break;
	case PHYSICS_SURFACE_DIRT:
		ReturnSound = EffectData.DirtSound;
		break;
	case PHYSICS_SURFACE_GRASS:
		ReturnSound = EffectData.GrassSound;
		break;
	case PHYSICS_SURFACE_GLASS:
		ReturnSound = EffectData.GlassSound;
		break;
	case PHYSICS_SURFACE_TILE:
		ReturnSound = EffectData.TileSound;
		break;
	case PHYSICS_SURFACE_CARPET:
		ReturnSound = EffectData.CarpetSound;
		break;
	case PHYSICS_SURFACE_WOOD:
		ReturnSound = EffectData.WoodSound;
		break;
	case PHYSICS_SURFACE_WATER:
		ReturnSound = EffectData.WaterSound;
		break;
	case PHYSICS_SURFACE_SCRAP:
		ReturnSound = EffectData.ScrapSound;
		break;
	case PHYSICS_SURFACE_FLESH:
		ReturnSound = EffectData.FleshSound;
		break;
	}

	return ReturnSound ? ReturnSound : EffectData.DefaultSound;
}