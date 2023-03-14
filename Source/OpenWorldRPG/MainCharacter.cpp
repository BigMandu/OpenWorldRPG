// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "MainController.h"
#include "MainAnimInstance.h"

#include "OpenWorldRPG/Item/Interactable.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/BaseGrenade.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"

#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/UI/ResourceStatusWidgetInInventory.h"
#include "OpenWorldRPG/UI/ResourceStatusWidget.h"
#include "OpenWorldRPG/UI/WeaponStatusWidget.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/CompassWidget.h"


#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Camera/CameraComponent.h"

//#include "OpenWorldRPG/Item/Interactive_Interface.h"


#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h" //?踰源
??


// Sets default values
AMainCharacter::AMainCharacter() : Super()
{
	GetCharacterMovement()->bOrientRotationToMovement = true; //?吏??諛⑺?= 吏?諛⑺μ쇰? ?ㅼ
	/********** Input ***********/
	bDisableInput = false;

	bCrouchToggle = true; //?
?щ━湲???Toggle? true濡 湲곕낯?명
 ???
	bWalkToggle = true; //嫄룰린 ??Toggle? true濡 ?명
???
	bAimToggle = true; //議곗??ㅻ? toggel true?명
.

	bIsAim = false;
	bTabKeyDown = false;
	bIsLookInput = false;
	ActiveInteractDistance = 200.f; //??몄????댄
?????? 理?嫄곕━.

	/* 移대???愿??*/
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

	CameraTPS->SetRelativeLocation(TPSCam_Rel_Location); //3?몄묶 移대??쇰? ?댁? ?곗륫?쇰? 移?곗?寃 ???

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

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraFPS);

	ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowMesh"));
	ShadowMesh->SetupAttachment(GetRootComponent());

	CraftSysComp = CreateDefaultSubobject<UCraftSystemComponent>(TEXT("CraftSysComp"));
}


void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Player? AnimInstance瑜?遺?ъ⑤? */
	//TPAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance()); 
	FPAnimInstance = Cast<UMainAnimInstance>(FPMesh->GetAnimInstance());

	if (FPAnimInstance)//TPAnimInstance && FPAnimInstance)
	{
		/* ?ъ대? TP Animation? 湲곗??쇰? 異?ν?? */ //AnimInstance? StepSound_Notify?? ?몄?.
		//TPAnimInstance->StepSound.AddUObject(this, &AMainCharacter::StepSound); 
		//TPAnimInstance->WeaponTypeNumber = 0;
		FPAnimInstance->WeaponTypeNumber = 0;
		//FPAnimInstance->StartADS.AddUFunction(this,FName("GetADSPosition"));
	}
	
	//ShadowMesh? AnimInstance瑜??ｌ댁???
	ShadowMesh->SetAnimInstanceClass(GetMesh()->GetAnimClass());
	
	ShadowMesh->SetOnlyOwnerSee(true);
	ShadowMesh->SetRenderInMainPass(false);
	ShadowMesh->SetHiddenInGame(true);

	//ShadowMesh->SetOwnerNoSee(true);
	//ShadowMesh->SetCastShadow(true);
	
	//ShadowMesh->SetHiddenInGame(true);
	//ShadowMesh->SetCastHiddenShadow(true);
	
	

	CameraFPS->SetRelativeLocation(FVector( 20.136719, -2.999939, 68.834351));
	FPMesh->SetRelativeLocation(FVector(-15.128922, -3.533375, -150.495850));

	FPMeshOriginTransform = FPMesh->GetRelativeTransform();
	FPMesh->SetHiddenInGame(true);
	FPMesh->SetCastShadow(false);
	GetMesh()->SetCastShadow(false);

	

	/* FPS Aim愿??湲곕낯 ?ㅼ 媛 ???/
	BaseCamTPSfov = CameraTPS->FieldOfView;
	BaseCamFPSfov = CameraFPS->FieldOfView;
	BaseFPMeshTransform = FPMesh->GetRelativeTransform();

	
	//GetCapsuleComponent()->SetCollisionProfileName(FName("MainChar"));
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	//GetCapsuleComponent()->SetCollisionProfileName("MainChar");


	//FPMesh->HideBoneByName(FName("spine_02"));
	/*FPLowerLegMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_l"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->HideBoneByName(FName("upperarm_r"), EPhysBodyOp::PBO_None);
	FPLowerLegMesh->SetHiddenInGame(true);

	FPLowerLegMesh->SetCastShadow(false);
	FPLowerLegMesh->SetOnlyOwnerSee(true);
	FPLowerLegMesh->SetReceivesDecals(false);*/


	CompassBeginUse.AddDynamic(this,&AMainCharacter::CompassStart);
	CompassEndUse.AddDynamic(this,&AMainCharacter::CompassStop);


	
	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	/********** 珥湲??명
 ************/
	SetCameraMode(ECameraMode::ECM_TPS); //珥湲?移대???紐⑤? 3?몄묶 紐⑤濡.
	SetAimMode(EAimMode::EAM_NotAim);

	MainController = Cast<AMainController>(GetController());


	/****** Main Hud? Widget怨?Event Bind ********/
	UMainHud* MainHud = Cast<UMainHud>(MainController->MainHud);
	if (MainHud)
	{

		if(StatManagementComponent)
		{
			MainHud->ResourceWidget->BindStatManager(StatManagementComponent);
			MainHud->NewInventoryWidget->ResourceStatusWidget->BindStatManager(StatManagementComponent);
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
	
	//bIsLookInput ?명
 (Aim Init? ??寃.)
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

	//this bool toggle at MainController, when GetIn/Out of the car.
	if(bDisableInteractionLineTrace == false)
	{
		InteractionLineTrace();
	}

	/* Weapon? AimInitialize?⑥?? ?ъ⑺? Timer ?댁??*/
	if (EquippedWeapon)
	{
		//??대㉧媛 ??以?닿?
		if (GetWorldTimerManager().IsTimerActive(EquippedWeapon->AimInitHandle))
		{
			//aim? ?由ш굅?, AlphaTime??1珥媛 ??닿?嫄곕, ?? 以?대쇰㈃
			if (bIsLookInput || EquippedWeapon->AlphaTime >= 1.f || EquippedWeapon->bIsFiring)
			{
				//?? 珥湲고 ??대㉧瑜?珥湲고 ??⑤?
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


void AMainCharacter::InteractionLineTrace()
{
	/* ??몄??Line Trace 蹂寃?. ver2  ?? 肄?瑜?GetPlayerViewPoint?⑥瑜??댁⑺?媛?듯 ?耳곕? */
	FRotator Rot;
	MainController->GetPlayerViewPoint(Interact_LineTrace_StartLocation, Rot);
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * 500.f;
		break;
	case ECameraMode::ECM_TPS:
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + Rot.Vector() * (MAXCameraLength + 800.f); //移대???oom湲몄대낫???湲멸? ???쇳??
	}

	/* 移대???紐⑤? ?곕Ⅸ ??몄??LineTrace 蹂寃? ver 1 --> ver 2濡 援泥?*/
	/*
	switch (CameraMode)
	{
	case ECameraMode::ECM_FPS:
		Interact_LineTrace_StartLocation = CameraFPS->GetComponentLocation();
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraFPS->GetComponentRotation().Vector() * 500.f;

		break;
	case ECameraMode::ECM_TPS:
	{
		Interact_LineTrace_StartLocation = CameraTPS->GetComponentLocation(); //TPS移대??쇰? 湲곗??쇰? Trace瑜??????
		Interact_LineTrace_EndLocation = Interact_LineTrace_StartLocation + CameraTPS->GetComponentRotation().Vector() * (MAXCameraLength + 800.f); //移대???oom湲몄대낫???湲멸? ???쇳??
		break;
	}
	default:
		break;
	}
	*/


	/* ??몄????ㅽ몃? ?? */
	/* Static FHitResult瑜?由ы대??? Interface 蹂?, ?깃났?硫?Outline怨?TEXT瑜???.*/
	AActor* HitActor = InteractableLineTrace(Interact_LineTrace_StartLocation, Interact_LineTrace_EndLocation).GetActor();

	//AInteractable* HitInteractActor = Cast<AInteractable>(HitActor);
	IInteractive_Interface* HitInteractActor = Cast<IInteractive_Interface>(HitActor);
	if (HitInteractActor)// && HitInteractActor->bCanNotInteractable == false)
	{
		if (InteractActor) //?대?InteractActor媛 ?怨
		{
			if (InteractActor != HitActor)//HitInteractActor) //吏湲 蹂닿? ?? Actor? InteractActor媛 ?ㅻⅤ硫?
			{
				UnsetInteractActor(); //InteractActor瑜?Unset?댁???
			}
			else
			{
				//嫄곕━? ?곕?InteractText媛 ?
?곗댄??湲??臾몄 異媛 ?댁???
				SetInteractActor(HitActor);//HitInteractActor);
			}
		}
		else //InteractActor媛 ?? 寃쎌?
		{
			SetInteractActor(HitActor);//HitInteractActor); //Interactable??寃쎌?Set? ?댁???
		}
	}
	else
	{
		UnsetInteractActor();
	}
	//InteractActor媛 ?ㅻ?寃쎌?UnsetInteractActor ?몄???. (Outline?????댁?? 踰洹멸? ??.) -> ?닿껐??
}


void AMainCharacter::FPSAimLocationAdjust()
{
	if (CameraFPS)
	{
		if (bIsAim)
		{
			//媛 珥留??FPMesh? ?щ?瑜??移? 遺李⑺湲????
			//Weapon Class? Transform 蹂?瑜?留?ㅼ댁ㄼ??
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


//???댁??? ?? -> VKeyDN
void AMainCharacter::SetCameraMode(ECameraMode Type)
{
	CameraMode = Type;
	switch (CameraMode)
	{
	case ECameraMode::ECM_TPS:
		CameraTPS->Activate();
		CameraFPS->Deactivate();

		/* ??? 移대??쇱留 ???媛?濡 ?ㅼ */
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		/* Character Mesh ?ㅼ */
		FPMesh->SetHiddenInGame(true);  //1?몄묶 Mesh ?④?
		//GetMesh()->SetHiddenInGame(false); //3?몄묶 Mesh ??④?
		GetMesh()->SetCastShadow(true);
		GetMesh()->UnHideBoneByName(FName("spine_01")); //spine_02
		ShadowMesh->SetHiddenInGame(true);


		if (bIsUsingCompass)
		{
			MainController->ControlCoreUsableWidget(true);
		}

		UCustomInventoryLibrary::ShowAllEquipment(Equipment);

	break;

	case ECameraMode::ECM_FPS:
		CameraFPS->Activate();
		CameraTPS->Deactivate();

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		//移대??쇱 Yaw??? ?곕?罹由?곗 Yaw Mesh媛 ????濡 ??? 洹몃???룰구?, 醫??寃嫄몄??媛?ν??
		bUseControllerRotationYaw = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;

		/*  Character Mesh ?ㅼ */
		FPMesh->SetHiddenInGame(false);
		GetMesh()->HideBoneByName(FName("spine_01"), EPhysBodyOp::PBO_None);
		ShadowMesh->SetHiddenInGame(false);
		GetMesh()->SetCastShadow(false);

		//bRenderInMainPass瑜?false濡 ?寃?硫?Rendering? ?吏 ?吏留, Shadow? ??留 ???
		// ?닿구 ?ъ⑺?ㅻ㈃ Mesh媛 Show??댁???? SetHiddenInGame? True濡 ?硫????	
		//GetMesh()->SetRenderInMainPass(false);

		if (bIsUsingCompass)
		{
			MainController->ControlCoreUsableWidget(false);
		}

		//?μ갑以???λ?瑜?紐⑤ hide???
		UCustomInventoryLibrary::HideAllEquipment(Equipment);

	break;
	}

	/* ?μ갑 臾닿린 Mesh? 遺李?*/
	if (EquippedWeapon)
	{
		EquippedWeapon->GunAttachToMesh(this);
	}
	else if (HoldingItem.IsValid())
	{
		ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
		if (Grenade)
		{
			Grenade->AttachToHand(this, HoldingItem->ItemObj);
		}		
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
			if (CameraMode == ECameraMode::ECM_TPS)
			{
				//TPS紐⑤ + Aim???쇰? 移대??쇰? ?댁? ??쇰? ?↔꺼以??
				//???SpringArm? 湲몄대? ??ν??
				/*BeforeCameraLength = CameraBoom->TargetArmLength;
				CameraBoom->TargetArmLength = MINCameraLength + 20.f;
				CameraTPS->SetRelativeLocation(TPSCam_Aim_Rel_Location);
				CameraTPS->FieldOfView = 77.f;*/
				LerpCamera(CameraTPS, MINCameraLength + 20.f, TPSCam_Aim_Rel_Location, 77.f);

				bIsAim = true;
				EquippedWeapon->bIsAiming = true;
				GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;
				
			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//idle anim? ???Socket? WorldTF媛????댁??嫄?諛⑹??湲?????吏Anim? ?ъ???
				if (EquippedWeapon->WeaponDataAsset->Animaton.FPS_ADS_Anim)
				{
					//below is using Blend.
					FPAnimInstance->Montage_Play(EquippedWeapon->WeaponDataAsset->Animaton.FPS_ADS_Anim, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
					FPAnimInstance->Montage_JumpToSection(FName("Default"), EquippedWeapon->WeaponDataAsset->Animaton.FPS_ADS_Anim);

					//FPMesh->bPauseAnims = true;
					LerpCamera(CameraFPS, 75.f);
				}
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
				//?↔릿 移대??쇰? ?ㅼ ?蹂???⑤?
				/*float CurrentLength = CameraBoom->TargetArmLength;
				float CameraLength = FMath::FInterpTo(CurrentLength, BeforeCameraLength, GetWorld()->GetDeltaSeconds(), 15.f);*/
				/*CameraBoom->TargetArmLength = BeforeCameraLength;
				CameraTPS->SetRelativeLocation(TPSCam_Rel_Location);
				CameraTPS->FieldOfView = BaseCamTPSfov;*/
				LerpCamera(CameraTPS, MAXCameraLength - 20.f, TPSCam_Rel_Location, BaseCamTPSfov);

			}
			else if (CameraMode == ECameraMode::ECM_FPS)
			{
				//FPAnimInstance->StopAllMontages(0.f);//Montage_Stop( (0.f);
				FPSnotADS();

				FPMesh->bPauseAnims = false;
				//FPMesh->SetRelativeTransform(FPMeshOriginTransform);
				//CameraFPS->FieldOfView = BaseCamFPSfov;
				LerpCamera(CameraFPS,BaseCamFPSfov);
			}
			break;
		}
		default:
			break;
		}
	}
}

//called from StartFPADS class (Anim Notify)
void AMainCharacter::FPSADS()
{
	UE_LOG(LogTemp, Warning, TEXT("AMainCharacter::FPSADS called"));
	FTransform ADSPos = GetFpsAdsPosition();
	
	bIsAim = true;
	EquippedWeapon->bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = MinWalkSpeed;

	ADSTime = 0.f;
	ADSAlphaTime = 0.f;

	
	GetWorldTimerManager().SetTimer(T_ADSTimerHandle, [=] {
		FTransform OriginPos = FPMesh->GetRelativeTransform();
		if (ADSAlphaTime >= 1.1f)
		{
			GetWorldTimerManager().ClearTimer(T_ADSTimerHandle);
		}
		ADSTime += GetWorld()->GetDeltaSeconds();
		ADSAlphaTime = ADSTime/1.f;

		FTransform LerpPos = UKismetMathLibrary::TLerp(OriginPos, ADSPos, ADSAlphaTime);

		//FPMesh瑜??대?Offset留???대, Camera? 以?? ?ㅻ濡 ???
		FPMesh->SetRelativeTransform(LerpPos);//OffsetWithoutX);

		},GetWorld()->GetDeltaSeconds(), true);

	//紐⑤ ?명
????硫?AnimMontage瑜??댁?댁 Locomotion Anim? ?댁대媛?濡 ???
	FPAnimInstance->StopAllMontages(0.f);
	
	//CameraFPS->SetWorldTransform(SightTransform);
}

void AMainCharacter::FPSnotADS()
{
	ADSTime = 0.f;
	ADSAlphaTime = 0.f;


	GetWorldTimerManager().SetTimer(T_ADSTimerHandle, [=] {
	FTransform CurrentFPPos = FPMesh->GetRelativeTransform();
	if (ADSAlphaTime >= 1.1f)
	{
		GetWorldTimerManager().ClearTimer(T_ADSTimerHandle);
	}
	ADSTime += GetWorld()->GetDeltaSeconds();
	ADSAlphaTime = ADSTime / 1.f;

	FTransform LerpPos = UKismetMathLibrary::TLerp(CurrentFPPos, FPMeshOriginTransform, ADSAlphaTime);

	
	FPMesh->SetRelativeTransform(LerpPos);//OffsetWithoutX);

	}, GetWorld()->GetDeltaSeconds(), true);
}

FTransform AMainCharacter::GetFpsAdsPosition()
{	
	
	UE_LOG(LogTemp, Warning, TEXT("AMainCharacter::GetADSPosition"));

	FTransform FPMeshWorld = FPMesh->GetComponentTransform();
	
	//EquippedWeapon?? SightSocket? Get?? ?⑥瑜??몄????
	FTransform SightTransform = EquippedWeapon->GetSightSocketTransform();


	//SightSocket??  FPMesh? Relative Transform? 援ы??
	//?대寃 援ы Offset? Mesh? Transform? ??⑺??

	/**explain Make Relative Transform
	 * Example: ChildOffset = MakeRelativeTransform(Child.GetActorTransform(), Parent.GetActorTransform())
	 * This computes the relative transform of the Child from the Parent.
	 */
	FTransform Offset = FPMeshWorld.GetRelativeTransform(SightTransform);//.Inverse();

	// Weapon? Clipping? ?? Trace瑜?FPMesh?쇰 Aim, NotAim? 蹂댁?湲????Aim?쇰 Mesh瑜???쇰? 醫 ?媛寃 ???
	// (湲곗〈 Aim?? FPMesh媛 ?ㅻ? ?ㅼ닿?踰??Clipping臾몄媛 ??쇱??ъ.)
	FTransform OffsetWithoutX = FTransform(Offset.GetRotation(), FVector(-10.f, Offset.GetTranslation().Y, Offset.GetTranslation().Z));

	return OffsetWithoutX;
}

void AMainCharacter::LerpCamera(UCameraComponent* VarTPScam, float TargetBoomLength, FVector TargetCamRelativeLocation, float TargetFOV)
{
	CamTime = 0.f;
	CamAlphaTime = 0.f;
	GetWorldTimerManager().ClearTimer(T_CameraHandle);

	GetWorldTimerManager().SetTimer(T_CameraHandle, [=] {
		FVector CamCurrentLo = VarTPScam->GetRelativeLocation();
		float CamCurrentFOV = VarTPScam->FieldOfView;
		float  BoomCurrentLen = CameraBoom->TargetArmLength;

	CamTime += GetWorld()->GetDeltaSeconds();
	CamAlphaTime = CamTime / 1.f;

	FVector SetCamLo = UKismetMathLibrary::VLerp(CamCurrentLo, TargetCamRelativeLocation, CamAlphaTime);
	float SetFOV = UKismetMathLibrary::Lerp(CamCurrentFOV, TargetFOV, CamAlphaTime);
	float SetBomLength = UKismetMathLibrary::Lerp(BoomCurrentLen, TargetBoomLength, CamAlphaTime);
	
	VarTPScam->SetRelativeLocation(SetCamLo);
	VarTPScam->SetFieldOfView(SetFOV);
	CameraBoom->TargetArmLength = SetBomLength;

	if (CamAlphaTime >= 1.1f)
	{
		GetWorldTimerManager().ClearTimer(T_CameraHandle);
	}

		}, GetWorld()->GetDeltaSeconds(), true);
}

void AMainCharacter::LerpCamera(UCameraComponent * VarFPScam, float TargetFOV)
{
	CamTime = 0.f;
	CamAlphaTime = 0.f;
	GetWorldTimerManager().ClearTimer(T_CameraHandle);

	GetWorldTimerManager().SetTimer(T_CameraHandle, [=] {
	float CamCurrentFOV = VarFPScam->FieldOfView;

	CamTime += GetWorld()->GetDeltaSeconds();
	CamAlphaTime = CamTime / 1.f;	
	float SetFOV = UKismetMathLibrary::Lerp(CamCurrentFOV, TargetFOV, CamAlphaTime);
		
	VarFPScam->SetFieldOfView(SetFOV);

	if (CamAlphaTime >= 1.1f)
	{
		GetWorldTimerManager().ClearTimer(T_CameraHandle);
	}

		}, GetWorld()->GetDeltaSeconds(), true);
}



/****************************** ?대, ?? 愿????諛?몃 ***************************/

void AMainCharacter::TurnAtRate(float Rate)
{
	//留?delta珥 留??base rate留????
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
		//?대諛⑺μ쇰? 媛吏 李얘?
		FRotator Rotation = MainController->GetControlRotation();
		FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		// forward踰≫곕? 援ы??
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//洹?諛⑺μ쇰? value留??媛??
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

		//right 踰≫?
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
		//Up Timer 珥湲고
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
		//Down Timer 珥湲고
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
		Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance())->bIsJumpkeyDown = true;
		Super::Jump();
	}
}
void AMainCharacter::MyStopJumping()
{
	bIsJumpKeyDown = false;
	Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance())->bIsJumpkeyDown = false;
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
	//Crouch瑜?Toggle濡 ?ъ⑺吏 ???留 Key Release  bind?⑥瑜??몄????
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
			//bIsWalking = false; -> Set enum?⑥?? 蹂寃쏀댁ㄼ??
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
		//TPS紐⑤??寃쎌곗留 ?ㅽщ· ?ㅼ? ?
??媛?ν?濡 (移대???硫寃 , 媛源??
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
			//vkeydown?쇰? ??? 蹂寃쏀?ㅻ㈃, 湲곗〈Aimmode瑜?洹몃濡 ?ｌ?Fps/Tps瑜??ㅼ ?명
???
			SetAimMode(AimMode);
			break;
		case ECameraMode::ECM_FPS:
			SetCameraMode(ECameraMode::ECM_TPS);
			SetAimMode(AimMode);
			
			break;
		}

		ReAttachHoldingItem();
	}
}


/************** Interactive & Inventory Key bind ?⑥ ***********/
void AMainCharacter::LMBDown()
{
	if (bDisableInput == false)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StartFire();
		}
		else if (HoldingItem.IsValid())
		{
			ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
			if(Grenade)
			{
				Grenade->ReadyToThrow();
			}
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
		else if (HoldingItem.IsValid())
		{
			ABaseGrenade* Grenade = Cast<ABaseGrenade>(HoldingItem);
			if (Grenade)
			{
				Grenade->ThrowGrenade(this);
			}
			
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

/*******************************  Vehicle 愿??************************************************/

//void AMainCharacter::ToggleCar()
//{
//	
//	
//}
//
//void AMainCharacter::GetIntheCar()
//{
//	
//}
//void AMainCharacter::GetOuttheCar()
//{
//
//}

/*************************  Weapon, Item 愿??***************************************************/
void AMainCharacter::PlayUseItemAnim(AItem* Item)
{
	Super::PlayUseItemAnim(Item);
	
	//FPMesh? Anim? ?ъ???
	FPAnimInstance->Montage_Play(Item->ItemSetting.DataAsset->FPS_UseAnimMontage);
		
}

void AMainCharacter::StopUseItemAnim()
{
	Super::StopUseItemAnim();
	FPAnimInstance->Montage_Stop(0.f,nullptr);
}

//CameraMode? ?곕?HoldingItem? Attach??⑤?
void AMainCharacter::ReAttachHoldingItem()
{
	if(HoldingItem.IsValid() == false) return;

	const USkeletalMeshSocket* HandSocket = nullptr; 
	FTransform RelativeTF;
	USkeletalMeshComponent* OnMesh = nullptr;

	if (CameraMode == ECameraMode::ECM_FPS)
	{
		OnMesh = FPMesh;
		HandSocket = FPMesh->GetSocketByName(LeftHandGripSocketName);
		RelativeTF = HoldingItem->ItemSetting.DataAsset->FPS_HandAttachTransform;
	}
	else
	{
		OnMesh = GetMesh();
		HandSocket = GetMesh()->GetSocketByName(LeftHandGripSocketName);
		RelativeTF = HoldingItem->ItemSetting.DataAsset->TPS_HandAttachTransform;
	}
	
	
	if (HandSocket && OnMesh)
	{
		if (HandSocket->AttachActor(HoldingItem.Get(), OnMesh))
		{
			//Mesh? Attach??ㅻ㈃, Item? Relative?移,??媛? 蹂寃쏀??
			HoldingItem->SetActorRelativeTransform(RelativeTF);
		}
	}
}

bool AMainCharacter::ChangeWeapon(int32 index)
{
	Super::ChangeWeapon(index);
	
	//FPMode +Shadow Mesh? AnimUpdate瑜????媛깆???
	UMainAnimInstance* ShadowAnim = Cast<UMainAnimInstance>(ShadowMesh->GetAnimInstance());
	check(ShadowAnim);
	switch (index)
	{
	case 0:
		FPAnimInstance->WeaponTypeNumber = 0;
		ShadowAnim->WeaponTypeNumber = 0;
		break;
	case 1:
		if (PrimaryWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
			ShadowAnim->WeaponTypeNumber = 1;
		}
		break;
	case 2:
		if (SubWeapon)
		{			
			FPAnimInstance->WeaponTypeNumber = 1;
			ShadowAnim->WeaponTypeNumber = 1;
		}
		break;
	case 3:
		if (PistolWeapon)
		{
			FPAnimInstance->WeaponTypeNumber = 2;
			ShadowAnim->WeaponTypeNumber = 2;
		}
		break;
	}

	//For WeaponStatus Widget
	OnChangeWeapon.Broadcast(EquippedWeapon);
	OnGetAmmo.Broadcast(EquippedWeapon);
	OnChangeMode.Broadcast(EquippedWeapon);
	return true;
}

/*************************  Interaction 愿??***************************************************/

/* Static? 媛吏?댁 FHitResult瑜?由ы?*/
FHitResult AMainCharacter::InteractableLineTrace(const FVector& StartLo, const FVector& EndLo)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(10.f, 10.f); //?댁? ?寃 ?댁 ?吏媛 ?쎄? ???
	Params.AddIgnoredActor(this);

	if (EquippedWeapon)
	{
		Params.AddIgnoredActor(EquippedWeapon);
	}
	else if (HoldingItem.IsValid())
	{
		Params.AddIgnoredActor(HoldingItem.Get());
	}

	//SweepSingle濡 蹂寃? 罹≪??? 紐⑥?쇰? LineTrace.	
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

		//InteractActor? Static Mesh? ?移瑜?湲곗??쇰? 嫄곕━瑜?援ы??
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
			//if Car, Increase the length of the Interaction LineTrace.
			ANiceCar* MyCar = Cast<ANiceCar>(InteractActor);
			if (MyCar)
			{
				float size = (GetActorLocation() - InteractActor->GetActorLocation()).Size();
				if (size <= 650.f)
				{
					Interface->Interaction(this);
				}
			}
			else if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= ActiveInteractDistance)
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
	if (MainHud->CompassWidget)
	{
		//this bool for VkeyDown, hide/show CompassWidget
		bIsUsingCompass = true;
		
		MainHud->CompassWidget->PrepareCompassUpdate();
		if (CameraMode == ECameraMode::ECM_TPS)
		{
			MainController->ControlCoreUsableWidget(true);
		}

		GetWorldTimerManager().SetTimer(T_CompassHandle, [=] {
			MainHud->CompassWidget->UpdateCompassPointer();
			}, GetWorld()->DeltaTimeSeconds, true);
	}
}
	
void AMainCharacter::CompassStop()
{
	//this bool for vkey down
	bIsUsingCompass = false;
	MainController->ControlCoreUsableWidget(false);
	if (GetWorldTimerManager().IsTimerActive(T_CompassHandle))
	{
		GetWorldTimerManager().ClearTimer(T_CompassHandle);
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
