// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassWidget.h"
#include "OpenWorldRPG/NorthPoint.h"
//#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

#include "Components/Image.h"

#include "Kismet/GameplayStatics.h"

//NorthPointActor를 가져와 NPLocation을 저장한다.
//완료 됐다면 true를 리턴한다.
void UCompassWidget::PrepareCompassUpdate()
{
	UWorld* world = GetWorld();
	if (world)
	{
		TSubclassOf<ANorthPoint> FindNPActor = TSubclassOf<ANorthPoint>(ANorthPoint::StaticClass());
		if(FindNPActor == nullptr) return;

		AActor* NPActor = UGameplayStatics::GetActorOfClass(world, FindNPActor);
		NPLocation = NPActor->GetActorLocation();
		//return true;
	}

	//return false;
}

void UCompassWidget::UpdateCompassPointer()
{
	UE_LOG(LogTemp,Warning,TEXT("UCompassWidget::UpdateCompassPointer"));
	float NorthAngle = 0.f;

	AMainController* PlayerCon = Cast<AMainController>(GetWorld()->GetFirstPlayerController());


	if (PlayerCon && NPLocation != FVector(0.f))
	{
		FRotator NewCompassRotator;

		FRotator ViewportRotator;
		FVector ViewportLocation;
		PlayerCon->GetPlayerViewPoint(ViewportLocation, ViewportRotator);

		FRotator NorthDirection = (NPLocation - ViewportLocation).GetSafeNormal().Rotation();

		float ConYaw = ViewportRotator.Yaw; //OwnerActor->GetActorRotation().Yaw;

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

		ComPointer->SetRenderTransformAngle(AdjustYaw);
	}

	
}