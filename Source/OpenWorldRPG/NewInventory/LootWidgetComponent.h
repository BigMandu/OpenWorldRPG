// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootWidgetComponent.generated.h"

class AMainController;

UENUM()
enum class EWidgetType : uint8
{
	EWT_LootBox			UMETA(Displayname = "LootBox"),
	EWT_Character		UMETA(Displayname = "Character"),
	EWT_StorageEquip	UMETA(Displayname = "Equipment"),

	EWT_MAX				UMETA(Displayname = "DefaultsMAX")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API ULootWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULootWidgetComponent();

	UPROPERTY(EditAnywhere, Category = Enums)
	EWidgetType WidgetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LootWidget")
	TSubclassOf<UUserWidget> WBPWidget;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LootWidget")
	UUserWidget* Widget;*/

protected:
	// Called when the game starts
	//virtual void BeginPlay() override;

public:
	void CreateInteractionWidget(AMainController* MainCon, AActor* actor);
	

		
};
