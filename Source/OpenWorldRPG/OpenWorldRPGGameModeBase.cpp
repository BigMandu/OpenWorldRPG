// Copyright Epic Games, Inc. All Rights Reserved.


#include "OpenWorldRPGGameModeBase.h"

#include "Kismet/GameplayStatics.h"

#include "OpenWorldRPG/SpawnVolume.h"
#include "OpenWorldRPG/Save/SaveWorld.h"
#include "OpenWorldRPG/GameData/MyGameInstance.h"


//Blueprint'/Game/Blueprints/DamageType/DmgType_Falling.DmgType_Falling'
AOpenWorldRPGGameModeBase::AOpenWorldRPGGameModeBase()
{
	FString DmgtypePath = TEXT("/Game/Blueprints/DamageType/DmgType_Falling");
	ConstructorHelpers::FClassFinder<UMyDamageType>FallingDmgTypeOBJ(*DmgtypePath);

	if ( FallingDmgTypeOBJ.Succeeded() )
	{
		FallingDmgType = FallingDmgTypeOBJ.Class;
	}
}

void AOpenWorldRPGGameModeBase::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogTemp,Warning,TEXT("AOpenWorldRPGGameModeBase::StartPlay"));

	FInputModeGameOnly Inputmode;
	GetWorld()->GetFirstPlayerController()->SetInputMode(Inputmode);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	UMyGameInstance* Inst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if ( Inst )
	{
		//Inst->Hide Loading Scren()
		Inst->StopLoadingScreen();		
	}
}



void AOpenWorldRPGGameModeBase::SaveWorldStatus()
{
	FString SlotName = TEXT("SaveWorld");
	int32 Index = 0;

	if ( UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false )
	{
		SaveGame_World = Cast<USaveWorld>(UGameplayStatics::CreateSaveGameObject(USaveWorld::StaticClass()));
	}
	else
	{
		SaveGame_World = Cast<USaveWorld>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));
	}


	if ( SaveGame_World == nullptr ) return;

	//Save시에 그냥 통으로 날리고 새로 저장하도록 한다.
	SaveGame_World->SavedSpawnVolStatusMap.Empty();


	//World에 배치된 Spawn Vol을 전부 구한다.
	TArray<AActor*> WorldVolumeList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), WorldVolumeList);	

	/*Spawnvolume을 순회하면서
	* spawnVolume의 DestroyCount와 Spawnvol자체를 Map에 저장한다
	* */
	for ( auto GetActor : WorldVolumeList )
	{
		if ( GetActor == nullptr ) continue;
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(GetActor);
		if ( SpawnVolume )
		{
			UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::SaveWorldStatus// %s status saved "), *SpawnVolume->GetFName().ToString());
			//SaveGame_World->SavedSpawnVolume.Add(SpawnVolume);
			SaveGame_World->SavedSpawnVolStatusMap.Add(SpawnVolume, SpawnVolume->DestoryCount);
		}
	}

	UGameplayStatics::SaveGameToSlot(SaveGame_World, SlotName, Index);
}

void AOpenWorldRPGGameModeBase::LoadWorldStatus()
{
	FString SlotName = TEXT("SaveWorld");
	int32 Index = 0;

	if ( UGameplayStatics::DoesSaveGameExist(SlotName, Index) == false )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::SaveWorldStatus // Load Fail!"));
	}

	USaveWorld* LoadGame_World = Cast<USaveWorld>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));

	if ( LoadGame_World == nullptr ) return;

	TArray<AActor*> WorldVolumeList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), WorldVolumeList);

	/* save와 비슷하다.
	* 우선 world에 배치된 spawnvol을 순회하면서
	* loadGame에 있는 Map에서 spawnvol을 비교하며 일치하면 destroycnt를 그대로 대입시킨다.
	*/
	for ( auto GetActor : WorldVolumeList )
	{
		if ( !GetActor ) continue;
		ASpawnVolume* SpawnVol = Cast<ASpawnVolume>(GetActor);
		if ( SpawnVol )
		{
			int32* DestoryCnt = LoadGame_World->SavedSpawnVolStatusMap.Find(SpawnVol);
			SpawnVol->DestoryCount = *DestoryCnt;
			UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::LoadWorldStatus //VolName : %s , DestCnt : %d"), *SpawnVol->GetFName().ToString(), *DestoryCnt);
		}
	}

}




float AOpenWorldRPGGameModeBase::ModifyApplyDamage(float BeforeDamage, struct FDamageEvent const& DamageEvent, class AActor* DamagedVictim, class AController* EventInstigator, AActor* DamageCauser)
{
	FVector ImpulseDir;
	FHitResult Hit;
	AActor* HitInstigator = DamageCauser ? DamageCauser : nullptr;
	DamageEvent.GetBestHitInfo(DamagedVictim, HitInstigator, Hit, ImpulseDir);

	
	float BonusDamageFactor = 1.f;

	FString HitBoneName = Hit.BoneName.ToString();
	UE_LOG(LogTemp, Warning, TEXT("BChar::HitBone: %s"), *HitBoneName);


	if ( HitBoneName.Contains("Head", ESearchCase::IgnoreCase, ESearchDir::FromStart) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HeadShot!"));
		BonusDamageFactor = 1.55f;
	}
	else if ( HitBoneName.Contains("Neck", ESearchCase::IgnoreCase, ESearchDir::FromStart) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::Neck Shot!"));
		BonusDamageFactor = 1.3f;
	}
	else if ( HitBoneName.Contains("Spine_03", ESearchCase::IgnoreCase, ESearchDir::FromStart) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::Chest Shot!"));
		BonusDamageFactor = 1.3f;
	}


	//Damage formula
	float Armor = 30.f; 
	//DamagedVictim->Armor <<- 나중에 이걸로 바꿔야함.
	float CalculateDamage = BeforeDamage * BonusDamageFactor * ( 1 - Armor / 100 );

	return CalculateDamage;
}


void AOpenWorldRPGGameModeBase::ProcessKillEvent(AController* Killer, AController* KilledChar, struct FDamageEvent const& DamageEvent)
{
	AMyPlayerState* KillerState = Killer ? Killer->GetPlayerState<AMyPlayerState>() : nullptr;
	AMyPlayerState* VictimState = KilledChar ? KilledChar->GetPlayerState<AMyPlayerState>() : nullptr;

	if ( KillerState && Killer != KilledChar )
	{
		FVector ImpulseDir;
		FHitResult Hit;
		AActor* HitInstigator = Killer->GetCharacter() ? Killer->GetCharacter() : nullptr;
		DamageEvent.GetBestHitInfo(KilledChar, HitInstigator, Hit, ImpulseDir);

		if ( Hit.IsValidBlockingHit() )
		{
			FString HitBoneName = Hit.BoneName.ToString();
			if ( HitBoneName.Contains("Head", ESearchCase::IgnoreCase, ESearchDir::FromStart) )
			{
				KillerState->AddHeadShotCount();
			}

			KillerState->AddKillCount();

		}

		//KillerState->KillScore
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::you Kill Enemy!"));
	}

	if ( VictimState )
	{
		UMyDamageType* DmgType = Cast<UMyDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if ( DmgType )
		{
			VictimState->AddDeathCount();

			if ( APlayerController* PCon = Cast<APlayerController>(KilledChar) )
			{
				UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::You Died"));
				if ( PCon->bForceFeedbackEnabled )
				{
					FForceFeedbackParameters FFBParams;
					FFBParams.bLooping = false;
					FFBParams.Tag = "Death";
					FFBParams.bPlayWhilePaused = true;
					//PCon->PlayDynamicForceFeedback()
					PCon->ClientPlayForceFeedback(DmgType->KilledForceFB, FFBParams);
				}
			}
		}
		//VictimState->death
	}
}


void AOpenWorldRPGGameModeBase::ProcessHitEvent(AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent)
{
	//Player가 맞았을 경우
	if ( AMainController* PlayerCon = Cast<AMainController>(DamageReceiver) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HitEvent, I'm Hit!!" ));
		/*여기서 Playercon의 함수 호출하자.
		* Show Hit indicator 함수?를 만들어서 호출하면 될듯.
		*/
		if(!PlayerCon->MainHud) return;
		PlayerCon->MainHud->NotifyHitbySomething(DamageGiver, DamageReceiver, DamageEvent);
	}

	//Player가 enemy를 때려서 맞췄을 경우
	if ( AMainController* PlayerCon = Cast<AMainController>(DamageGiver) )
	{
		UE_LOG(LogTemp, Warning, TEXT("AOpenWorldRPGGameModeBase::HitEvent, you Hit someone!!"));
		/*맞췄다는 표시를 Hud에 띄우자.
		*/
		if ( !PlayerCon->MainHud ) return;
		PlayerCon->MainHud->NotifyHitSomething();
	}

}