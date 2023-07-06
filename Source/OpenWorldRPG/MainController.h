// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainController.generated.h"

/**
 * 
 */

class UUserWidget;
class AMainCharacter;

UCLASS()
class OPENWORLDRPG_API AMainController : public APlayerController
{
	GENERATED_BODY()
private:
	

	TArray<class AEnemyAIController*> TargetAIList;
public:

	AMainController();

	AMainCharacter* Main;

	/****** Main Hud *****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | MainHUD")
	TSubclassOf<UUserWidget> WMainHud;
	class UMainHud* MainHud;

	/***** Widget ******/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Interact")
	TSubclassOf<UUserWidget> WInteractText;
	UUserWidget* InteractText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Inventory")
	TSubclassOf<UUserWidget> WInventory;
	UUserWidget* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Menu")
	TSubclassOf<UUserWidget> WDeathWidget;
	UUserWidget* DeathWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets | Loading")
	TSubclassOf<UUserWidget> WLoadingWidget;
	UUserWidget* LoadingWidget;

	FTimerHandle LoadingwidgetTimer;

	bool bIsInteractLootBox;
	bool bIsInteractCharacterLoot;
	bool bIsInventoryVisible;
	bool bIsCompassWidgetVisible;

	/********** Vehicle ********/
	bool bIsinCar;
	TWeakObjectPtr<class ANiceCar> Car;

	/* Save */
	UPROPERTY()
	class USavePlayer* SaveGame_Player;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:	

	void ToggleInventory();

	void ControlInteractText(bool bIsInteract);

	void ControlCoreUsableWidget(bool bIsTPSMode);

	void CreateLootWidget();
	void ShowLootBoxWidget();
	void HideLootBoxWidget();

	void SetInputAndFocus(bool bIsShow);

	void Die();

	void Respawn();

	/*************Vehicle *********/
	bool ToggleCar(class ANiceCar* vCar, FVector OutPos = FVector(0.f));
	bool GetIntheCar(ANiceCar* vCar);
	void GetOuttheCar(FVector OutPos);//ANiceCar* Car);


	/* 아래 함수 3개는 AI가 이 Player를 식별 했을 때
	* Player의 특정 행동에 제약을 주기 위해
	* 
	* 현재 이 제약을 적용한 행동리스트
	* - 교전중 차량 탑승 불가,
	*
	* 이 Player를 식별한 AI를 관리한다.
	*/

	/*AI가 이 Controller의 OwnerActor를 식별하면
	* 리스트에 해당 AI를 추가한다.*
	* AIController::DetectEnemy에서 호출
	*/
	void AddToListTargetingThisActor(class AEnemyAIController* AICon);

	//AIController::LostTarget, BaseChar::Die에서 호출됨
	void RemoveAtListTargetingThisActor(AEnemyAIController* AICon);

	/*이 Controller의 OwnerActor를 식별한 AI의 목록을 얻는다.* */
	//TArray<AEnemyAIController> GetListTargetingThisActor();
	bool bIsthisTargetForAI();

	//void UseQuickSlotItem(EQuickSlotNumber QuickSlotNum);


	/* Save And Load Game */
	void SaveGame();
	void LoadGame();


	void SavePlayerStatus();
	void LoadPlayerStatus();

	void OriginalToCopy(TArray<class UNewItemObject*> &Original, TArray<struct FSaveItem> &Copy);
	void CopyToOriginal(TArray<class UNewItemObject*> &Original, TArray<struct FSaveItem> &Copy);

	UFUNCTION()
	void LoadingProcessCompleted();

};
