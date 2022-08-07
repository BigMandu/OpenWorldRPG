// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/InteractWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "MyNamespace"

void UInteractWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (MainChar == nullptr)
	{
		//GetOwningPlayer();
		//GetOwningLocalPlayer();
		MainChar = Cast<AMainCharacter>(GetOwningPlayerPawn());
		check(MainChar);
		
	}
}

FText UInteractWidget::InterSetText()
{
	FText ReturnTEXT;
	if (MainChar)
	{
		if (MainChar->InteractActor)
		{
			AInteractable* InActor = Cast<AInteractable>(MainChar->InteractActor);
			if (InActor && InActor->ItemSetting.DataAsset)
			{
				//*@usage FText::FormatNamed(FText::FromString(TEXT("{PlayerName} is really cool")), TEXT("PlayerName"), FText::FromString(TEXT("Awesomegirl")));
				
				/*FString Action = InActor->CusPDA->UseActionText.ToString();
				FString Name = InActor->CusPDA->ItemName.ToString();*/

				FText Action = InActor->ItemSetting.DataAsset->UseActionText;
				FText Name = InActor->ItemSetting.DataAsset->ItemName;

				FFormatNamedArguments args;
				args.Add(TEXT("Action"),Action);
				args.Add(TEXT("Name"),Name);

				//FString Str = FString(TEXT("{Action} {Name}"));
				FText text = FText::Format(LOCTEXT("InteractText", "{Action} {Name}"), args);

				ReturnTEXT = text;
				//FTextFormat Tform = FTextFormat(FText::FromString(Str));


				//FTextFormat TTForm = FTextFormat(Str, FTextFormatPatternDefinitionConstRef());
				
				//ReturnTEXT = FText::Format(, args); //그대로 {Action}{Name}이렇게 나옴. ㅋㅋ  확인 해야죔
				//ReturnTEXT->FromString(TEXT("{")
			}

		}
	}
#undef LOCTEXT_NAMESPACE

	return ReturnTEXT;
}