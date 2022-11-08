// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/InteractWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/BasePDA.h"
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

//WBP_InteractWidget���� FText�� �ش� �Լ��� �����Ѵ�.
//Actor�� DataAsset�� ������ �۵����� �ʴ´�.
FText UInteractWidget::InterSetText()
{
	FText ReturnTEXT;
	if (MainChar && MainChar->InteractActor)
	{
		AInteractable* InActor = Cast<AInteractable>(MainChar->InteractActor);
		if (InActor && InActor->ItemSetting.DataAsset)
		{
			//*@usage FText::FormatNamed(FText::FromString(TEXT("{PlayerName} is really cool")), TEXT("PlayerName"), FText::FromString(TEXT("Awesomegirl")));
		
			FText Action = InActor->ItemSetting.DataAsset->UseActionText;
			FText Name = InActor->ItemSetting.DataAsset->ItemName;

			FFormatNamedArguments args;
			args.Add(TEXT("Action"),Action);
			args.Add(TEXT("Name"),Name);

			FText text = FText::Format(LOCTEXT("InteractText", "{Action} {Name}"), args);

			ReturnTEXT = text;
		}

		
	}
#undef LOCTEXT_NAMESPACE

	return ReturnTEXT;
}