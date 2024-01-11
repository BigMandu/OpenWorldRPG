// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CraftSystemWidget.h"
#include "OpenWorldRPG/UI/CraftItemWidget.h"

#include "OpenWorldRPG/GameData/MyGameInstance.h"
#include "OpenWorldRPG/GameData/CraftSystemComponent.h"
#include "OpenWorldRPG/GameData/CraftRecipe.h"

#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "Components/WrapBox.h"
#include "Components/Button.h"

void UCraftSystemWidget::NativeConstruct()
{
	CraftButton->OnClicked.AddDynamic(this, &UCraftSystemWidget::CraftItem);
	SettingCraftWidget();//MyGameInst->CraftRecipeDataTable);
}

void UCraftSystemWidget::SettingCraftWidget()//UDataTable* CraftDT)
{
	//auto CraftMap = CraftDT->GetRowMap();
	UMyGameInstance* MyGameInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AMainCharacter* Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (!MyGameInst || !Player) return;
	if (!MyGameInst->CraftRecipeDataTable) return;
	
	
	PlayerCraftComp = Player->CraftSysComp;
	CraftRecipeTable = MyGameInst->CraftRecipeDataTable;


	if(CraftRecipeTable.IsValid() == false) return;
	
	CraftGridPanel->ClearChildren();
	ClearRecipe();
	PlayerCraftComp->DeleteAllCraftStatus();
	CheckCanCraft();


	for (auto looprow : CraftRecipeTable->GetRowMap()) //CraftDT->GetRowMap())
	{
		//loop.Key  has the key from first column of the CSV file
		//loop.Value has a pointer to a struct of data. you can safely cast it to your actual TYPE. e.g, FMyStruct* data = (FMyStruct*)(loop.Value);
		FString RowName = (looprow.Key).ToString();
		FCraftRecipeTable* Row = (FCraftRecipeTable*)looprow.Value;
		//UE_LOG(LogTemp,Warning,TEXT("RowName : %s, Crafted ItmeName : %s, Ind1 Name : %s, Ind2 Name: %s"),*RowName, *Row->CraftedItem->GetFName().ToString(),
		//*Row->Ingredient_One->GetFName().ToString(), *Row->Ingredient_Two->GetFName().ToString());

		//CraftGridPanel->GetDesiredSize();

		if (WCraftItemWidget)
		{
			UCraftItemWidget* CraftableWidget = CreateWidget<UCraftItemWidget>(this,WCraftItemWidget);
			CraftableWidget->InitCraftItemWidget(Row->CraftedItem, true);
			CraftableWidget->OnCraftWidgetClicked.AddDynamic(this, &UCraftSystemWidget::SettingRecipe);
			
			CraftGridPanel->AddChildToWrapBox(CraftableWidget);// AddChildToUniformGrid(CraftableWidget);
		}
	}
}

void UCraftSystemWidget::SettingRecipe(UCraftItemWidget* NewClickedWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("UCraftSystemWidget::SettingRecipe"));

	
	//Table과 이전 Click여부를 확인한다.
	if (CraftRecipeTable.IsValid() && bIsSameClickedItem(NewClickedWidget))
	{
		FString TableName(CraftRecipeTable->GetName());
		FName RowName = FName(ClickedCraftWidget->CraftItemDA->ItemName.ToString());

		 FCraftRecipeTable* Row = CraftRecipeTable->FindRow<FCraftRecipeTable>(RowName,TableName);
		 if(Row != nullptr)
		 {
			UE_LOG(LogTemp,Warning,TEXT("RowName : %s, Crafted ItmeName : %s, Ind1 Name : %s, Ind2 Name: %s"), *RowName.ToString(), 
			*Row->CraftedItem->GetFName().ToString(), *Row->Ingredient_One->GetFName().ToString(), *Row->Ingredient_Two->GetFName().ToString());

			CraftItemName->SetText(ClickedCraftWidget->CraftItemDA->ItemName);
			CraftItemDescription->SetText(ClickedCraftWidget->CraftItemDA->ItemDescription);

			switch (Row->NumberOfIngredient)
			{
			case 2:
				TwoIngredientRecipe(Row);
			break;
			case 3:
				ThreeIngredientRecipe(Row);
			break;
			case 4:
				FourIngredientRecipe(Row);
			break;
			case 5:
			break;
			}
		 }
	}

}

void UCraftSystemWidget::ClearRecipe()
{
	FText BlankText = FText::FromString(TEXT(" "));
	RecipeBox->ClearChildren();
	CraftItemName->SetText(BlankText);
	CraftItemDescription->SetText(BlankText);
}

//이전에 Click한것인지 확인한다.
bool UCraftSystemWidget::bIsSameClickedItem(UCraftItemWidget* NewClickedWidget)
{
	//기존 저장되어있는 값이 있고
	if (ClickedCraftWidget.IsValid())
	{
		//저장된 값과 새로 Click한 위젯 정보가 같은경우 false를 리턴하고 마친다.
		if (ClickedCraftWidget.Get() == NewClickedWidget)
		{
			//CraftButton을 누르고 초기화시 이 함수를 거쳐야 하는데, 이땐 ClickedCraftWidget이 null이기 때문에 아래 false를 없앴다.
			//return false;
		}
		else
		{
			//이전widget의 색을 white로 변경한다.
			ClickedCraftWidget->SetBorderColor(FLinearColor::White);
		}
	}

	
	 
	//새클릭widget값을 넣어준다.
	ClickedCraftWidget = NewClickedWidget;

	//새로운 widget의 색을 yellow로 변경한다.
	ClickedCraftWidget->SetBorderColor(FLinearColor::Yellow);

	return true;
}

bool UCraftSystemWidget::CheckCanCraft()
{

	bool bCanCraft = true;

	TArray<UWidget*> ChildArray = RecipeBox->GetAllChildren();
	for (auto IngredientWidget : ChildArray)
	{
		if (IngredientWidget)
		{
			UCraftItemWidget* IngWidget = Cast<UCraftItemWidget>(IngredientWidget);
			if (IngWidget)
			{
				//현재개수가 모자라면 fasle
				if (IngWidget->CurrentCount_num < IngWidget->NeedCount_num)
				{
					bCanCraft = false;
					break;
				}
			}
			else bCanCraft = false;
		}
		else bCanCraft = false;
	}

	if (bCanCraft)
	{
		CraftButton->SetBackgroundColor(FLinearColor::Green);
	}
	else
	{
		CraftButton->SetBackgroundColor(FLinearColor(0.5f,0.5f,0.5f,1.f));
	}

	return bCanCraft;
}

void UCraftSystemWidget::CraftItem()
{
	if(!PlayerCraftComp.IsValid()) return;

	if (CheckCanCraft() && ClickedCraftWidget.IsValid())
	{
		if (ClickedCraftWidget->CraftItemDA.IsValid())
		{
			UCustomPDA* EquipPDA = Cast<UCustomPDA>(ClickedCraftWidget->CraftItemDA);
			class AItem* SpawnedItem = nullptr;
			if (EquipPDA)
			{
				SpawnedItem = UCustomInventoryLibrary::SpawnEquipment(GetWorld(),EquipPDA);
			}
			else
			{
				SpawnedItem = UCustomInventoryLibrary::SpawnItem(GetWorld(),ClickedCraftWidget->CraftItemDA.Get());
			}
			

			
			if (SpawnedItem)
			{
				SpawnedItem->SetItemState(EItemState::EIS_Spawn);
				if (SpawnedItem->Pickup(PlayerCraftComp->GetOwner()))
				{
					//Pickup에 성공했다면 CraftComp에 저장된 내역을 삭제한다.

					/* 아래는 Row를 가져와 Row안에 있는 NumberofIng를 토대로 해도되고,
					* RecipeBox의 자손개수를 토대로 해도 된다.
					*/
					FString TableName(CraftRecipeTable->GetName());
					FName RowName = FName(ClickedCraftWidget->CraftItemDA->ItemName.ToString());
					FCraftRecipeTable* Row = CraftRecipeTable->FindRow<FCraftRecipeTable>(RowName, TableName);
					if (Row != nullptr)
					{
						switch (Row->NumberOfIngredient)
						{
							case 2:			
								PlayerCraftComp->DeleteCraftStatus_TwoIng(RowName);
							break;
							case 3:
								PlayerCraftComp->DeleteCraftStatus_ThreeIng(RowName);
							break;
							case 4:
								PlayerCraftComp->DeleteCraftStatus_FourIng(RowName);
							break;
							case 5:
							break;
						}
						
					}
					//Craft를 했다면 Refresh시킨다.
					SettingRecipe(ClickedCraftWidget.Get());
				}
				//Pickup에 실패했다면 생성한 Item을 Destory시킨다.
				else
				{
					SpawnedItem->Destroy();
				}
			}
		}
	}
	CheckCanCraft();
}


void UCraftSystemWidget::OnDropAtIngridentWidget(UCraftItemWidget* OnWidget, UCustomDDOperation* CusDDOper)
{
	UE_LOG(LogTemp, Warning, TEXT("UCraftSystemWidget::OnDropAtIngridentWidget"));

	if (PlayerCraftComp.IsValid())
	{
		//OnWidget의 CurrentCount를 Player의 CraftSystemComp에 저장하기...
		//어떻게 저장할지 생각중임.
		//SettingRecipe 함수에서 해당 값을 가져올때  TMap형태로 가져오고 Key값(CraftItem의 Name)으로 가져와야 하기때문에
		// 
		//		

		TArray<int32> IngCount;
		TArray<UWidget*> ChildArray = RecipeBox->GetAllChildren();

		for (auto IngredientWidget : ChildArray)
		{
			if(IngredientWidget != nullptr)
			{
				UCraftItemWidget* IngWidget = Cast<UCraftItemWidget>(IngredientWidget);
				if (IngWidget)
				{
					IngCount.Add(IngWidget->CurrentCount_num);
				}
			}
		}


		switch (IngCount.Num())
		{
		case 2:
		{
			FTwoIngredientRecipe RemainStatus = FTwoIngredientRecipe(IngCount[0], IngCount[1]);
			PlayerCraftComp->SaveCraftStatus_TwoIng(OnWidget->MotherItemName, RemainStatus);
		}
		break;
		case 3:
		{
			FThreeIngredientRecipe RemainStatus = FThreeIngredientRecipe(IngCount[0], IngCount[1], IngCount[2]);
			PlayerCraftComp->SaveCraftStatus_ThreeIng(OnWidget->MotherItemName, RemainStatus);
		}
		break;
		case 4:
		{
			FFourIngredientRecipe RemainStatus = FFourIngredientRecipe(IngCount[0], IngCount[1], IngCount[2], IngCount[3]);
			PlayerCraftComp->SaveCraftStatus_FourIng(OnWidget->MotherItemName, RemainStatus);
		}
		break;
		case 5:
		break;
		default:
			UE_LOG(LogTemp,Warning,TEXT("UCraftSystemWidget::OnDropAtIngridentWidget//Something wrong"));
		break;
		}

		
		CheckCanCraft();
	}

}


void UCraftSystemWidget::TwoIngredientRecipe(FCraftRecipeTable* Row)
{
	if (PlayerCraftComp.IsValid() == false) return;


	//Create RecipeNumberInfo Struct for TMap
	struct FReciepeNumberInfo
	{
		int32 IngCurrentNum;
		int32 IngNeedNum;
	};
	FReciepeNumberInfo ReciepeNumberInfo = FReciepeNumberInfo();


	//Create TMap for easy Setting of widget
	TMap<UBasePDA*, FReciepeNumberInfo> IngredientMap;

	//Load Craft Ingredient Number Status 
	FName MotherName = FName(Row->CraftedItem->ItemName.ToString());
	FTwoIngredientRecipe* LoadTwoIngReceipe = PlayerCraftComp->LoadCraftIngredient_TwoIng(MotherName);

	
	
	//If has exist Recipe data
	if (LoadTwoIngReceipe)
	{
		//Load IngOne Count And Add to TMap
		ReciepeNumberInfo.IngCurrentNum = LoadTwoIngReceipe->IngOneRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadTwoIngReceipe->IngTwoRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;		
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);
	}
	else
	{
		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;	
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;		
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);
	}
	

	


	//Clear And Setting Widget(Widget's Count)
	RecipeBox->ClearChildren();

	for (auto& IngItem : IngredientMap)//: IngArray)
	{
		if (IngItem.Key)
		{
			FReciepeNumberInfo NumInfo = IngItem.Value;

			UCraftItemWidget* IngredientWidget = CreateWidget<UCraftItemWidget>(this, WCraftItemWidget);
			IngredientWidget->InitCraftItemWidget(IngItem.Key, false, MotherName);
			IngredientWidget->SetCurrentCountTextBox(NumInfo.IngCurrentNum);
			IngredientWidget->SetNeedCountTextBox(NumInfo.IngNeedNum);

			IngredientWidget->OnDropCraftWidget.AddDynamic(this, &UCraftSystemWidget::OnDropAtIngridentWidget);
			RecipeBox->AddChildToWrapBox(IngredientWidget);
		}
	}

	CheckCanCraft();
}


void UCraftSystemWidget::ThreeIngredientRecipe(FCraftRecipeTable* Row)
{
	if (PlayerCraftComp.IsValid() == false) return;


	//Create RecipeNumberInfo Struct for TMap
	struct FReciepeNumberInfo
	{
		int32 IngCurrentNum;
		int32 IngNeedNum;
	};
	FReciepeNumberInfo ReciepeNumberInfo = FReciepeNumberInfo();


	//Create TMap for easy Setting of widget
	TMap<UBasePDA*, FReciepeNumberInfo> IngredientMap;

	//Load Craft Ingredient Number Status 
	FName MotherName = FName(Row->CraftedItem->ItemName.ToString());
	FThreeIngredientRecipe* LoadThreeIngReceipe = PlayerCraftComp->LoadCraftIngredient_ThreeIng(MotherName);



	//If has exist Recipe data
	if (LoadThreeIngReceipe)
	{
		//Load IngOne Count And Add to TMap
		ReciepeNumberInfo.IngCurrentNum = LoadThreeIngReceipe->IngOneRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadThreeIngReceipe->IngTwoRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadThreeIngReceipe->IngThreeRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngThree_NeedNum;
		IngredientMap.Add(Row->Ingredient_Three, ReciepeNumberInfo);
	}
	else
	{
		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngThree_NeedNum;
		IngredientMap.Add(Row->Ingredient_Three, ReciepeNumberInfo);
	}





	//Clear And Setting Widget(Widget's Count)
	RecipeBox->ClearChildren();

	for (auto& IngItem : IngredientMap)//: IngArray)
	{
		if (IngItem.Key)
		{
			FReciepeNumberInfo NumInfo = IngItem.Value;

			UCraftItemWidget* IngredientWidget = CreateWidget<UCraftItemWidget>(this, WCraftItemWidget);
			IngredientWidget->InitCraftItemWidget(IngItem.Key, false, MotherName);
			IngredientWidget->SetCurrentCountTextBox(NumInfo.IngCurrentNum);
			IngredientWidget->SetNeedCountTextBox(NumInfo.IngNeedNum);

			IngredientWidget->OnDropCraftWidget.AddDynamic(this, &UCraftSystemWidget::OnDropAtIngridentWidget);
			RecipeBox->AddChildToWrapBox(IngredientWidget);
		}
	}

	CheckCanCraft();
}


void UCraftSystemWidget::FourIngredientRecipe(FCraftRecipeTable* Row)
{
	if (PlayerCraftComp.IsValid() == false) return;


	//Create RecipeNumberInfo Struct for TMap
	struct FReciepeNumberInfo
	{
		int32 IngCurrentNum;
		int32 IngNeedNum;
	};
	FReciepeNumberInfo ReciepeNumberInfo = FReciepeNumberInfo();


	//Create TMap for easy Setting of widget
	TMap<UBasePDA*, FReciepeNumberInfo> IngredientMap;

	//Load Craft Ingredient Number Status 
	FName MotherName = FName(Row->CraftedItem->ItemName.ToString());
	FFourIngredientRecipe* LoadFourIngReceipe = PlayerCraftComp->LoadCraftIngredient_FourIng(MotherName);



	//If has exist Recipe data
	if (LoadFourIngReceipe)
	{
		//Load IngOne Count And Add to TMap
		ReciepeNumberInfo.IngCurrentNum = LoadFourIngReceipe->IngOneRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadFourIngReceipe->IngTwoRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadFourIngReceipe->IngThreeRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngThree_NeedNum;
		IngredientMap.Add(Row->Ingredient_Three, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = LoadFourIngReceipe->IngFourRemainCount;
		ReciepeNumberInfo.IngNeedNum = Row->IngFour_NeedNum;
		IngredientMap.Add(Row->Ingredient_Four, ReciepeNumberInfo);
	}
	else
	{
		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngOne_NeedNum;
		IngredientMap.Add(Row->Ingredient_One, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngTwo_NeedNum;
		IngredientMap.Add(Row->Ingredient_Two, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngThree_NeedNum;
		IngredientMap.Add(Row->Ingredient_Three, ReciepeNumberInfo);

		ReciepeNumberInfo.IngCurrentNum = 0;
		ReciepeNumberInfo.IngNeedNum = Row->IngFour_NeedNum;
		IngredientMap.Add(Row->Ingredient_Four, ReciepeNumberInfo);
	}





	//Clear And Setting Widget(Widget's Count)
	RecipeBox->ClearChildren();

	for (auto& IngItem : IngredientMap)//: IngArray)
	{
		if (IngItem.Key)
		{
			FReciepeNumberInfo NumInfo = IngItem.Value;

			UCraftItemWidget* IngredientWidget = CreateWidget<UCraftItemWidget>(this, WCraftItemWidget);
			IngredientWidget->InitCraftItemWidget(IngItem.Key, false, MotherName);
			IngredientWidget->SetCurrentCountTextBox(NumInfo.IngCurrentNum);
			IngredientWidget->SetNeedCountTextBox(NumInfo.IngNeedNum);

			IngredientWidget->OnDropCraftWidget.AddDynamic(this, &UCraftSystemWidget::OnDropAtIngridentWidget);
			RecipeBox->AddChildToWrapBox(IngredientWidget);
		}
	}

	CheckCanCraft();
}