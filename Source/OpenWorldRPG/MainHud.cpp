// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"
#include "OpenWorldRPG/UI/NotificationWidget.h"
#include "OpenWorldRPG/UI/SubTitleWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"


#define LOCTEXT_NAMESPACE "HUD"

UMainHud::UMainHud(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTexture2D> HitNoteTextureOBJ(TEXT("/Game/UI/HUD/HitIndicator"));
	HitIndicatorTexture = HitNoteTextureOBJ.Object;

	//Texture, TopLeft, BottomRight, Offset
	HitIndicatorIconData[0] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(490,870), FVector2D(615,1170), FVector2D(250, 0), FVector2D(0.8f, 7.f)); //Left
	HitIndicatorIconData[1] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(500,500), FVector2D(800,800), FVector2D(230, 150), FVector2D(2.8f, 3.5f)); //Front Left
	HitIndicatorIconData[2] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(880,480), FVector2D(1185,620), FVector2D(0, 180), FVector2D(2.5f, 2.25f)); //Front
	HitIndicatorIconData[3] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(1245,550), FVector2D(1495,820), FVector2D(-230, 150), FVector2D(2.8f, 3.5f)); //Front Right
	HitIndicatorIconData[4] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(1430,868), FVector2D(1566,1192), FVector2D(-250, 0), FVector2D(0.8f, 7.f)); //Right
	HitIndicatorIconData[5] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(1210,1255), FVector2D(1490,1500), FVector2D(-230, -150), FVector2D(2.8f, 3.5f)); //Rear Right
	HitIndicatorIconData[6] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(875,1430), FVector2D(1170,1570), FVector2D(0, -180), FVector2D(2.5f, 2.25f)); //Rear
	HitIndicatorIconData[7] = FHitIndicatorIconData(HitIndicatorTexture, FVector2D(555,1250), FVector2D(830,1510), FVector2D(230, -150), FVector2D(2.8f, 3.5f)); //Rear Left
	
	//HitByNotifyIcon[0] = UCanvas::MakeIcon(HitIndicatorTexture, 158, 831, 585, 392); //Left
	//HitByNotifyIcon[1] = UCanvas::MakeIcon(HitIndicatorTexture, 369, 434, 460, 378); //FrontLeft
	//HitByNotifyIcon[2] = UCanvas::MakeIcon(HitIndicatorTexture, 848, 284, 361, 395); //Front
	//HitByNotifyIcon[3] = UCanvas::MakeIcon(HitIndicatorTexture, 1212, 397, 427, 394); //FrontRight
	//HitByNotifyIcon[4] = UCanvas::MakeIcon(HitIndicatorTexture, 1350, 844, 547, 321); //Right
	//HitByNotifyIcon[5] = UCanvas::MakeIcon(HitIndicatorTexture, 1232, 1241, 458, 341); //RearRight
	//HitByNotifyIcon[6] = UCanvas::MakeIcon(HitIndicatorTexture, 862, 1384, 353, 408); //Rear
	//HitByNotifyIcon[7] = UCanvas::MakeIcon(HitIndicatorTexture, 454, 1251, 371, 410); //RearLeft

	//NotifyOffset[0] = FVector2D(173, 0);	   //Left
	//NotifyOffset[1] = FVector2D(120, 125);   //FrontLeft
	//NotifyOffset[2] = FVector2D(0, 173);	   //Front
	//NotifyOffset[3] = FVector2D(-120, 125);  //FrontRight
	//NotifyOffset[4] = FVector2D(-173, 0);	   //Right
	//NotifyOffset[5] = FVector2D(-120, -125); //RearRight
	//NotifyOffset[6] = FVector2D(0, -173);	   //Rear
	//NotifyOffset[7] = FVector2D(120, -125);  //RearLeft

	//일단 hit indicator를 0.8초간 표시하기로 한다.
	HitIndicatorDisplayTime = 0.8f;
}

void UMainHud::NativeConstruct()
{
	
	if (WInteractText)
	{
		InteractText = CreateWidget<UUserWidget>(this, WInteractText);
		if (InteractText)
		{
			InteractText->AddToViewport();
			InteractText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if(CompassWidget)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	
}

int32 UMainHud::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 ReturnVal = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	if ( HitIndicatorTexture )
	{
		const float CurrentWorldTime = GetWorld()->GetTimeSeconds();

		if ( CurrentWorldTime - LastShowingHitIndicatorTime <= HitIndicatorDisplayTime )
		{
			FVector2D HudSize = this->GetDesiredSize(); //HudCanvas->GetDesiredSize(); 
			const float HudMidX = HudSize.X / 2;
			const float HudMidY = HudSize.Y / 2;

			for ( int32 iter = 0; iter < 8; ++iter )
			{
				float Ratio = ( CurrentWorldTime - HitIndicatorData[ iter ].LastHitTime ) / HitIndicatorDisplayTime;

				if ( HitIndicatorData[ iter ].IndicatorStrength >= 1.f )
				{
					
					UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
					int32 NotifyTextureX = HitIndicatorIconData[iter].HitTexture->GetSizeX();
					int32 NotifyTextureY = HitIndicatorIconData[iter].HitTexture->GetSizeY();
					//UE_LOG(LogTemp, Warning, TEXT("MainHud::NativePaint // Texture X : %d, Texture Y : %d"), NotifyTextureX, NotifyTextureY);


					//coordinate transform
					const FVector2D NTopLeft = FVector2D(HitIndicatorIconData[iter].TopLeft.X / NotifyTextureX, 1.f - HitIndicatorIconData[iter].TopLeft.Y / NotifyTextureY);//FVector2D(HitByNotifyIcon[ iter ].U, HitByNotifyIcon[ iter ].V);
					const FVector2D NBottomRight = FVector2D(HitIndicatorIconData[iter].BottomRight.X / NotifyTextureX, 1.f - HitIndicatorIconData[iter].BottomRight.Y / NotifyTextureY);//HitIndicatorIconData[iter].BottomRight;
					const FVector2D ImageSize = FMath::Abs(HitIndicatorIconData[iter].BottomRight - HitIndicatorIconData[iter].TopLeft);//FVector2D(HitByNotifyIcon[ iter ].UL, HitByNotifyIcon[ iter ].VL);
					const FVector2D ImageOffset = HitIndicatorIconData[iter].Offset;//NotifyOffset[ iter ];// - ImageSize / 2.f;
										
	
					FSlateBrush Brush;
					Brush.SetResourceObject(HitIndicatorIconData[iter].HitTexture /* HitIndicatorTexture*/);

					FBox2D UVRegion;
					UVRegion.Min = NTopLeft; // FVector2D(HitIndicatorIconData[iter].HitTexture->GetSizeX(), HitIndicatorIconData[iter].HitTexture->GetSizeY());
					UVRegion.Max = NBottomRight; // FVector2D(HitIndicatorIconData[ iter ].HitTexture->GetSizeX(), HitIndicatorIconData[ iter ].HitTexture->GetSizeY());//FVector2D(HitIndicatorTexture->GetSizeX(), HitIndicatorTexture->GetSizeY());
					Brush.SetUVRegion(UVRegion);

					Image->SetBrush(Brush);
					Image->SetRenderScale(ImageSize * 0.01);

					//Left & Right
					//if ( iter == 0 || iter == 4 )
					//{
					//	Image->SetRenderScale(FVector2D(.8f,7.f));
					//}
					////front& rear
					//else if ( iter == 2 || iter == 6 )
					//{
					//	Image->SetRenderScale(FVector2D(2.5f, 2.25f));
					//}
					////나머지 대각
					//else
					//{
					//	Image->SetRenderScale(FVector2D(2.8f, 3.5f));
					//}
					
					//위에 주석하면 이상하게됨;...뭐지..그래서 RenderScale을 못쓴다..
					Image->SetRenderScale(HitIndicatorIconData[iter].RenderScale);

					Image->SetRenderTranslation(FVector2D(HudMidX - ImageOffset.X, HudMidY - ImageOffset.Y));
					Image->SetVisibility(ESlateVisibility::Visible);

					HudCanvas->AddChild(Image);
					
					//for debug 일단, 잘됨.
					/*FString dirstr;

					switch ( iter )
					{
						case 0:
							dirstr = "Left";
						break;
						case 1:
							dirstr = "Fr.Le";
							break;
						case 2:
							dirstr = "Front";
							break;
						case 3:
							dirstr = "Fr.Ri";
							break;
						case 4:
							dirstr = "Right";
							break;
						case 5:
							dirstr = "Re.Ri";
							break;
						case 6:
							dirstr = "Rear";
							break;
						case 7:
							dirstr = "Re.Le";
							break;						
					}

					UE_LOG(LogTemp,Warning,TEXT("UMainHUD::NativePaint // Hit by indicator : %d"), iter);
					UE_LOG(LogTemp,Warning,TEXT("UMainHUD::NativePaint // Hit by indicator : %s"), *dirstr);*/
					
				}
			}

		}
		else
		{
			for ( auto Content : HudCanvas->GetAllChildren() )
			{
				if ( UImage* Img = Cast<UImage>(Content) )
				{
					const FSlateBrush& VarBrush = Img->Brush;
					if ( UTexture2D* BrushTexture = Cast<UTexture2D>(VarBrush.GetResourceObject()) )
					{
						if ( BrushTexture == HitIndicatorTexture )
						{
							HudCanvas->RemoveChild(Img);
							Img->RemoveFromParent();
							Img->ConditionalBeginDestroy();
						}
					}
				}
			}
		}
	}
	return ReturnVal;
}


/*뭔가를 때렸을 때 */
void UMainHud::NotifyHitSomething()
{


}

/* 누군가로 부터 맞았을 때 */
void UMainHud::NotifyHitbySomething(AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent)
{
	if ( this->GetOwningPlayer() != DamageReceiver ) return;

	if ( !DamageGiver ) return;

	GetWorld()->GetTimerManager().ClearTimer(HitByNotifyTimer);

	//월드에서 흐른 초를 구한다.
	float CurrentTime = GetWorld()->GetTimeSeconds();
	AActor* Owner = this->GetOwningPlayerPawn();

	//월드 경과 시간에서 마지막 표시된 indicator시간을 뺀 시간이. HID표시될 시간보다 크다면, 지워줘야함.
	if ( CurrentTime - LastShowingHitIndicatorTime > HitIndicatorDisplayTime )
	{
		for ( int32 iter = 0; iter < 8; ++iter )
		{
			//HID의 세기를 0으로 하여 지운다.
			HitIndicatorData[ iter ].IndicatorStrength = 0.f;
		}
	}



	FHitResult HitResult;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(Owner, DamageGiver->GetPawn(), HitResult, ImpulseDir);

	//Player 기준으로 회전 매트릭스를 만들어 로컬좌표계를 나타내도록하고
	//InversTransfrom을 이용해 world(impulsedir)를 local좌표로 변환한다.
	//impulseDir을 반대로 바꿔야 한다. 맞은쪽을 가리켜야 하기 때문이다.
	FVector AttackDir = FRotationMatrix(DamageReceiver->GetControlRotation()).InverseTransformVector(-ImpulseDir);

	if ( ImpulseDir != FVector::ZeroVector )
	{
		//Dir Array를 생성한다.
		FVector Dir[ 8 ] = {
			//x, y, z
			//왼쪽부터 시계방향으로 돈다 (앞 전방방향이 자주 피격 되니까)
			FVector(0, -1, 0), //Left
			FVector(1, -1, 0), //front left
			FVector(1, 0, 0), //front
			FVector(1, 1, 0), //front right
			FVector(0, 1, 0), //right
			FVector(-1, 1, 0), //back right
			FVector(-1, 0, 0), //back
			FVector(-1, -1, 0), //back left
		};

		int32 DirIndex = -1;
		float HighDirMod = 0.f;

		for ( int32 iter = 0; iter < 8; ++iter )
		{
			Dir[ iter ].Normalize();
			//내적 벡터한 결과값을 0과 비교한다. 즉,Dot결과가 수직인 경우는 버린다는 뜻.
			float DirMod = FMath::Max(0.f, FVector::DotProduct(Dir[ iter ], AttackDir));

			//가장 1에 가까운(평행 관계) Dirmod를 구한다.
			if ( DirMod > HighDirMod )
			{
				DirIndex = iter;
				HighDirMod = DirMod;
			}
		}

		//Dirindex를 구했다면 표시하는 그걸 띄울 수 있도록 정보를 어떻게 좀 해보자.
		if ( DirIndex > -1 )
		{
			HitIndicatorData[ DirIndex ].IndicatorStrength = 1.f;
			HitIndicatorData[ DirIndex ].LastHitTime = CurrentTime;
		}
	}

	LastShowingHitIndicatorTime = CurrentTime;

	//GetWorld()->GetTimerManager().SetTimer(HitByNotifyTimer,this, &UMainHud::DrawHitByNotify, GetWorld()->GetDeltaSeconds(),true);
}

void UMainHud::ShowInventoryWindow()
{
	if (NewInventoryWidget)
	{
		//Inventory를 열면 표시된 widget을 hide한다.
		HideOtherUI();

		FInputModeGameAndUI Mode; ///TabKey로 HIde를 하기위해 Game And UI mode로 했는데도 키가 안먹힌다. 
		Mode.SetWidgetToFocus(NewInventoryWidget->TakeWidget());
		MainCon->SetInputMode(Mode);

		bIsInventoryWindowShow = true;
		NewInventoryWidget->ChangeRightSwitcher();
		NewInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHud::HideInventoryWindow()
{
	if (NewInventoryWidget)
	{
		//Inventory를 닫으면 표시해야할 widget을 show한다.
		ShowOtherUI();
		//닫을때 마다 HighestZ를 2로 초기화 한다.
		NewInventoryWidget->HighestZ = 2;
		
		NewInventoryWidget->CloseAllAdditionalWidget(NewInventoryWidget->OpenedWindowArray.Num());
		bIsInventoryWindowShow = false;		
		NewInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);				
	}
}

void UMainHud::ShowOtherUI()
{
	ResourceWidget->SetVisibility(ESlateVisibility::Visible);
	WeaponStatus->SetVisibility(ESlateVisibility::Visible);

	if (bIsCompassWidgetVisible)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Visible);
	}
	//QuickSlot->SetVisibility(ESlateVisibility::Visible);
}

void UMainHud::HideOtherUI()
{
	ResourceWidget->SetVisibility(ESlateVisibility::Collapsed);
	WeaponStatus->SetVisibility(ESlateVisibility::Collapsed);
	//QuickSlot->SetVisibility(ESlateVisibility::Collapsed);

	if (bIsCompassWidgetVisible)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}



void UMainHud::ShowInteractText()
{
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHud::HideInteractText()
{
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// Show/Hide OtherUI function에서 사용한다.
void UMainHud::SettingIsCompassWidgetVisible(bool bIsVisible)
{
	bIsCompassWidgetVisible = bIsVisible;
}


void UMainHud::SetNotificationMessage(ESystemNotificationMessageType NotiMsgType)
{
	SystemNotificationWidget->AddNotificationMessage(NotiMsgType);
	/* Noti Audio */
}

void UMainHud::SetNotificationMessage(EGameProgressNotificationMessageType NotiMsgType)
{
	GameProgressNotificationWidget->AddNotificationMessage(NotiMsgType);
	/* noti audio */

}

void UMainHud::SetSubtitleText(FText vName, FText vText, bool bBlueTeam)
{
	SubTitleWidget->SetSub(vName,vText, bBlueTeam);
}

void UMainHud::RemoveSubtitleText()
{
	SubTitleWidget->RemoveSub();
}

void UMainHud::DrawHitByNotify()
{
	//NativePaint함수로 대체함.
}

//필요없어짐.
void UMainHud::UnDrawHitByNotify()
{
	GetWorld()->GetTimerManager().ClearTimer(HitByNotifyTimer);
}


#undef NSLOCTEXT_NAMESPACE
/*

바로 위 함수에서 한걸, 아래 함수가 표시하는거임.해보자.

void AShooterHUD::DrawHitIndicator()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if ( CurrentTime - LastHitTime <= HitNotifyDisplayTime )
	{
		const float StartX = Canvas->ClipX / 2.0f;
		const float StartY = Canvas->ClipY / 2.0f;

		for ( uint8 i = 0; i < 8; i++ )
		{
			const float TimeModifier = FMath::Max(0.0f, 1 - ( CurrentTime - HitNotifyData[ i ].HitTime ) / HitNotifyDisplayTime);
			const float Alpha = TimeModifier * HitNotifyData[ i ].HitPercentage;
			Canvas->SetDrawColor(255, 255, 255, FMath::Clamp(FMath::TruncToInt(Alpha * 255 * 1.5f), 0, 255));
			Canvas->DrawIcon(HitNotifyIcon[ i ],
				StartX + ( HitNotifyIcon[ i ].U - HitNotifyTexture->GetSizeX() / 2 + Offsets[ i ].X ) * ScaleUI,
				StartY + ( HitNotifyIcon[ i ].V - HitNotifyTexture->GetSizeY() / 2 + Offsets[ i ].Y ) * ScaleUI,
				ScaleUI);
		}
	}
}
*/