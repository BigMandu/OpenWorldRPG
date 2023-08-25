// Fill out your copyright notice in the Description page of Project Settings.


#include "SubTitleWidget.h"
#include "Components/TextBlock.h"

void USubTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	RemoveSub();
}

void USubTitleWidget::SetSub(FText CharacterName, FText Script, bool bBlueTeam)
{
	/* Name Color Setting */
	FLinearColor JudgeColor = FLinearColor::Red;
	if (bBlueTeam)
	{
		JudgeColor = FLinearColor::Green;
	}
	FSlateColor NameColor = FSlateColor(JudgeColor);
	Name->SetColorAndOpacity(NameColor);


	FTextFormat TextForm = FTextFormat(NSLOCTEXT("SubNS", "Name", "{name}: "));
	FText FormText = FText::Format(TextForm, CharacterName);

	Name->SetText( FormText/*CharacterName*/);
	Context->SetText(Script);


	//CharName과 Script를 합치려 했으나 Charname에 color를 주기위해 text를 분할 하기로 함.
	/*FFormatNamedArguments args;
	args.Add(TEXT("name"), vName);
	args.Add(TEXT("text"), vText);

	FTextFormat TxtForm = FTextFormat(NSLOCTEXT("HUD", "SubTitle", "{name} : {text}"));

	FText FomatText = FText::Format(TxtForm, args);
	SubtitleText->SetText(FomatText);*/
}
void USubTitleWidget::RemoveSub()
{
	Name->SetText(FText::FromString(" "));
	Context->SetText(FText::FromString(" "));
}
