// Fill out your copyright notice in the Description page of Project Settings.



#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/CustomInventoryLibrary.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/EnemyAIController.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Sound/SoundCue.h"


AEquipment::AEquipment() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	EquipInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	//Mesh->CanCharacterStepUpOn = ECB_No;

	SKMesh->SetHiddenInGame(true);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Overlap);

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	/*if (bHasStorage)
	{
		SettingStorage();
	}*/

}

void AEquipment::ReInitialize(UNewItemObject* Obj)
{
	if(ItemObj)// && EquipGridWidget == nullptr)
	{
		ItemObj = Obj;
		//if(bHasStorage) ����� �׳� �ٷ� SettingStorage�� ��������.
		{
			/*if (MainCon == nullptr) 
			{
				MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			}*/
			//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			//if (WEquipGridWidget)
			//{
			//	if(MainCon)
			//	{
			//		EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			//		EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);


			//	}
			//	//else if(AICon)
			//	//{
			//		EquipGridWidget = CreateWidget<UNewInventoryGrid>(AICon, WEquipGridWidget);
			//	//}
			//}
		}
	}
}

//�θ� Ŭ�������� Beginplay�� �ѹ��� ����.
UNewItemObject* AEquipment::GetDefaultItemObj()
{
	UNewItemObject* Obj = Super::GetDefaultItemObj();

	if(Obj && bHasStorage && EquipInventoryComp)
	{
		Obj->SetItemInvComp(EquipInventoryComp);
	}
	return Obj;
}

void AEquipment::SetOwningPlayer(AActor* Actor)
{
	if (OwningPlayer != Actor)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
		if (BChar)
		{
			AMainController* TMainCon = Cast<AMainController>(BChar->GetController());
			AEnemyAIController* TAICon = Cast<AEnemyAIController>(BChar->GetController());
			if(TMainCon)
			{
				MainCon = TMainCon;
			}
			else if(TAICon)
			{
				AICon = TAICon;
			}

			OwningPlayer = BChar;
			SetInstigator(BChar); //Instigator ����.
		}
	}
}

AActor* AEquipment::GetOwningPlayer()
{
	if (OwningPlayer)
	{
		return OwningPlayer;
	}
	return nullptr;
}

bool AEquipment::Equip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	
	if (BChar)
	{
		/*�� ������ Ÿ�԰� ��ġ�ϴ� ���Ⱑ �̹� �ִٸ�
		 * 1. �� ���Ⱑ ���忡 ������ ���¶��, Inventory�� �߰��� �õ��Ѵ�.
		 * 2. �� ���Ⱑ Inventory�� �ִ� ���¶�� ���� ������ ���� �ٲ� ���� �Ѵ�.
		 * ��ġ�ϴ� ���Ⱑ ���ٸ� Equip�Լ��� ȣ���Ѵ�.
		 */
		if (BChar->Equipment->IsSameTypeExist(this,RifleSlot))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//�κ��丮�� �� item�� ������ �Լ��� �����Ѵ�.
				SendToInventory(BChar);
				return true;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* ���� �Ϸ��� ��� Pickup����(Inventory�� �ִ� ����)��
				 * ��� Swap�� �����Ѵ�.
				 * 
				 * Weapon�� ���, RifleAssign�� 
				 */
				
				AEquipment* BeforeEquipment = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, this, RifleSlot);
				if (BeforeEquipment)
				{
					BChar->Equipment->RemoveEquipment(BeforeEquipment); //SwapEquipment(BeforeEquipment, this);
					BeforeEquipment->SendToInventory(BChar);
					/*this�� SpawnActor�ؼ� Data�� �̰��ϴ� �۾��� �ϴ� �Լ��� ȣ���ؾ��Ѵ�.
					 * UEqupmentSlot::NativeDrop���� �ϴ� ��ƾ�� �Լ�ȭ �ؾ���
					 */
				}
			}
		}

		//���⼭ 2���б�� ������. 
		//if������ ������ ���� INV�� �ִ� Equipment�� Drag&Drop���� �������� �ʰ�. 
		//Equip��ư�� ���� �����ϴ°��
		if (ItemObj->bIsDestoryed)
		{
			//UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj); //�Լ�ȭ ��.
			//�� SpawnEquipment���� BChar�� �Ѱ��ִ� �Ű������� �߰��ؾߵ�.
			//�ش� �Լ����� �ٷ� StepEquip�� ������ ���⼱ �׳� return�ϸ��.

			//���� ������ Equipment�� invComp�� ItemObj->InvComp���� �̰�����.
			UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj, Actor);
			return true;
		}
		//�� ���� INV�� �ִ� Equipment�� Drag&Drop���� ���� �������. (EquipSlot::TrySlotEquip���� ȣ����)
		else
		{
			//Destoryed�� �ƴϸ� �׳� �� ���ϰ� �ٷ� step equip����
			bReturn = StepEquip(Actor, RifleSlot); //������� EquipInventory�� ������
		}
	}
	return bReturn;
}

bool AEquipment::StepEquip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	const USkeletalMeshSocket* Socket = nullptr;
	switch (EquipmentType)
	{
		
	case EEquipmentType::EET_Helmet:
		Socket = BChar->GetMesh()->GetSocketByName("headsocket");
		break;
	case EEquipmentType::EET_Vest:
		Socket = BChar->GetMesh()->GetSocketByName("VestSocket");
		break;
	case EEquipmentType::EET_Backpack:
		Socket = BChar->GetMesh()->GetSocketByName("BackpackSocket");
		break;
	}

	if (Socket != nullptr || (EquipmentType == EEquipmentType::EET_Rifle || EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);
		}
		//if (Socket->AttachActor(this, BChar->GetMesh()))

		SKMesh->SetHiddenInGame(false);
		Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwningPlayer(BChar);
		//Main�� �ִ� Equipment�� Add���ش�.
		BChar->Equipment->AddEquipment(this);

		bReturn = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment::StepEquip, Socket is null "));
	}

	/*if (EquippedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
	}*/

	return bReturn;
}



void AEquipment::SendToInventory(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	check(BChar);

	OwningEquipment = nullptr;
	ItemObj->bIsDestoryed = true;

	//���� �߰��� InventoryComponent�� ������.
	/*if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}*/

	//�θ� class�� �ִ� Pickup�Լ� ȣ���ؼ� item�� ��´�.
	Pickup(BChar);
}

//�� �Լ��� ���߿� CustomInventoryLibrary�� �����ҵ�.
//ItemObj�� ����ؾߵ�.
void AEquipment::SettingStorage()
{
	//EquipGridWidget�� �������� �����Ѵ�.
	if (EquipGridWidget == nullptr)
	{
		if (MainCon == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SettingStorage::MainCon is nullptr try Get Controller"));
			if (OwningPlayer)
			{
				//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				MainCon = Cast<AMainController>(OwningPlayer->GetController());
			}
		}

		if (WEquipGridWidget && MainCon)
		{
			EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			if (EquipGridWidget && EquipInventoryComp)//ItemObj->GetItemInvComp() != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("SettingStorage::Try Initialize EquipGridWidget "));
				
				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap�ÿ� ���� �߻� (ItemObj�� InvComp�� null�� ������ ����)
				//EquipGridWidget->GridInitialize(ItemObj->GetItemInvComp(), ItemObj->GetItemInvComp()->TileSize);
				EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
				
			}
		}
	}
}

void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}

void AEquipment::Remove()
{
	/* �ƹ��͵� ����. .. ��..*/
	//Weapon�� Remove�� ����. �� ȣ���ؾߵ�.

	if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}

}