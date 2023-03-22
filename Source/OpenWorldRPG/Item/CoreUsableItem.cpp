// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreUsableItem.h"
#include "OpenWorldRPG/NorthPoint.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ACoreUsableItem::ACoreUsableItem()
{
	OptionalStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OptionalStaticMesh"));
	OptionalStaticMesh->SetupAttachment(GetRootComponent());

	OptionalStaticMesh->SetSimulatePhysics(false);
	Mesh->SetSimulatePhysics(false);
}
void ACoreUsableItem::SetMesh()
{
	Super::SetMesh();

	if (ItemSetting.DataAsset->Mesh_Optional)
	{
		OptionalStaticMesh->SetStaticMesh(ItemSetting.DataAsset->Mesh_Optional);
	}

}

void ACoreUsableItem::AttachToHand(class ABaseCharacter* Actor, class UNewItemObject* Obj, bool bIsNeedToDestory)
{
	if (OptionalStaticMesh->GetStaticMesh())
	{
		OptionalStaticMesh->SetSimulatePhysics(false);
	}
	Super::AttachToHand(Actor,Obj, bIsNeedToDestory);
}


void ACoreUsableItem::Use(ABaseCharacter* Actor, UNewItemObject* Obj)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Actor);
	if(Player == nullptr) return;

	if (Obj->bIsUsing)
	{
		ToggleOff(Player, Obj);
	}
	else
	{
		ToggleOn(Player, Obj);		
	}
}


void ACoreUsableItem::ToggleOn(AMainCharacter* Actor, UNewItemObject* Obj)
{
	UWorld* World = GetWorld();

	if (World == nullptr) return;

	TSubclassOf<ANorthPoint> NorthPoint = TSubclassOf<ANorthPoint>(ANorthPoint::StaticClass());
	ANorthPoint* NP = Cast<ANorthPoint>(UGameplayStatics::GetActorOfClass(World, NorthPoint));
	if (NP == nullptr) return;

	Obj->bIsUsing = true;
	//BaseCharacter의 PlayUseItemAnim func 호출
	Actor->PlayUseItemAnim(this);
	
	Actor->CompassBeginUse.Broadcast();

	NPLocation = NP->GetActorLocation();	

	GetWorldTimerManager().SetTimer(CompassHeldTimer, this, &ACoreUsableItem::UpdateCompassPointerRotation, World->GetDeltaSeconds(), true);
}

void ACoreUsableItem::ToggleOff(AMainCharacter* Actor, UNewItemObject* Obj)
{
	if(Actor == nullptr) return;

	Obj->bIsUsing = false;
	ItemUseEnd.Broadcast();	
	Actor->CompassEndUse.Broadcast();

	DetachFromHand(Actor, true);
	Destroy();
}

void ACoreUsableItem::UpdateCompassPointerRotation()
{ 
	//AActor* OwnerActor = GetWorld()->GetFirstPlayerController()->GetCharacter();
	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());


	if (OptionalStaticMesh->GetStaticMesh() &&  PlayerCon && NPLocation != FVector(0.f))
	{
		FRotator NewCompassRotator;

		FRotator ViewportRotator;
		FVector ViewportLocation;
		PlayerCon->GetPlayerViewPoint(ViewportLocation,ViewportRotator);

		float ConYaw = ViewportRotator.Yaw;
		FRotator NorthDirection = (NPLocation - ViewportLocation).GetSafeNormal().Rotation();
		

		/**캐릭터 회전값과 NP사이의 각도계산시 -180~180사이의 값을 가져야 하기 때문에
		* 180이하라면 360을 더하고, 180이상이면 360을 빼서 -180~180사이의 값을 유지한다.
		*/
		float AdjustYaw = NorthDirection.Yaw - ConYaw;
		if (AdjustYaw < 180.f)
		{
			AdjustYaw += 360.f;
		}
		else if (AdjustYaw > 180.f)
		{
			AdjustYaw -= 360.f;
		}

		NewCompassRotator = FRotator(0.f, AdjustYaw, 0.f);

		OptionalStaticMesh->SetRelativeRotation(NewCompassRotator);
	}
}