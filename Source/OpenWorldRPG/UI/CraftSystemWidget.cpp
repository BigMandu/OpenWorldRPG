// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/CraftSystemWidget.h"
#include "OpenWorldRPG/UI/CraftItemWidget.h"

#include "OpenWorldRPG/GameData/MyGameInstance.h"
#include "OpenWorldRPG/GameData/CraftSystemComponent.h"

#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
//#include "Components/UniformGridPanel.h"
#include "Components/WrapBox.h"

void UCraftSystemWidget::NativeConstruct()
{
	
	UMyGameInstance* MyGameInst = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if(MyGameInst)
	{
		if(MyGameInst->CraftRecipeDataTable)
		{
			AMainCharacter* Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
			PlayerCraftComp = Player->CraftSysComp;

			CraftRecipeTable = MyGameInst->CraftRecipeDataTable;
			SettingCraftWidget();//MyGameInst->CraftRecipeDataTable);

			CraftButton->OnClicked.AddDynamic(this,&UCraftSystemWidget::CraftItem);
		}
	}
	
}

void UCraftSystemWidget::SettingCraftWidget()//UDataTable* CraftDT)
{
	//auto CraftMap = CraftDT->GetRowMap();
	if(CraftRecipeTable.IsValid() == false) return;
	
	CraftGridPanel->ClearChildren();

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

	
	//Table�� ���� Click���θ� Ȯ���Ѵ�.
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



//������ Click�Ѱ����� Ȯ���Ѵ�.
bool UCraftSystemWidget::bIsSameClickedItem(UCraftItemWidget* NewClickedWidget)
{
	//���� ����Ǿ��ִ� ���� �ְ�
	if (ClickedCraftWidget.IsValid())
	{
		//����� ���� ���� Click�� ���� ������ ������� false�� �����ϰ� ��ģ��.
		if (ClickedCraftWidget.Get() == NewClickedWidget)
		{
			//CraftButton�� ������ �ʱ�ȭ�� �� �Լ��� ���ľ� �ϴµ�, �̶� ClickedCraftWidget�� null�̱� ������ �Ʒ� false�� ���ݴ�.
			//return false;
		}
		else
		{
			//����widget�� ���� white�� �����Ѵ�.
			ClickedCraftWidget->SetBorderColor(FLinearColor::White);
		}
	}

	
	 
	//��Ŭ��widget���� �־��ش�.
	ClickedCraftWidget = NewClickedWidget;

	//���ο� widget�� ���� yellow�� �����Ѵ�.
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
				//���簳���� ���ڶ�� fasle
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
					//Pickup�� �����ߴٸ� CraftComp�� ����� ������ �����Ѵ�.

					/* �Ʒ��� Row�� ������ Row�ȿ� �ִ� NumberofIng�� ���� �ص��ǰ�,
					* RecipeBox�� �ڼհ����� ���� �ص� �ȴ�.
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
					//Craft�� �ߴٸ� Refresh��Ų��.
					SettingRecipe(ClickedCraftWidget.Get());
				}
				//Pickup�� �����ߴٸ� ������ Item�� Destory��Ų��.
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
		//OnWidget�� CurrentCount�� Player�� CraftSystemComp�� �����ϱ�...
		//��� �������� ��������.
		//SettingRecipe �Լ����� �ش� ���� �����ö�  TMap���·� �������� Key��(CraftItem�� Name)���� �����;� �ϱ⶧����
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