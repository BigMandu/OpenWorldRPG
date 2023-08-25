// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/UI/ResourceStatusWidgetInInventory.h"
#include "OpenWorldRPG/UI/ResourceStatusWidget.h"
#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/CompassWidget.h"
#include "OpenWorldRPG/UI/CharacterOverviewWidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"

#include "Components/SphereComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h" //디버깅용
#include "AI/EnemyCharacter.h"


// Sets default values
AMainCharacter::AMainCharacter() : Super()
{
	GetCharacterMovement()->bOrientRotationToMovement = true; //움직인 방향 = 진행방향으로 설정
	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //웅크리기 키 Toggle을 true로 기본세팅 한다.
	bWalkToggle = true; //걷기 키 Toggle을 true로 세팅한다.
	bAimToggle = true; //조준키를 toggel true세팅.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;
	ActiveInteractDistance = 200.f; //상호작용 아이템이 표시되는 최대거리.


	//Character Render Image는 안하기로 함. 여기에 Mission widget을 넣기로 결정.
	/*CharacterCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CharacterCaptureComp"));
	CharacterCapture->SetupAttachment(RootComponent);*/

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
	CameraFPS->SetOrthoNearClipPlane(0.f);

	CharacterMaskClothes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MaskClothes"));
	CharacterMaskClothes->SetupAttachment(GetRootComponent());

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowMesh"));
	ShadowMesh->SetupAttachment(GetRootComponent());

	CraftSysComp = CreateDefaultSubobject<UCraftSystemComponent>(TEXT("CraftSysComp"));

	if (AOpenWorldRPGGameModeBase* Mode = Cast<AOpenWorldRPGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		Gmode = Mode;
	}
}


void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
	/* Player의 AnimInstance를 불러온다. */
	//TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); 
	FPAnimInstance = Cast<UMainAnimInstance>(FPMesh->GetAnimInstance());


	//ShadowMesh의 AnimInstance를 넣어준다. -> 이거 바꿔야함 NoSound로 ㅇㅇ (주석처리함)
	//ShadowMesh->SetAnimInstanceClass(GetMesh()->GetAnimClass());

	ShadowMesh->SetOnlyOwnerSee(true);
	ShadowMesh->SetRenderInMainPass(false);
	ShadowMesh->SetHiddenInGame(true);

	ChangeWeaponTypeNumber(0);
	//ShadowMesh->SetOwnerNoSee(true);
	//ShadowMesh->SetCastShadow(true);

	//ShadowMesh->SetHiddenInGame(true);
	//ShadowMesh->SetCastHiddenShadow(true);

	CameraBoom->SetRelativeLocation(FVector(0.f, 40.f, 80.f));
	CameraFPS->SetRelativeLocation(FVector(20.136719, -2.999939, 68.834351));
	FPMesh->SetRelativeLocation(FVector(-15.128922, -3.533375, -150.495850));

	FPMeshOriginTransform = FPMesh->GetRelativeTransform();
	FPMesh->SetHiddenInGame(true);
	FPMesh->SetCastShadow(false);
	GetMesh()->SetCastShadow(false);


	/* Mask의류 입기 */
	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,true);
	CharacterMaskClothes->AttachToComponent(GetMesh(),Rules, MaskSocketName);
	CharacterMaskClothes->SetRelativeTransform(FTransform(FRotator(-90.f, 0.f,0.f), FVector(-168.f, 2.3f, 0.f)));


	/* FPS Aim관련 기본 설정 값 저장*/
	BaseCamTPSfov = CameraTPS->FieldOfView;
	BaseCamFPSfov = CameraFPS->FieldOfView;
	BaseFPMeshTransform = FPMesh->GetRelativeTransform();


	//GetCapsuleComponent()->SetCollisionProfileName(FName("MainChar"));
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	//GetCapsuleComponent()->SetCollisionProfileName("MainChar");
	//GetCapsuleComponent()->SetCollisionProfileName("MainChar");

	//FPMesh->HideBoneByName(FName("spine_02"));
	/*FPLowerLegMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_l"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_r"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->SetHiddenInGame(true);
	FPLowerLegMesh->SetCastShadow(false);
	FPLowerLegMesh->SetOnlyOwnerSee(true);
	FPLowerLegMesh->SetReceivesDecals(false);*/


	CompassBeginUse.AddDynamic(this, &AMainCharacter::CompassStart);
	CompassEndUse.AddDynamic(this, &AMainCharacter::CompassStop);




}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	/********** 초기 세팅 ************/
	SetCameraMode(ECameraMode::ECM_TPS); //초기 카메라 모드는 3인칭 모드로.
	SetAimMode(EAimMode::EAM_NotAim);

	MainController = Cast<AMainController>(GetController());

	if(!MainController ) return;
	/****** Main Hud의 Widget과 Event Bind ********/
	UMainHud* MainHud = Cast<UMainHud>(MainController->MainHud);
	if ( MainHud )
	{

		if ( StatManagementComponent )
		{
			MainHud->ResourceWidget->BindStatManager(StatManagementComponent);
			MainHud->NewInventoryWidget->ResourceStatusWidget->BindStatManager(StatManagementComponent);
			MainHud->NewInventoryWidget->CharacterOverviewWidget->BindStatManager(StatManagementComponent);
		}


		if ( MainHud->WeaponStatus )
		{
			MainHud->WeaponStatus->BindWeaponWidget(this);
		}

		if ( MainHud->QuickSlot )
		{
			MainHud->QuickSlot->BindSlotWidget(this);
		}
	}
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!MainController) return;

	//bIsLookInput 세팅 (Aim Init을 위한것.)
	float turnvalue = GetInputAxisValue("Turn");
	float lookvalue = GetInputAxisValue("LookUp");
	if ( turnvalue == 0.f && lookvalue == 0.f )
	{
		bIsLookInput = false;
	}
	else
	{
		bIsLookInput = true;
	}

	//this bool toggle at MainController, when GetIn/Out of the car.
	if ( bDisableInteractionLineTrace == false )
	{
		InteractionLineTrace();
	}

	/* Weapon의 AimInitialize함수에서 사용하는 Timer 해제용 */
	if ( EquippedWeapon )
	{
		//타이머가 작동중이고
		if ( GetWorldTimerManager().IsTimerActive(EquippedWeapon->AimInitHandle) )
		{
			//aim을 돌리거나, AlphaTime이 1초가 넘어가거나, 쏘는 중이라면
			if ( bIsLookInput || EquippedWeapon->AlphaTime >= 1.f || EquippedWeapon->bIsFiring )
			{
				//에임 초기화 타이머를 초기화 시킨다.
				UE_LOG(LogTemp, Warning, TEXT("MainChar:: Clear AimInit Timer"));
				GetWorldTimerManager().ClearTimer(EquippedWeapon->AimInitHandle);
			}
		}
		if ( EquippedWeapon->RecoilAlphaTime >= 1.f )
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

	PlayerInputComponent->BindAction("TacticalToggle", IE_Pressed, this, &ABaseCharacter::ToggleTacticalEquip); //F key
	PlayerInputComponent->BindAction("ChangeSafetyLever", IE_Pressed, this, &ABaseCharacter::ChangeSafetyLever); //T key
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::ReloadWeapon); //R key

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::MyStopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::MyCrouch); //C key
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::MyUnCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::Walk); //Ctrl key
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::UnWalk);


	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Sprint); //Shift key
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


	/***************** Test save and load *********************/
	
	

}


void AMainCharacter::InteractionLineTrace()
{
	/* 상호작용 Line Trace 변경 . ver2  아래 코드를 GetPlayerViewPoint함수를 이용해 간략화 시켰다. */
	FRotator Rot;
	MainController->GetPlayerViewPoint(Interact_LineTrace_StartLocation, Rot);
	switch ( CameraMode )
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * ( MAXCameraLength + 800.f ); //카메라Boom길이보다 더길게 끝나야한다.
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


	/* 상호작용 텍스트를 띄움 */
	/* Static FHitResult를 리턴받아서 Interface 변환, 성공하면 Outline과 TEXT를 띄움.*/
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();

	//AInteractable* HitInteractActor = Cast<AInteractable>(HitActor);
	IInteractive_Interface* HitInteractActor = Cast<IInteractive_Interface>(HitActor);
	if ( HitInteractActor )// && HitInteractActor->bCanNotInteractable == false)
	{
		if ( InteractActor ) //이미 InteractActor가 있고
		{
			if ( InteractActor != HitActor )//HitInteractActor) //지금 보고 있는 Actor와 InteractActor가 다르면
			{
				UnsetInteractActor(); //InteractActor를 Unset해준다.
			}
			else
			{
				//거리에 따라 InteractText가 업데이트 되기 때문에 추가 해준다.
				SetInteractActor(HitActor);//HitInteractActor);
			}
		}
		else //InteractActor가 없을 경우
		{
			SetInteractActor(HitActor);//HitInteractActor); //Interactable인 경우 Set을 해준다.
		}
	}
	else
	{
		UnsetInteractActor();
	}
	//InteractActor가 다를경우 UnsetInteractActor 호출하자. (Outline이 안없어지는 버그가 있음.) -> 해결함.
}

/*
void AMainCharacter::FPSAimLocationAdjust()
{
	if ( CameraFPS )
	{
		if ( bIsAim )
		{
			//각 총마다 FPMesh의 올바른 위치에 부착하기 위해
			//Weapon Class에 Transform 변수를 만들어줬다.
			const FTransform NewFPMeshTransform = EquippedWeapon->CharFPMeshTransform;
			const FTransform NewWeaponTransform = EquippedWeapon->WeapSKMeshTransform;
			CameraFPS->SetFieldOfView(70.f);
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
*/

//플레이어 시점 상태 -> VKeyDN
void AMainCharacter::SetCameraMode(ECameraMode Type)
{
	CameraMode = Type;
	switch ( CameraMode )
	{
	case ECameraMode::ECM_TPS:
		CameraTPS->Activate();
		CameraFPS->Deactivate();

		/* 회전시 카메라에만 영향 가도록 설정 */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		/* Character Mesh 설정 */
		FPMesh->SetHiddenInGame(true);  //1인칭 Mesh 숨김
		//GetMesh()->SetHiddenInGame(false); //3인칭 Mesh 안숨김
		GetMesh()->SetCastShadow(true);
		GetMesh()->UnHideBoneByName(FName("spine_01")); //spine_02
		ShadowMesh->SetHiddenInGame(true);


		if ( bIsUsingCompass )
		{
			MainController->ControlCoreUsableWidget(true);
		}

		UCustomInventoryLibrary::ShowAllEquipment(Equipment);
		CharacterMaskClothes->SetRenderInMainPass(true);

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
		GetMesh()->HideBoneByName(FName("spine_01"), EPhysBodyOp::PBO_None);
		ShadowMesh->SetHiddenInGame(false);
		GetMesh()->SetCastShadow(false);

		//bRenderInMainPass를 false로 하게되면 Rendering은 되지 않지만, Shadow는 렌더링 된다.
		// 이걸 사용하려면 Mesh가 Show되어야 한다. SetHiddenInGame을 True로 하면 안됨.	
		//GetMesh()->SetRenderInMainPass(false);

		if ( bIsUsingCompass )
		{
			MainController->ControlCoreUsableWidget(false);
		}

		//장착중인 장비를 모두 hide한다.
		UCustomInventoryLibrary::HideAllEquipment(Equipment);
		CharacterMaskClothes->SetRenderInMainPass(false);

		break;
	}

	/* 장착 무기 Mesh에 부착 */
	if ( EquippedWeapon )
	{
		EquippedWeapon->GunAttachToMesh(this);
	}
}

//RMB Down
void AMainCharacter::SetAimMode(EAimMode Mode)
{
	Super::SetAimMode(Mode);
	//AimMode = Mode;
	if ( EquippedWeapon == nullptr ) return;

	//Idle 상태가 아니면 ADS를 하지 못하게 막는다.
	if ( EquippedWeapon->CurrentWeaponState != EWeaponState::EWS_Idle )
	{
		if (Gmode)
		{
			Gmode->ShowSystemNotiMessage(ESystemNotificationMessageType::ESNMT_WeaponIsBusy);
		}
		return;
	}

	switch ( AimMode )
	{
	case EAimMode::EAM_Aim:
	{			
		if ( CameraMode == ECameraMode::ECM_TPS )
		{			
			LerpCamera(CameraTPS, MINCameraLength + 20.f, TPSCam_Aim_Rel_Location, 77.f);
		}
		else if ( CameraMode == ECameraMode::ECM_FPS )
		{
			//idle anim에 의해 Socket의 WorldTF값이 틀어지는걸 방지하기 위해 정지Anim을 재생한다.
			if ( EquippedWeapon->WeaponDataAsset->WeaponAnimaton.FPS_ADS_Anim )
			{
				//sFPMesh->bPauseAnims = true;
				//below is using Blend.
				FPAnimInstance->Montage_Play(EquippedWeapon->WeaponDataAsset->WeaponAnimaton.FPS_ADS_Anim, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
				FPAnimInstance->Montage_JumpToSection(FName("ADS"), EquippedWeapon->WeaponDataAsset->WeaponAnimaton.FPS_ADS_Anim);
				LerpCamera(CameraFPS, 60.f);
			}
		}
		break;



	}
	case EAimMode::EAM_NotAim:
	{
		if ( CameraMode == ECameraMode::ECM_TPS )
		{
			LerpCamera(CameraTPS, MAXCameraLength - 20.f, TPSCam_Rel_Location, BaseCamTPSfov);
		}
		else if ( CameraMode == ECameraMode::ECM_FPS )
		{
			//FPAnimInstance->StopAllMontages(0.f);//Montage_Stop( (0.f);
			FPSnotADS();
			LerpCamera(CameraFPS, BaseCamFPSfov);
			FPMesh->bPauseAnims = false;
			//FPMesh->SetRelativeTransform(FPMeshOriginTransform);
			//CameraFPS->FieldOfView = BaseCamFPSfov;
				
		}
		break;
	}
	default:
		break;
	}
	
}

//called from StartFPADS class (Anim Notify)
void AMainCharacter::FPSADS()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainCharacter::FPSADS called"));

	//FPMesh (Arm Mesh)가 이동할 위치값을 가져온다.
	FTransform ADSPos = GetFpsAdsPosition();

	ADSTime = 0.f;
	ADSAlphaTime = 0.f;
	//FPMesh->bPauseAnims = true;
	
	GetWorldTimerManager().SetTimer(T_ADSTimerHandle, [=] {
		FTransform OriginPos = FPMesh->GetRelativeTransform();

	//FPMesh를 중앙으로 옮기는 시간을 0.8초로 정한다.
	ADSTime += GetWorld()->GetDeltaSeconds();
	ADSAlphaTime = ADSTime / .8f; //.8f;

	FTransform LerpPos = UKismetMathLibrary::TLerp(OriginPos, ADSPos, ADSAlphaTime,ELerpInterpolationMode::QuatInterp);

	//FPMesh를 해당 Offset만큼 이동, Camera의 중앙에 오도록 한다.
	FPMesh->SetRelativeTransform(LerpPos);//OffsetWithoutX);
	UE_LOG(LogTemp,Warning,TEXT("AMainCharacter::FPSADS // ADSTime = %f, ADSAlphaTime = %f"),ADSTime,ADSAlphaTime);
	//LerpCamera 함수와 시간을 맞춰주자.
	if ( FPMesh->GetRelativeTransform().Equals(ADSPos) || ADSAlphaTime >= 0.99f)//0.4f )
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainCharacter::FPSADS Time 0.99f!!!!~!!! Try ClearTimer // ADSAlphaTime = %f"), ADSAlphaTime);
		FPMesh->bPauseAnims = false;
		//FPMesh->SetRelativeTransform(ADSPos);
		//FPAnimInstance->StopAllMontages(0.1f);
		GetWorldTimerManager().ClearTimer(T_ADSTimerHandle);

	}

		}, GetWorld()->GetDeltaSeconds(), true);

	//모든 세팅이 끝나면 AnimMontage를 해제해서 Locomotion Anim을 이어나가도록 한다.
	FPMesh->bPauseAnims = true;
	/*FPAnimInstance->StopAllMontages(0.1f);
	*/
	
	//CameraFPS->SetWorldTransform(SightTransform);
}

void AMainCharacter::FPSnotADS()
{
	ADSTime = 0.f;
	ADSAlphaTime = 0.f;


	GetWorldTimerManager().SetTimer(T_ADSTimerHandle, [ = ] {
		FTransform CurrentFPPos = FPMesh->GetRelativeTransform();
		//LerpCamera 함수와 시간을 맞춰주자.
	if ( FPMesh->GetRelativeTransform().Equals(FPMeshOriginTransform) || ADSAlphaTime >= 0.99f )
	{
		GetWorldTimerManager().ClearTimer(T_ADSTimerHandle);
	}
	ADSTime += GetWorld()->GetDeltaSeconds();
	ADSAlphaTime = ADSTime / .8f;

	FTransform LerpPos = UKismetMathLibrary::TLerp(CurrentFPPos, FPMeshOriginTransform, ADSAlphaTime);


	FPMesh->SetRelativeTransform(LerpPos);//OffsetWithoutX);

	}, GetWorld()->GetDeltaSeconds(), true);
}

//FPMesh가 이동할 위치를 구한다.
FTransform AMainCharacter::GetFpsAdsPosition()
{

	UE_LOG(LogTemp, Warning, TEXT("AMainCharacter::GetADSPosition"));

	FTransform FPMeshWorld = FPMesh->GetComponentTransform();

	//EquippedWeapon에서 SightSocket을 Get하는 함수를 호출한다.
	FTransform SightTransform = EquippedWeapon->GetSightSocketTransform();


	//SightSocket에서  FPMesh의 Relative Transform을 구한다.
	//이렇게 구한 Offset을 Mesh의 Transform에 적용한다.

	/**explain Make Relative Transform
	 * Example: ChildOffset = MakeRelativeTransform(Child.GetActorTransform(), Parent.GetActorTransform())
	 * This computes the relative transform of the Child from the Parent.
	 */
	FTransform Offset = FPMeshWorld.GetRelativeTransform(SightTransform);//.Inverse();

	//DrawDebugSphere(GetWorld(),Offset.GetTranslation(),6.f,6, FColor::Cyan,false,10.f,0,3.f);
	// Weapon의 Clipping을 위한 Trace를 FPMesh일때 Aim, NotAim을 보완하기 위해 Aim일때 Mesh를 앞으로 좀 나가게 한다.
	// (기존 Aim때는 FPMesh가 뒤로 들어가버려 Clipping문제가 안일어 났음.)
	//FTransform OffsetWithoutX = FTransform(Offset.GetRotation(), FVector(-10.f, Offset.GetTranslation().Y, Offset.GetTranslation().Z));

	return Offset;//OffsetWithoutX;
}

void AMainCharacter::LerpCamera(UCameraComponent* VarTPScam, float TargetBoomLength, FVector TargetCamRelativeLocation, float TargetFOV)
{
	CamTime = 0.f;
	CamAlphaTime = 0.f;
	GetWorldTimerManager().ClearTimer(T_CameraHandle);

	GetWorldTimerManager().SetTimer(T_CameraHandle, [ = ] {
		FVector CamCurrentLo = VarTPScam->GetRelativeLocation();
	float CamCurrentFOV = VarTPScam->FieldOfView;
	float  BoomCurrentLen = CameraBoom->TargetArmLength;

	CamTime += GetWorld()->GetDeltaSeconds();
	//ADS함수와 시간을 맞춰주자.
	CamAlphaTime = CamTime / .8f;

	FVector SetCamLo = UKismetMathLibrary::VLerp(CamCurrentLo, TargetCamRelativeLocation, CamAlphaTime);
	float SetFOV = UKismetMathLibrary::Lerp(CamCurrentFOV, TargetFOV, CamAlphaTime);
	float SetBomLength = UKismetMathLibrary::Lerp(BoomCurrentLen, TargetBoomLength, CamAlphaTime);

	VarTPScam->SetRelativeLocation(SetCamLo);
	VarTPScam->SetFieldOfView(SetFOV);
	CameraBoom->TargetArmLength = SetBomLength;

	if ( CamAlphaTime >= 1.0f )
	{
		GetWorldTimerManager().ClearTimer(T_CameraHandle);
	}

		}, GetWorld()->GetDeltaSeconds(), true);
}

void AMainCharacter::LerpCamera(UCameraComponent* VarFPScam, float TargetFOV)
{
	CamTime = 0.f;
	CamAlphaTime = 0.f;
	GetWorldTimerManager().ClearTimer(T_CameraHandle);

	GetWorldTimerManager().SetTimer(T_CameraHandle, [ = ] {
		float CamCurrentFOV = VarFPScam->FieldOfView;

	CamTime += GetWorld()->GetDeltaSeconds();
	//ADS함수와 시간을 맞춰주자.
	CamAlphaTime = CamTime / .8f;
	float SetFOV = UKismetMathLibrary::Lerp(CamCurrentFOV, TargetFOV, CamAlphaTime);

	VarFPScam->SetFieldOfView(SetFOV);

	if ( CamAlphaTime >= 1.0f )
	{
		GetWorldTimerManager().ClearTimer(T_CameraHandle);
	}

		}, GetWorld()->GetDeltaSeconds(), true);
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
	bMovingStraightForward = false;
	if ( MainController != NULL && Value != 0.f )
	{
		bMoveForward = true;
		/* for Sprint*/
		if ( Value > 0.f )
		{
			bMovingStraightForward = true;
		}
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
	//bBlockMoveRight는 Sprint함수에서 제어한다.
	if ( MainController != NULL && Value != 0.f && bBlockMoveRight == false)
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
	//UE_LOG(LogTemp, Warning, TEXT("bMovingStraightForward : %s"), bMovingStraightForward ? "True" : "False");
}

void AMainCharacter::Sprint()
{
	//FTimerHandle Timer;
	if (CanSprint())
	{
		Super::Sprint();

		FPAnimInstance->bIsSprint = true;
		//HandIK를 해제한다.
		SetLeftHandIK(0.f);
		//FPAnimInstance->SetLeftHandIKAlpha(0.f);

		bSprintKeyDown = true;

		//좌우 회전 민감도를 확 낮춘다.
		TempVar_OriginInputYawScale = MainController->InputYawScale;
		TempVar_OriginInputPitchScale = MainController->InputPitchScale;

		MainController->InputYawScale = 0.1f;
		MainController->InputPitchScale = -0.1f;

		//Sprint중에 MoveRight를 막아준다.
		bBlockMoveRight = true;
		
		//Up Timer 초기화
		GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
		GetWorldTimerManager().SetTimer(T_SprintKeyDown, [ & ]
		{
			if(CanSprint())
			{
				StatManagementComponent->StaminaManage(bSprintKeyDown);
			}
			else
			{
				UnSprint();
			}
			
		}, GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMainCharacter::UnSprint()
{
	if ( GetWorldTimerManager().IsTimerActive(T_SprintKeyUp) == false )
	{
		Super::UnSprint();
		FPAnimInstance->bIsSprint = false;
		//장착한 EquippedWeapon이 있다면 HandIK를 재설정한다.
		if ( EquippedWeapon )
		{
			SetLeftHandIK(1.f);
			//FPAnimInstance->SetLeftHandIKAlpha(1.f);
		}
		
		//FPAnimInstance->LeftHandAlpha = 1.f;

		//TPAnimInstance->bIsSprint = false;
		bSprintKeyDown = false;
		bBlockMoveRight = false;

		//좌우 회전 민감도를 기본값으로 돌려놓는다.
		MainController->InputYawScale = TempVar_OriginInputYawScale;
		MainController->InputPitchScale = TempVar_OriginInputPitchScale;

		//Down Timer 초기화
		GetWorldTimerManager().ClearTimer(T_SprintKeyDown);

		GetWorldTimerManager().SetTimer(T_SprintKeyUp, [ & ]
		{
			StatManagementComponent->StaminaManage(bSprintKeyDown);
		}, GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMainCharacter::ClearSprintUpTimer()
{
	GetWorldTimerManager().ClearTimer(T_SprintKeyUp);
}

bool AMainCharacter::CanSprint()
{
	if ( bIsCrouched == false && bDisableInput == false && GetCharacterMovement()->IsFalling() == false && bIsAim == false
&& bMovingStraightForward)
	{
		if ( EquippedWeapon && EquippedWeapon->CurrentWeaponState != EWeaponState::EWS_Idle )
		{
			return false;
		}

		return true;
	}
	return false;
}

void AMainCharacter::MyJump()
{	
	if ( bDisableInput == false )
	{
		Super::MyJump();
		
		FPAnimInstance->bIsJumpkeyDown = true;
		Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance())->bIsJumpkeyDown = true;
		//Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance())->bIsJumpkeyDown = true;
		Super::Jump();
	}
}
void AMainCharacter::MyStopJumping()
{	
	FPAnimInstance->bIsJumpkeyDown = false;
	Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance())->bIsJumpkeyDown = false;
	//Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance())->bIsJumpkeyDown = false;

	Super::MyStopJumping();
	
}

void AMainCharacter::MyCrouch()
{
	if ( bDisableInput == false )
	{
		if ( bCrouchToggle && bIsCrouched )
		{
			Super::UnCrouch();
			UE_LOG(LogTemp, Warning, TEXT("Crouch:: UnCrouch"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
		}

		if ( bIsCrouched == false )
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
	if ( bDisableInput == false )
	{
		if ( bCrouchToggle == false && bIsCrouched )
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
	if ( bDisableInput == false )
	{
		if ( bWalkToggle && bIsWalking && AimMode != EAimMode::EAM_Aim )
		{
			UE_LOG(LogTemp, Warning, TEXT("Walk:: UnWalk"));
			//SetMainCharacterStatus(EPlayerStatus::EPS_Normal);
			SetCharacterStatus(ECharacterStatus::EPS_Normal);
			//bIsWalking = false; -> Set enum함수에서 변경해줬다.
			return;
		}

		if ( bIsWalking == false && AimMode != EAimMode::EAM_Aim )
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
	if ( bDisableInput == false )
	{
		if ( bWalkToggle == false && bIsWalking == true && AimMode != EAimMode::EAM_Aim )
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
	if ( bDisableInput == false )
	{
		//TPS모드일 경우에만 스크롤 다운, 업이 가능하도록 (카메라 멀게 , 가까이)
		if ( CameraMode == ECameraMode::ECM_TPS )
		{
			if ( CameraBoom->TargetArmLength <= MINCameraLength )
			{
				CameraBoom->TargetArmLength = MINCameraLength;
			}
			else CameraBoom->TargetArmLength -= 25;
		}
	}
}

void AMainCharacter::ScrollUP()
{
	if ( bDisableInput == false )
	{
		if ( CameraMode == ECameraMode::ECM_TPS )
		{
			if ( CameraBoom->TargetArmLength >= MAXCameraLength )
			{
				CameraBoom->TargetArmLength = MAXCameraLength;
			}
			else CameraBoom->TargetArmLength += 25;
		}
	}
}

void AMainCharacter::VKeyDN()
{
	if ( bDisableInput == false )
	{
		switch ( CameraMode )
		{
		case ECameraMode::ECM_TPS:
			SetCameraMode(ECameraMode::ECM_FPS);
			//vkeydown으로 시점을 변경했다면, 기존Aimmode를 그대로 넣어 Fps/Tps를 다시 세팅한다.
			SetAimMode(AimMode);
			break;
		case ECameraMode::ECM_FPS:
			SetCameraMode(ECameraMode::ECM_TPS);
			SetAimMode(AimMode);

			break;
		}

		if ( HoldingItem )
		{
			ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
			if ( Grenade )
			{
				//false로 넘겨서 이미 들고 있는 Item을 Destory못하게 한다.
				Grenade->AttachToHand(this, HoldingItem->ItemObj, false);
			}
			else
			{
				ReAttachHoldingItem();
			}
		}
			
	}
}


/************** Interactive & Inventory Key bind 함수 ***********/
void AMainCharacter::LMBDown()
{
	if ( bDisableInput == false )
	{
		if ( EquippedWeapon )
		{
			EquippedWeapon->StartFire();
		}
		else if ( HoldingItem )
		{
			ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
			if ( Grenade )
			{
				Grenade->ReadyToThrow();
			}
		}
	}
}

void AMainCharacter::LMBUp()
{
	if ( bDisableInput == false )
	{
		if ( EquippedWeapon )
		{
			EquippedWeapon->StopFire();
		}
		else if ( HoldingItem )
		{
			ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
			if ( Grenade )
			{
				Grenade->ThrowGrenade(this);
			}

		}
	}
}



void AMainCharacter::RMBDown()
{
	if ( bDisableInput == false )
	{
		if ( EquippedWeapon != nullptr )
		{
			if ( EquippedWeapon->bIsHighReady == false && bIsSprinting == false )
			{
				if ( bAimToggle && bIsAim )
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
	if ( bDisableInput == false )
	{
		if ( EquippedWeapon != nullptr )
		{
			if ( bAimToggle == false )
			{
				SetAimMode(EAimMode::EAM_NotAim);
				UE_LOG(LogTemp, Warning, TEXT("Aim off"));
			}
		}
	}
}


void AMainCharacter::TabKeyDown()
{
	if(bIsDie) return;

	bTabKeyDown = true;
	UE_LOG(LogTemp, Warning, TEXT("Tab key down"));

	MainController->ToggleInventory();

	OnGetAmmo.Broadcast(EquippedWeapon);
}

void AMainCharacter::EKeyDown()
{
	if ( bDisableInput == false && bIsDie == false)
	{
		Interactive();
	}
}

void AMainCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	Super::SetEquippedWeapon(Weapon);
	if ( Weapon )
	{
		SetLeftHandIK(1.f);
		//FPAnimInstance->SetLeftHandIKAlpha(1.f);
	}
	else
	{
		SetLeftHandIK(0.f);
		FPAnimInstance->SetLeftHandIKAlpha(0.f);
	}
	
}

void AMainCharacter::ChangeWeaponTypeNumber(int32 number)
{
	Super::ChangeWeaponTypeNumber(number);
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	if ( FPAnimInstance == nullptr || ShadowAnim == nullptr) return;

	FPAnimInstance->SetWeaponTypeNumber(number); //WeaponTypeNumber = number;
	ShadowAnim->SetWeaponTypeNumber(number);// WeaponTypeNumber = number;
	UE_LOG(LogTemp,Warning,TEXT("AMainChar::ChangeWeaponTypeNum"));
}
/*******************************  Sound 관련 ************************************************/

void AMainCharacter::StepSound()
{
	if (this->GetMesh() != ShadowMesh)
	{
		Super::StepSound();
	}
}
void AMainCharacter::SpeakSound(USoundCue* Sound)
{
	if (this->GetMesh() != ShadowMesh)
	{
		Super::SpeakSound(Sound);
	}
}
void AMainCharacter::PlayReloadSound(EPlayReloadSound ReloadSoundType)
{
	if (this->GetMesh() != ShadowMesh)
	{
		Super::PlayReloadSound(ReloadSoundType);
	}
}

/*************************  Weapon, Item 관련 ***************************************************/

void AMainCharacter::PlayAnimation(UAnimMontage* TPAnim, UAnimMontage* FPAnim)
{
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	if(!TPAnim || !FPAnim || !FPAnimInstance || !ShadowAnim) return;	
	
	Super::PlayAnimation(TPAnim, nullptr);
	ShadowAnim->Montage_Play(TPAnim);
	FPAnimInstance->Montage_Play(FPAnim);
	
}

void AMainCharacter::StopAnimation()
{
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	if ( !TPAnimInstance || !FPAnimInstance || !ShadowAnim ) return;
		
	UAnimMontage* TPMontageToStop = GetCurrentMontage();
	UAnimMontage* FPMontageToStop = FPAnimInstance->GetCurrentActiveMontage();
	if ( FPMontageToStop && TPMontageToStop )
	{
		ShadowAnim->Montage_Stop(TPMontageToStop->BlendOut.GetBlendTime(), TPMontageToStop);
		FPAnimInstance->Montage_Stop(FPMontageToStop->BlendOut.GetBlendTime(), FPMontageToStop);
		Super::StopAnimation();
	}
}

//CameraMode에 따라 HoldingItem을 Attach시킨다.
void AMainCharacter::ReAttachHoldingItem()
{
	if ( HoldingItem == nullptr ) return;

	const USkeletalMeshSocket* HandSocket = nullptr;
	FTransform RelativeTF;
	USkeletalMeshComponent* OnMesh = nullptr;

	if ( CameraMode == ECameraMode::ECM_FPS )
	{
		OnMesh = FPMesh;
		if ( HoldingItem->ItemSetting.DataAsset->bAttachedToLeftHand )
		{
			HandSocket = FPMesh->GetSocketByName(LeftHandGripSocketName);
		}
		else
		{
			HandSocket = FPMesh->GetSocketByName(GripSocketName);
		}
		
		RelativeTF = HoldingItem->ItemSetting.DataAsset->FPS_HandAttachTransform;
	}
	else
	{
		OnMesh = GetMesh();
		if ( HoldingItem->ItemSetting.DataAsset->bAttachedToLeftHand )
		{
			HandSocket = GetMesh()->GetSocketByName(LeftHandGripSocketName);
		}
		else
		{
			HandSocket = GetMesh()->GetSocketByName(GripSocketName);
		}
		
		RelativeTF = HoldingItem->ItemSetting.DataAsset->TPS_HandAttachTransform;
	}


	if ( HandSocket && OnMesh )
	{
		if ( HandSocket->AttachActor(HoldingItem, OnMesh) )
		{
			//Mesh에 Attach했다면, Item의 Relative위치,회전값을 변경한다.
			HoldingItem->SetActorRelativeTransform(RelativeTF);
		}
	}
}

bool AMainCharacter::ChangeWeapon(int32 index)
{
	Super::ChangeWeapon(index);

	/*
	//FPMode +Shadow Mesh의 AnimUpdate를 위해 갱신한다.
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	check(ShadowAnim);
	switch ( index )
	{
	case 0:
		ChangeWeaponTypeNumber(0);
		
		//FPAnimInstance->WeaponTypeNumber = 0;
		//ShadowAnim->WeaponTypeNumber = 0;
		break;
	case 1:
		if ( PrimaryWeapon )
		{
			ChangeWeaponTypeNumber(1);
			SetEquippedWeapon(PrimaryWeapon);
			//FPAnimInstance->WeaponTypeNumber = 1;
			//ShadowAnim->WeaponTypeNumber = 1;
		}
		break;
	case 2:
		if ( SubWeapon )
		{
			ChangeWeaponTypeNumber(1);
			SetEquippedWeapon(SubWeapon);
			//FPAnimInstance->WeaponTypeNumber = 1;
			//ShadowAnim->WeaponTypeNumber = 1;
		}
		break;
	case 3:
		if ( PistolWeapon )
		{
			ChangeWeaponTypeNumber(2);
			SetEquippedWeapon(PistolWeapon);
			//FPAnimInstance->WeaponTypeNumber = 2;
			//ShadowAnim->WeaponTypeNumber = 2;
		}
		break;
	}
	*/

	//For WeaponStatus Widget
	OnChangeWeapon.Broadcast(EquippedWeapon);
	OnGetAmmo.Broadcast(EquippedWeapon);
	OnChangeMode.Broadcast(EquippedWeapon);
	return true;
}

/*************************  Interaction 관련 ***************************************************/

/* Static을 감지해서 FHitResult를 리턴 */
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(10.f, 10.f); //살짝 넓게 해서 탐지가 쉽게 했다.
	Params.AddIgnoredActor(this);

	if ( EquippedWeapon )
	{
		Params.AddIgnoredActor(EquippedWeapon);
	}
	else if ( HoldingItem )
	{
		Params.AddIgnoredActor(HoldingItem);
	}

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
	if ( InterfaceActor )
	{
		InterfaceActor->SetOutline();

		//InteractActor의 Static Mesh의 위치를 기준으로 거리를 구한다.
		if ( ( GetActorLocation() - InteractActor->GetActorLocation() ).Size() <= ActiveInteractDistance )
		{
			MainController->ControlInteractText(true);
		}

		/* debug */
		//UE_LOG(LogTemp, Warning, TEXT("Main::InteractActor is Valid"));
	}
}

void AMainCharacter::UnsetInteractActor()
{
	if ( InteractActor )
	{
		IInteractive_Interface* InterfaceActor = Cast<IInteractive_Interface>(InteractActor);
		if ( InterfaceActor )
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
	if ( InteractActor )
	{
		IInteractive_Interface* Interface = Cast<IInteractive_Interface>(InteractActor);
		if ( Interface )
		{
			//if Car, Increase the length of the Interaction LineTrace.
			ANiceCar* MyCar = Cast<ANiceCar>(InteractActor);
			if ( MyCar )
			{
				float size = ( GetActorLocation() - InteractActor->GetActorLocation() ).Size();
				if ( size <= 650.f )
				{
					Interface->Interaction(this);
				}
			}
			else if ( ( GetActorLocation() - InteractActor->GetActorLocation() ).Size() <= ActiveInteractDistance )
			{
				Interface->Interaction(this);
			}
		}
	}

}

/*Called from ACoreUsableItem class, bind function, bind at Beginplay*/
void AMainCharacter::CompassStart()
{
	UMainHud* MainHud = Cast<UMainHud>(MainController->MainHud);
	if ( MainHud->CompassWidget )
	{
		//this bool for VkeyDown, hide/show CompassWidget
		bIsUsingCompass = true;

		MainHud->CompassWidget->PrepareCompassUpdate();
		if ( CameraMode == ECameraMode::ECM_TPS )
		{
			MainController->ControlCoreUsableWidget(true);
		}

		GetWorldTimerManager().SetTimer(T_CompassHandle, [ = ] {
			MainHud->CompassWidget->UpdateCompassPointer();
			}, GetWorld()->DeltaTimeSeconds, true);
	}
}

void AMainCharacter::CompassStop()
{
	//this bool for vkey down
	bIsUsingCompass = false;
	MainController->ControlCoreUsableWidget(false);
	if ( GetWorldTimerManager().IsTimerActive(T_CompassHandle) )
	{
		GetWorldTimerManager().ClearTimer(T_CompassHandle);
	}
}

FTransform AMainCharacter::LeftHandik()
{
	FTransform Transform;
	if ( EquippedWeapon )
	{
		switch ( CameraMode )
		{
			case ECameraMode::ECM_FPS:
				Transform = EquippedWeapon->SKMesh->GetSocketTransform(WeaponFPSLeftHandSocketName, ERelativeTransformSpace::RTS_World);
			break;
			case ECameraMode::ECM_TPS:
				Transform = EquippedWeapon->SKMesh->GetSocketTransform(WeaponTPSLeftHandSocketName, ERelativeTransformSpace::RTS_World);
			break;
		}		
		return Transform;
	}

	return Transform;
}

void AMainCharacter::SetLeftHandIK(float Alpha)
{
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	if ( !TPAnimInstance || !FPAnimInstance || !ShadowAnim ) return;

	Super::SetLeftHandIK(Alpha);
	ShadowAnim->SetLeftHandIKAlpha(Alpha);
	FPAnimInstance->SetLeftHandIKAlpha(Alpha);

}

void AMainCharacter::Die()
{
	Super::Die();

	//죽으면 TPS cam으로 변경한다.
	SetCameraMode(ECameraMode::ECM_TPS);

	ShadowMesh->SetCollisionProfileName(FName("RagDoll"));
	FPMesh->SetCollisionProfileName(FName("RagDoll"));
	//DisableInput(MainController);

	MainController->Die();
}

void AMainCharacter::QuickSlotNum4()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N4);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N4);
}
void AMainCharacter::QuickSlotNum5()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N5);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N5);
}
void AMainCharacter::QuickSlotNum6()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N6);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N6);
}
void AMainCharacter::QuickSlotNum7()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N7);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N7);
}
void AMainCharacter::QuickSlotNum8()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N8);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N8);
}
void AMainCharacter::QuickSlotNum9()
{
	if ( bIsDie ) return;
	OnQuickSlotUse.Broadcast(EQuickSlotNumber::EQSN_N9);
	//MainController->UseQuickSlotItem(EQuickSlotNumber::EQSN_N9);
}