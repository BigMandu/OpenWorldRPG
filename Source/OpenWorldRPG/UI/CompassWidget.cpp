// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassWidget.h"
#include "OpenWorldRPG/NorthPoint.h"
//#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

#include "Components/Image.h"

#include "Kismet/GameplayStatics.h"

//NorthPointActor�� ������ NPLocation�� �����Ѵ�.
//�Ϸ� �ƴٸ� true�� �����Ѵ�.
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

		/**ĳ���� ȸ������ NP������ �������� -180~180������ ���� ������ �ϱ� ������
		* 180���϶�� 360�� ���ϰ�, 180�̻��̸� 360�� ���� -180~180������ ���� �����Ѵ�.
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