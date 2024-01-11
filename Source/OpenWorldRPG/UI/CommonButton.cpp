// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CommonButton.h"

void UCommonButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	SettingButton();
}

void UCommonButton::SettingButton()
{
	BtnText->SetText(TextContext);
	BtnText->SetColorAndOpacity(TextColor);
	BtnText->SetFont(TextInfo);
}