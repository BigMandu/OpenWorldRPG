// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"

#include "MainMenuWidget.generated.h"



/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	//TArray<UBorder> border

	UPROPERTY(meta = (bindwidget))
	class UButton* NewGame;
	UPROPERTY(meta = (bindwidget))
	UButton* Continue;
	UPROPERTY(meta = (bindwidget))
	UButton* Option;
	UPROPERTY(meta = (bindwidget))
	UButton* Credit;
	UPROPERTY(meta = (bindwidget))
	UButton* Exit;


	/* Sound */
private:

	FTimerHandle GetControlHandle;
	//Loop
	class USoundCue* MainThemeSound;

	USoundCue* ClickSound;



public:

	/* func */

	virtual void NativeConstruct() override;
	void GetControllerDelayed();


	void CheckContinueButtonEnabled();

	UFUNCTION()
	void Click_Newgame();

	UFUNCTION()
	void Click_Continue();

	UFUNCTION()
	void Click_Option();

	UFUNCTION()
	void Click_Credit();

	UFUNCTION()
	void Click_Exit();



	UFUNCTION()
	void Hoverbutton();
	UFUNCTION()
	void UnHoverbutton();


};
