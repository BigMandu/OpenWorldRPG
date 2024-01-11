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

	TWeakObjectPtr<class UOptionWidget> OptionWidget;

	FTimerHandle GetControlHandle;

	/* Sound */
	//Loop
	class USoundCue* MainThemeSound;

	USoundCue* ClickSound;

public:

	UPROPERTY(meta = (bindwidget))
	class UButton* NewGameBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* ContinueBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* OptionBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* CreditBtn;
	UPROPERTY(meta = (bindwidget))
	UButton* ExitBtn;

private:

	UFUNCTION()
	void RemoveAdditionalWidget();



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

};
