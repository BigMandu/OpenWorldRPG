// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/UI/CharacterStatusWidget.h"
#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
//#include "OpenWorldRPG/Item/Interactive_Interface.h"


#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h" //디버깅용


// Sets default values
AMainCharacter::AMainCharacter() : Super()
{
	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //웅크리기 키 Toggle을 true로 기본세팅 한다.
	bWalkToggle = true; //걷기 키 Toggle을 true로 세팅한다.
	bAimToggle = true; //조준키를 toggel true세팅.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;
	ActiveInteractDistance = 200.f; //상호작용 아이템이 표시되는 최대거리.

	/* 카메라 관련 */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraTPS"));
	CameraTPS->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->TargetArmLength = MAXCameraLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraTPS->bUsePawnControlRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3인칭 카메라를 살짝 우측으로 치우지게 한다.

	BaseTurnRate = 45.f;
	BaseLookupRate = 45.f;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

	/****** First Person *****/
	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPS"));
	CameraFPS->SetupAttachment(GetRootComponent());

	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->SetRelativeTransform(CameraFPSTransform);//SetRelativeLocationAndRotation(FVector(0.f, 8.f, 0.f), FRotator(-90.f, 0.f, 90.f));

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	FPLowerLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPLegMesh"));
	FPLowerLegMesh->SetupAttachment(GetRootComponent());
}


void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Player의 AnimInstance를 불러온다. */
	//TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); 
	FPAnimInstance = Cast<UMainAnimInstance>(FPMesh->GetAnimInstance());

	if (FPAnimInstance)//TPAnimInstance && FPAnimInstance)
	{
		/* 사운드는 TP Animation을 기준으로 출력한다. */ //AnimInstance의 StepSound_Notify에서 호출.
		//TPAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); 
		//TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
	}


	FPMeshOriginTransform = FPMesh->GetRelativeTransform();
	FPMesh->SetHiddenInGame(true);
	
	//FPMesh->HideBoneByName(FName("spine_02"));
	FPLowerLegMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_l"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_r"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->SetHiddenInGame(true);

	FPLowerLegMesh->SetCastShadow(false);
	FPLowerLegMesh->SetOnlyOwnerSee(true);
	FPLowerLegMesh->SetReceivesDecals(false);

	/* FPS Aim관련 기본 설정 값 저장*/
	BaseCamTPSfov = CameraTPS->FieldOfView;
	BaseCamFPSfov = CameraFPS->FieldOfView;
	BaseFPMeshTransform = FPMesh->GetRelativeTransform();


	//GetCapsuleComponent()->SetCollisionProfileName(FName("MainChar"));
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	//GetCapsuleComponent()->SetCollisionProfileName("MainChar");

	
	
	
	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	/********** 초기 세팅 ************/
	SetCameraMode(ECameraMode::ECM_TPS); //초기 카메라 모드는 3인칭 모드로.
	SetAimMode(EAimMode::EAM_NotAim);

	MainController = Cast<AMainController>(GetController());


	/****** Main Hud의 Widget과 Event Bind ********/
	UMainHud* MainHud = Cast<UMainHud>(MainController->MainHud);
	if (MainHud)
	{
		UCharacterInventoryWidget* CharWidget = MainHud->NewInventoryWidget->CharInvWidget;
		if (CharWidget)
		{
			CharWidget->InitializeInventory(this);
			CharWidget->BindingAdditional(MainHud->NewInventoryWidget);
		}

		if(StatManagementComponent)
		{
			MainHud->CharStatWidget->BindStatManager(StatManagementComponent);
		}
		
		
		if (MainHud->WeaponStatus)
		{
			MainHud->WeaponStatus->BindWeaponWidget(this);
		}

		if (MainHud->QuickSlot)
		{
			MainHud->QuickSlot->BindSlotWidget(this);
		}

	}
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//bIsLookInput 세팅
	float turnvalue = GetInputAxisValue("Turn");
	float lookvalue = GetInputAxisValue("LookUp");
	if (turnvalue == 0.f && lookvalue == 0.f)
	{
		bIsLookInput = false;
	}
	else
	{
		bIsLookInput = true;
	}

	/* 상호작용 Line Trace 변경 . ver2  아래 코드를 GetPlayerViewPoint함수를 이용해 간략화 시켰다. */
	FRotator Rot;
	MainController->GetPlayerViewPoint(Interact_LineTrace_StartLocation, Rot);
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * (MAXCameraLength + 800.f); //카메라Boom길이보다 더길게 끝나야한다.
	}
	
	/* 카메라 모드에 따른 상호작용 LineTrace 변경. ver 1 --> ver 2로 교체.*/
	/*
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_StartLocation = CameraFPS->GetComponentLocation();
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraFPS->GetComponentRotation().Vector() * 500.f;

		break;
	case ECameraMode::ECM_TPS:
	{
		Interact_LineTrace_StartLocation = CameraTPS->GetComponentLocation(); //TPS카메라를 기준으로 Trace를 시작한다.
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraTPS->GetComponentRotation().Vector() * (MAXCameraLength + 800.f); //카메라Boom길이보다 더길게 끝나야한다.
		break;
	}
	default:
		break;
	}
	*/


	//나중에 Event로 바꿔야함
	/* 3인칭 모드에서 총이 있고 움직이고 있으면 시점을 방향으로 지정한다.*/
	//if (EquippedWeapon && CameraMode == ECameraMode::ECM_TPS)
	//{
	//	bUseControllerRotationYaw = true;
	//	GetCharacterMovement()->bOrientRotationToMovement = false;
	//}
	

	/* 상호작용 텍스트를 띄움 */
	/* Static FHitResult를 리턴받아서 Interface 변환, 성공하면 Outline과 TEXT를 띄움.*/
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();
	if (HitActor)
	{
		if (InteractActor) //이미 InteractActor가 있고
		{
			if (HitActor != InteractActor) //지금 보고 있는 Actor와 InteractActor가 다르면
			{
				UnsetInteractActor(); //InteractActor를 Unset해준다.
			}
			else
			{
				//거리에 따라 InteractText가 업데이트 되기 때문에 추가 해준다.
				SetInteractActor(HitActor); 
			}
		}
		else //InteractActor가 없을 경우
		{
			//AInteractable* Interactable = Cast<AInteractable>(HitActor);
			//if (Interactable)
			{
				//SetInteractActor(HitActor); //Interactable인 경우 Set을 해준다.
				SetInteractActor(HitActor); //Interactable인 경우 Set을 해준다.
			}
		}
	}
	else
	{
		UnsetInteractActor();
	}
	//InteractActor가 다를경우 UnsetInteractActor 호출하자. (Outline이 안없어지는 버그가 있음.) -> 해결함.

	/* Weapon의 AimInitialize함수에서 사용하는 Timer 해제용 */
	if (EquippedWeapon)
	{
		//타이머가 작동중이고
		if (GetWorldTimerManager().IsTimerActive(EquippedWeapon->AimInitHandle))
		{
			//aim을 돌리거나, AlphaTime이 1초가 넘어가거나, 쏘는 중이라면
			if (bIsLookInput || EquippedWeapon->AlphaTime >= 1.f || EquippedWeapon->bIsFiring)
			{
				//에임 초기화 타이머를 초기화 시킨다.
				UE_LOG(LogTemp, Warning, TEXT("MainChar:: Clear AimInit Timer"));
				GetWorldTimerManager().ClearTimer(EquippedWeapon->AimInitHandle);
			}
		}
		if (EquippedWeapon->RecoilAlphaTime >= 1.f)
		{
			GetWorldTimerManager().ClearTimer(EquippedWeapon->RecoilHandle);
		}
		
	}
	
	
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/************** Movement & sight key bind ***************/
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	/* Bind Action */
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBUp);

	PlayerInputComponent->BindAction("ChangeSafetyLever", IE_Pressed, this, &ABaseCharacter::ChangeSafetyLever);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::MyStopJumping);
	
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);


	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::UnSprint);

	PlayerInputComponent->BindAction("QSlot_4", IE_Pressed, this, &AMainCharacter::QuickSlotNum4);
	PlayerInputComponent->BindAction("QSlot_5", IE_Pressed, this, &AMainCharacter::QuickSlotNum5);
	PlayerInputComponent->BindAction("QSlot_6", IE_Pressed, this, &AMainCharacter::QuickSlotNum6);
	PlayerInputComponent->BindAction("QSlot_7", IE_Pressed, this, &AMainCharacter::QuickSlotNum7);
	PlayerInputComponent->BindAction("QSlot_8", IE_Pressed, this, &AMainCharacter::QuickSlotNum8);
	PlayerInputComponent->BindAction("QSlot_9", IE_Pressed, this, &AMainCharacter::QuickSlotNum9);



	/************ Camera *************/
	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AMainCharacter::VKeyDN);
	PlayerInputComponent->BindAction("ScrollDN", IE_Pressed, this, &AMainCharacter::ScrollDN);
	PlayerInputComponent->BindAction("ScrollUP", IE_Pressed, this, &AMainCharacter::ScrollUP);

	
	/******** Weapon Quick Swap ***********/
	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &ABaseCharacter::ChangePrimaryWeapon);
	PlayerInputComponent->BindAction("Sub", IE_Pressed, this, &ABaseCharacter::ChangeSubWeapon);
	PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &ABaseCharacter::ChangePistolWeapon);

	/************** Interactive & Inventory key bind ************/

	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AMainCharacter::TabKeyDown);

	PlayerInputComponent->BindAction("Interactive", IE_Pressed, this, &AMainCharacter::EKeyDown);

}


void AMainCharacter::FPSAimLocationAdjust()
{
	if (CameraFPS)
	{
		if (bIsAim)
		{
			//각 총마다 FPMesh의 올바른 위치에 부착하기 위해
			//Weapon Class에 Transform 변수를 만들어줬다.
			const FTransform NewFPMeshTransform = EquippedWeapon->CharFPMeshTransform;
			const FTransform NewWeaponTransform = EquippedWeapon->WeapSKMeshTransform;
			CameraFPS->SetFieldOfView(70.f);
			//FPMesh->SetRelativeLocation(NewFPMeshLocation);
			FPMesh->SetRelativeTransform(NewFPMeshTransform);
			EquippedWeapon->SKMesh->SetRelativeTransform(NewWeaponTransform);
		}
		else
		{
			CameraFPS->SetFieldOfView(BaseCamFPSfov);
			FPMesh->SetRelativeTransform(BaseFPMeshTransform);
			EquippedWeapon->SKMesh->SetRelativeTransform(BaseWeapTransform);
		}
	}
}


//플레이어 시점 상태 -> VKeyDN
void AMainCharacter::SetCameraMode(ECameraMode Type)
{
	CameraMode = Type;
	switch (CameraMode)
	{
	case ECameraMode::ECM_TPS:
		CameraTPS->Activate();
		CameraFPS->Deactivate();

		/* 회전시 카메라에만 영향 가도록 설정 */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = true;
		UE_LOG(LogTemp,Warning,TEXT("MainChar::SetCamMode / Setting ControllerRotation "));
		GetCharacterMovement()->bOrientRotationToMovement = false;
		UE_LOG(LogTemp, Warning, TEXT("MainChar::SetCamMode / Setting OrientRotationToMov "));
		/* Character Mesh 설정 */
		FPMesh->SetHiddenInGame(true);  //1인칭 Mesh 숨김
		GetMesh()->SetHiddenInGame(false); //3인칭 Mesh 안숨김

		GetMesh()->SetRenderInMainPass(true);
		FPLowerLegMesh->SetHiddenInGame(true);

		/* 장착한 장비 모두 HIde하기 */
		for (auto Equipped : Equipment->EquipmentItems)
		{
			if (Equipped && Equipped->Equipment)
			{
				Equipped->Equipment->SKMesh->SetRenderInMainPass(true);
				//Equipped->Equipment->SetActorHiddenInGame(true); //SetHidden(true);
			}
		}

	break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//카메라의 Yaw회전에 따라 캐릭터의 Yaw Mesh가 회전하도록 한다. 그래야 뒷걸음, 좌우 게걸음이 가능하다.
		bUseControllerRotationYaw = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;

		/*  Character Mesh 설정 */
		FPMesh->SetHiddenInGame(false);
		FPLowerLegMesh->SetHiddenInGame(false);

		GetMesh()->SetHiddenInGame(true);
		//bRenderInMainPass를 false로 하게되면 Rendering은 되지 않지만, Shadow는 렌더링 된다.
		// 이걸 사용하려면 Mesh가 Show되어야 한다. SetHiddenInGame을 True로 하면 안됨.	
		GetMesh()->SetRenderInMainPass(false);

	

		UCustomPDA* CPDA;
		/* 장착한 장비 모두 HIde하기 */
		for (auto Equipped : Equipment->EquipmentItems)
		{
			if (Equipped && Equipped->Equipment)
			{
				CPDA = Cast<UCustomPDA>(Equipped->Equipment->ItemSetting.DataAsset);

				if (CPDA && 
					(CPDA->EquipmentType == EEquipmentType::EET_Rifle
					|| CPDA->EquipmentType == EEquipmentType::EET_Pistol))
				{
					continue;
				}
				Equipped->Equipment->SKMesh->SetRenderInMainPass(false);
				//Equipped->Equipment->SetActorHiddenInGame(true); //SetHidden(true);
			}
		}

	break;
	}

	/* 장착 무기 Mesh에 부착 */
	if (EquippedWeapon)
	{
		EquippedWeapon->GunAttachToMesh(this);
	}

}

//RMB Down
void AMainCharacter::SetAimMode(EAimMode Mode)
{
	AimMode = Mode;
	if (EquippedWeapon)
	{
		switch (AimMode)
		{
		case EAimMode::EAM_Aim:
		{
			bIsAim = true;
			EquippedWeapon->bIsAiming = true;
			GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;

			if (CameraMode == ECameraMode::ECM_TPS)
			{
				//TPS모드 + Aim상태일때는 카메라를 살짝 앞으로 땡겨준다.
				//현재 SpringArm의 길이를 저장한다.
				BeforeCameraLength = CameraBoom->TargetArmLength;
				//float CameraLength = FMath::FInterpTo(BeforeCameraLength, MINCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);
				CameraBoom->TargetArmLength = MINCameraLength + 20.f;
				CameraTPS->SetRelativeLocation(TPSCam_Aim_Rel_Location);
				CameraTPS->FieldOfView = 77.f;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//정확한 위치를 받아오기 위해 Animation을 끈다.
				//FPMesh->bPauseAnims = true;

				//EquippedWeapon에서 SightSocket을 Get하는 함수를 호출한다.
				FTransform SightTransform = EquippedWeapon->GetSightSocketTransform();
				FTransform FPMeshWorld = FPMesh->GetComponentTransform();


				//SightSocket에서  FPMesh의 Relative Transform을 구한다.
				//이렇게 구한 Offset을 Mesh의 Transform에 적용한다.

				//explain Make Relative Transform
				//Example: ChildOffset = MakeRelativeTransform(Child.GetActorTransform(), Parent.GetActorTransform())
				//This computes the relative transform of the Child from the Parent.

				FTransform  Offset = FPMeshWorld.GetRelativeTransform(SightTransform);

				// Weapon의 Clipping을 위한 Trace를 FPMesh일때 Aim, NotAim을 보완하기 위해 Aim일때 Mesh를 앞으로 좀 나가게 한다.
				// (기존 Aim때는 FPMesh가 뒤로 들어가버려 Clipping문제가 안일어 났음.)
				FTransform OffsetWithoutX = FTransform(Offset.GetRotation(), FVector(5.f, Offset.GetTranslation().Y, Offset.GetTranslation().Z));


				//FPMesh를 해당 Offset만큼 이동, Camera의 중앙에 오도록 한다.
				//FPMesh->SetRelativeTransform(OffsetWithoutX); //임시로 주석처리
				//CameraFPS->FieldOfView = 75.f;
				//CameraFPS->SetWorldTransform(SightTransform);
			}
			break;
		}
		case EAimMode::EAM_NotAim:
		{
			bIsAim = false;
			EquippedWeapon->bIsAiming = false;
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

			if (CameraMode == ECameraMode::ECM_TPS)
			{
				//if (EquippedWeapon)
				//{
				//	GetCharacterMovement()->bOrientRotationToMovement = false;
				//	bUseControllerRotationYaw = true; //false; //3인칭에 Aim상태가 아니면, Yaw를 풀어준다.
				//}
				//else
				//{
				//	GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
				//	bUseControllerRotationYaw = false; //3인칭에 Aim상태가 아니면, Yaw를 풀어준다.
				//}



				//땡긴 카메라를 다시 원복 시킨다.
				/*float CurrentLength = CameraBoom->TargetArmLength;
				float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
				CameraBoom->TargetArmLength = BeforeCameraLength;
				CameraTPS->SetRelativeLocation(TPSCam_Rel_Location);
				CameraTPS->FieldOfView = BaseCamTPSfov;
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//FPMesh->bPauseAnims = false;
				//FPMesh->SetRelativeTransform(FPMeshOriginTransform);

				CameraFPS->FieldOfView = BaseCamFPSfov;
				
			}
			break;
		}
		default:
			break;
		}
	}
}


/****************************** 이동, 시점 관련 키 바인드 ***************************/

void AMainCharacter::TurnAtRate(float Rate)
{
	//매 delta초 마다 base rate만큼 회전
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::MoveForward(float Value)
{
	bMoveForward = false;
	if (MainController != NULL && Value != 0.f)
	{
		bMoveForward = true;
		//어느방향으로 갈지 찾고
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward벡터를 구한다.
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//그 방향으로 value만큼 간다.
		AddMovementInput(Direction, Value);
		/*UE_LOG(LogTemp, Warning, TEXT("my  ForVec = %s"), *Direction.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Actor::ForVec = %s"), *GetActorForwardVector().ToString());*/
	}
}

void AMainCharacter::MoveRight(float Value)
{
	bMoveRight = false;
	if (MainController != NULL && Value != 0.f)
	{
		bMoveRight = true;
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		//right 벡터
		FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
		/*UE_LOG(LogTemp, Warning, TEXT("my  RigVec = %s"), *Direction.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Actor::RigVec = %s"), *GetActorRightVector().ToString());*/
	}
}

void AMainCharacter::Sprint()
{
	//FTimerHandle Timer;
	if(GetWorldTimerManager().IsTimerActive(T_SprintKeyDown) == false &&
	bIsCrouched ==false && bDisableInput == false && GetCharacterMovement()->IsFalling() == false && bIsAim == false )
	{
		bSprintKeyDown = true;
		//Up Timer 초기화
		GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
		GetWorldTimerManager().SetTimer(T_SprintKeyDown,[&]
		{
			StatManagementComponent->StaminaManage(bSprintKeyDown);
		},GetWorld()->GetDeltaSeconds(),true);
	}
}
void AMainCharacter::UnSprint()
{
	if (GetWorldTimerManager().IsTimerActive(T_SprintKeyUp) == false)
	{
		bSprintKeyDown = false;
		//Down Timer 초기화
		GetWorldTimerManager().ClearTimer(T_SprintKeyDown);

		GetWorldTimerManager().SetTimer(T_SprintKeyUp, [&]
		{
				StatManagementComponent->StaminaManage(bSprintKeyDown);
		}, GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMainCharacter::ClearSprintUpTimer()
{
	GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
}


void AMainCharacter::MyJump()
{
	if (bDisableInput == false)
	{
		bIsJumpKeyDown = true;
		Super::Jump();
	}
}
void AMainCharacter::MyStopJumping()
{
	bIsJumpKeyDown = false;
	Super::StopJumping();
}

void AMainCharacter::MyCrouch()
{
	if (bDisableInput == false)
	{
		if (bCrouchToggle && bIsCrouched)
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: UnCrouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}

		if (bIsCrouched == false)
		{
			Super::Crouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: Crouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Crouch);
			SetCharacterStatus(ECharacterStatus::EPS_Crouch);
		}
	}
}

void AMainCharacter::MyUnCrouch()
{
	//Crouch를 Toggle로 사용하지 않을때만 Key Release  bind함수를 호출한다.
	if (bDisableInput == false)
	{
		if (bCrouchToggle == false && bIsCrouched)
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("UnCrouch:: UnCrouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}
	}
}

void AMainCharacter::Walk()
{
	if (bDisableInput == false)
	{
		if (bWalkToggle && bIsWalking && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: UnWalk"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
			//bIsWalking = false; -> Set enum함수에서 변경해줬다.
			return;
		}

		if (bIsWalking == false && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: Walk"));
			SetCharacterStatus(ECharacterStatus::EPS_Walk);
			//SetMainCharacterStatus(EPlayerStatus::EPS_Walk);
			//bIsWalking = true;
			return;
		}
	}
}

void AMainCharacter::UnWalk()
{
	if (bDisableInput == false)
	{
		if (bWalkToggle == false && bIsWalking == true && AimMode != EAimMode::EAM_Aim)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnWalk:: UnWalk"));
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			//bIsWalking = false;		
		}
	}
}

void AMainCharacter::ScrollDN()
{
	if (bDisableInput == false)
	{
		//TPS모드일 경우에만 스크롤 다운, 업이 가능하도록 (카메라 멀게 , 가까이)
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			if (CameraBoom->TargetArmLength <= MINCameraLength)
			{
				CameraBoom->TargetArmLength = MINCameraLength;
			}
			else CameraBoom->TargetArmLength -= 25;
		}
	}
}

void AMainCharacter::ScrollUP()
{
	if (bDisableInput == false)
	{
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			if (CameraBoom->TargetArmLength >= MAXCameraLength)
			{
				CameraBoom->TargetArmLength = MAXCameraLength;
			}
			else CameraBoom->TargetArmLength += 25;
		}
	}
}

void AMainCharacter::VKeyDN()
{
	if (bDisableInput == false)
	{
		switch (CameraMode)
		{
		case ECameraMode::ECM_TPS:
			SetCameraMode(ECameraMode::ECM_FPS);
			break;
		case ECameraMode::ECM_FPS:
			SetCameraMode(ECameraMode::ECM_TPS);
			break;
		default:
			break;
		}
	}
}


/************** Interactive & Inventory Key bind 함수 ***********/
void AMainCharacter::LMBDown()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StartFire();
		}
	}
}

void AMainCharacter::LMBUp()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StopFire();
		}
	}
}

void AMainCharacter::RMBDown()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon != nullptr)
		{
			if (EquippedWeapon->bIsHighReady == false && bIsSprinting == false)
			{
				if (bAimToggle && bIsAim)
				{
					SetAimMode(EAimMode::EAM_NotAim);
					UE_LOG(LogTemp, Warning, TEXT("Aim off"));
				}
				else
				{
					SetAimMode(EAimMode::EAM_Aim);
					UE_LOG(LogTemp, Warning, TEXT("Aim on"));
				}
			}
		}
	}
}

void AMainCharacter::RMBUp()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon != nullptr)
		{
			if (bAimToggle == false)
			{
				SetAimMode(EAimMode::EAM_NotAim);
				UE_LOG(LogTemp, Warning, TEXT("Aim off"));
			}
		}
	}
}


void AMainCharacter::TabKeyDown()
{
	bTabKeyDown = true;
	UE_LOG(LogTemp, Warning, TEXT("Tab key down"));

	MainController->ToggleInventory();
	OnGetAmmo.Broadcast(EquippedWeapon);
}

void AMainCharacter::EKeyDown()
{
	if (bDisableInput == false)
	{
		Interactive();
	}
}

/*************************  Weapon, Item 관련 ***************************************************/
void AMainCharacter::ChangeWeapon(int32 index)
{
	Super::ChangeWeapon(index);

	//FPMode의 AnimUpdate를 위해 갱신한다.
	switch (index)
	{
	case 0:
		FPAnimInstance->WeaponTypeNumber = 0;
		break;
	case 1:
		if (PrimaryWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
		}
		break;
	case 2:
		if (SubWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
		}
		break;
	case 3:
		if (PistolWeapon)
		{
			FPAnimInstance->WeaponTypeNumber = 2;			
		}
		break;
	}

	//For WeaponStatus Widget
	OnChangeWeapon.Broadcast(EquippedWeapon);
	OnGetAmmo.Broadcast(EquippedWeapon);
}

/*************************  Interaction 관련 ***************************************************/

/* Static을 감지해서 FHitResult를 리턴 */
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(10.f, 10.f); //살짝 넓게 해서 탐지가 쉽게 했다.
	Params.AddIgnoredActor(this);

	//SweepSingle로 변경, 캡슐형태의 모양으로 LineTrace.	
	GetWorld()->SweepSingleByChannel(Hit, StartLo, EndLo, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, Shape, Params); 
	
	/* debug */
	//DrawDebugLine(GetWorld(), StartLo, EndLo, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
	//DrawDebugCapsule(GetWorld(), (StartLo+EndLo)/2, Shape.GetCapsuleHalfHeight(),Shape.GetCapsuleRadius(),FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Green, false, 0.8f, (uint8)nullptr, 2.f);

	return Hit;
}


void AMainCharacter::SetInteractActor(AActor* Actor)
{
	InteractActor = Actor;

	IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
	if (InterfaceActor)
	{
		InterfaceActor->SetOutline();

		//InteractActor의 Static Mesh의 위치를 기준으로 거리를 구한다.
		if((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
		{
			MainController->ControlInteractText(true);
		}

		/* debug */
		//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Valid"));
	}			
}

void AMainCharacter::UnsetInteractActor()
{
	if (InteractActor)
	{
		IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
		if(InterfaceActor)
		{
			InterfaceActor->UnsetOutline();
		}
		InteractActor = nullptr;
		MainController->ControlInteractText(false);
	}
	/* debug */
	//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Invalid"));
}

void AMainCharacter::Interactive()
{
	if (InteractActor)
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(InteractActor);
		if (Interface)
		{
			if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
			{

				Interface->Interaction(this);
			}
		}
	}
	
}



void AMainCharacter::QuickSlotNum4()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N4);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N4);
}
void AMainCharacter::QuickSlotNum5()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N5);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N5);
}
void AMainCharacter::QuickSlotNum6()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N6);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N6);
}
void AMainCharacter::QuickSlotNum7()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N7);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N7);
}
void AMainCharacter::QuickSlotNum8()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N8);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N8);
}
void AMainCharacter::QuickSlotNum9()
{
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N9);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N9);
}