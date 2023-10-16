// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Grey.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Item/Weapon/Weapon.h"
#include "Item/Weapon/TwoHandWeapon.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/Boxcomponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"
#include "Characters/LustreAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "HUD/LustreHUD.h"
#include "HUD/LustreWidget.h"
#include "Components/AttributeComponent.h"
#include "Item/Treasure/Treasure.h"
#include "Item/Soul.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PaperSpriteComponent.h"
#include "HUD/Death.h"
#include "Item/Door/Door.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/Enemy.h"
#include "Components/AudioComponent.h"
#include "KismetAnimationLibrary.h"

// Sets default values
AGrey::AGrey()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(SpringArm);
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Potion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Potion"));
	Potion->SetupAttachment(GetMesh(), FName("PotionSocket"));
	Potion->SetVisibility(false);

	MiniMapCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("MiniMapCamera"));
	MiniMapCamera->SetupAttachment(GetRootComponent());

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCaputre"));
	MiniMapCapture->SetupAttachment(MiniMapCamera);

	CharacterSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ChararcterSprite"));
	CharacterSprite->SetupAttachment(GetMesh());

	
}

void AGrey::SetOverlappingItem(AItem* Item)
{
	OverlappingItems = Item;

	ADoor* Door = Cast<ADoor>(Item);
	if (LustreWidget && Door)
	{
		LustreWidget->ShowInteractMeesage();
	}
	else
	{
		LustreWidget->HideInteractMessage();
	}

}

void AGrey::CameraShake()
{
	if (CameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(CameraShakeClass, 1.f);
	}
}

void AGrey::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	Tags.Add(FName("Grey"));

	InitializeWidget();

	SpawnDefaultWeapon();

	CharacterMovement = GetCharacterMovement();

	GetMesh()->HideBoneByName(FName("sword_bottom"), EPhysBodyOp::PBO_None);
	
	GetMesh()->HideBoneByName(FName("shield_inner"), EPhysBodyOp::PBO_None);

}

void AGrey::InitializeWidget()
{
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		TObjectPtr<ALustreHUD> LustreHUD = Cast<ALustreHUD>(PlayerController->GetHUD());
		if (LustreHUD)
		{
			LustreWidget = LustreHUD->GetLustreWidget();
			if (LustreWidget && Attributes)
			{
				LustreWidget->SetHealthBarPercent(Attributes->GetHealthPercent());
				
				LustreWidget->SetStaminaBarPercent(1.f);
				
				LustreWidget->SetGold(0);
				
				LustreWidget->SetSouls(0);
				
				LustreWidget->SetPotionText(Attributes->GetPotion());
			}
		}
	}
}

void AGrey::Move(const FInputActionValue& Value)
{
	if (IsAttacking || ActionState == EActionState::EAS_Dead) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller && CanMove)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector FowardDirecton = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(FowardDirecton, MovementVector.Y);

		const FVector RightDirectoin = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirectoin, MovementVector.X);

		IsRunning = true;
	}
}

void AGrey::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AGrey::Jump(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Dead) return;
	
	if (GetController() && !IsAttacking && !IsJumping && RollState == ERollState::ERS_NoRolling)
	{
		Super::Jump();
		IsJumping = true;
	}
}

void AGrey::Attack()
{
	bool bCanAttack = EquippedWeapon &&
		!IsAttacking && 
		CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState < EActionState::EAS_Dead &&
		GuardState == EGuardState::EGS_NoGuard;
	
	if (bCanAttack)
	{
		SaveAttacking = true;
		IsAttacking = true;
		PlayAttackMontages();
	}
}

void AGrey::SpecialAttack()
{
	if (!(Attributes->GetStamina() > Attributes->GetSpecialAttackCost())) return;
	
	if (EquippedWeapon && IsSpecialAttacking == false && CharacterState != ECharacterState::ECS_Unequipped)
	{
		Super::SpecialAttack();
		
		IsSpecialAttacking = true;
		
		AttackCount = 0;
		
		SaveAttacking = false;

		if (LustreWidget)
		{
			Attributes->UseStamina(Attributes->GetSpecialAttackCost());
			
			LustreWidget->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}

}

void AGrey::InteractPressed()
{
	ADoor* OverlappingDoor = Cast<ADoor>(OverlappingItems);
	if (OverlappingDoor && LustreWidget)
	{
		OverlappingDoor->OnInteract();
	}
}

void AGrey::GuardPressed()
{
	bUseControllerRotationYaw = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GuardState == EGuardState::EGS_NoGuard && EquippedWeapon && !IsAttacking)
	{
		AnimInstance->Montage_Play(BlockParryMontage);
		
		GuardState = EGuardState::EGS_Guarding;
	}
}

void AGrey::GuardReleased()
{
	bUseControllerRotationYaw = false;

	if (GuardState == EGuardState::EGS_Guarding && EquippedWeapon && !IsAttacking)
	{
		GuardState = EGuardState::EGS_NoGuard;
	}
}

void AGrey::TakePotion()
{
	bool bCanTakePotion = ActionState < EActionState::EAS_Dead && 
		CharacterMovement &&
		!IsAttacking && 
		!IsTakingPotion && 
		Attributes &&
		Attributes->IsAlive() &&
		Attributes->GetPotion() > 0;


	if(bCanTakePotion && Attributes && LustreWidget)
	{
		IsTakingPotion = true;
		
		PlayTakePotionMontage();
		
		CharacterMovement->DisableMovement();
		
		Attributes->UsePotion(1);
		
		LustreWidget->SetPotionText(Attributes->GetPotion());
	}
}

void AGrey::FinishTakePotion()
{
	if (Attributes && CharacterMovement)
	{
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
		
		Attributes->Heal(HealAmount);
		
		SetHUDHealth();
		
		EquippedWeapon->ItemMeshShield->SetVisibility(true);
		
		Potion->SetVisibility(false);
		
		IsTakingPotion = false;
	}
}

void AGrey::GuardSuccess(FVector& ImpactPoint)
{
	if (!Attributes->IsAlive()) Die();

	else if (Attributes->IsAlive() && GuardSound && GuardParticles && GetWorld())
	{
		ActionState = EActionState::EAS_Engaged;
		
		EngagedTime = 0.f;

		UGameplayStatics::PlaySoundAtLocation(this, GuardSound, EquippedWeapon->GetShieldBox()->GetComponentLocation());

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GuardParticles, EquippedWeapon->GetShieldBox()->GetComponentLocation());

		FVector KnockBack = GetActorLocation() - GetActorForwardVector() * KnockBackDistance; // 뒤로 넉백.

		SetActorLocation(KnockBack);
	}
}

void AGrey::ParryingSuccess(FVector& ImpactPoint)
{
	UWorld* World = GetWorld();
	if (ParryingSound && ParryingParticles && World)
	{
		ActionState = EActionState::EAS_Engaged;
		
		EngagedTime = 0.f;

		UGameplayStatics::PlaySoundAtLocation(this, ParryingSound, EquippedWeapon->GetShieldBox()->GetComponentLocation());

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParryingParticles, EquippedWeapon->GetShieldBox()->GetComponentLocation());
	}
}

void AGrey::PlayAttackMontages()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f);
		
		FName SectionName = FName();
		
		switch (AttackCount)
		{
		case 0:
			SectionName = FName("Attack1");
			AttackCount = 1;
			break;
		case 1:
			SectionName = FName("Attack2");
			AttackCount = 2;
			break;
		case 2:
			SectionName = FName("Attack3");
			AttackCount = 0;
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AGrey::PlayTakePotionMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		EquippedWeapon->ItemMeshShield->SetVisibility(false);
		
		Potion->SetVisibility(true);
		
		AnimInstance->Montage_Play(TakePotionMontage);
	}

	FOnMontageEnded MontageEnd;
	MontageEnd.BindWeakLambda(this, [this](UAnimMontage* Animmontage, bool bInterrupted)
		{
			if (bInterrupted && CharacterMovement) 
			{
				ActionState = EActionState::EAS_Engaged;
				CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
				EquippedWeapon->ItemMeshShield->SetVisibility(true);
				IsTakingPotion = false;
			}
			else
			{
				FinishTakePotion();
			}
		});
	AnimInstance->Montage_SetEndDelegate(MontageEnd, TakePotionMontage);
}

void AGrey::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);

	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_Engaged;
	EngagedTime = 0.f;

}

void AGrey::Die()
{
	ActionState = EActionState::EAS_Dead;
	
	Tags.Add(FName("Dead"));
	
	CanMove = false;

	if (CombatEnemy) CombatEnemy = nullptr;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);

	if (IsPlayingCombatSound) CombatSoundComponent->Stop();

	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	if (Instance && DeathMontage)
	{
		Instance->Montage_Play(DeathMontage);
	}
	
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		TObjectPtr<ALustreHUD> LustreHUD = Cast<ALustreHUD>(PlayerController->GetHUD());
		if (LustreHUD)
		{
			LustreHUD->PlayDeathUI();
		}
	}
	
}

void AGrey::AddSouls(ASoul* Soul)
{
	if (LustreWidget && Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
		LustreWidget->SetSouls(Attributes->GetSouls());
	}
}

void AGrey::AddGold(ATreasure* Gold)
{
	if (LustreWidget && Attributes)
	{
		Attributes->AddGold(Gold->GetGold());
		LustreWidget->SetGold(Attributes->GetGold());
	}
}

void AGrey::SpawnDefaultWeapon()
{
	Super::SpawnDefaultWeapon();

	CharacterState = ECharacterState::ECS_EquippedWeapon;
}

void AGrey::RegenStamina(float DeltaTime)
{
	if (Attributes && LustreWidget)
	{
		Attributes->RegenStamina(DeltaTime);
		LustreWidget->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AGrey::PlayDodgeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage) 
	{
		FName SectionName = RollName;

		AnimInstance->Montage_Play(DodgeMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DodgeMontage);
	}
}

void AGrey::Dodge()
{
	if (ActionState == EActionState::EAS_Dead || !HasEnoughStamina()) return;
	
	if (CharacterState != ECharacterState::ECS_Unequipped && RollState == ERollState::ERS_NoRolling)
	{
		PlayDodgeMontage();

		if (LustreWidget)
		{
			Attributes->UseStamina(Attributes->GetDodgeCost());
			LustreWidget->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}

}

bool AGrey::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void AGrey::SetActionState()
{
	switch (ActionState)
	{
	case EActionState::EAS_Unoccupied:
		bUseControllerRotationYaw = false;
		if (IsPlayingCombatSound)
		{
			CombatSoundComponent->FadeOut(2.f, 0.f);
			IsPlayingCombatSound = false;
		}
		break;
	case EActionState::EAS_Engaged:
		bUseControllerRotationYaw = true;
		if (!IsPlayingCombatSound)
		{
			CombatSoundComponent = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CombatSound, GetActorLocation(), GetActorRotation());
			IsPlayingCombatSound = true;
		}
		break;
	}
}

FRotator AGrey::GetLookAtEnemyYaw()
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatEnemy->GetMesh()->GetBoneLocation(FName("head")));

	FRotator LookAtYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtYaw;
}

void AGrey::SaveAttack()
{	
	IsAttacking = false;
	CanMove = false;
}


void AGrey::ResetCombo()
{
	AttackCount = 0;
	SaveAttacking = false;
	IsAttacking = false;
	CanMove = true;
}

void AGrey::CanMoving()
{
	CanMove = true;
}

void AGrey::Invincible()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGrey::NotInvincible()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGrey::CanJumping()
{
	IsJumping = false;
}

void AGrey::CanSpecialAttack()
{
	IsSpecialAttacking = false;
}

void AGrey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckRollDirection();

	SetActionState();

	RegenStamina(DeltaTime);

	//전투시간 체크.
	CheckEngagedTime(DeltaTime);

	// 전투중에는 적의 머리를 바라보도록 함.
	LookAtCombatEnemy(DeltaTime);
	
}

bool AGrey::IsEnemyAlive()
{
	return CombatEnemy && CombatEnemy->GetHealth() > 0.f;
}

void AGrey::LookAtCombatEnemy(float DeltaTime)
{
	if (IsEnemyAlive() && ActionState == EActionState::EAS_Engaged)
	{
		SetActorRotation(GetLookAtEnemyYaw());
	}
}

void AGrey::CheckEngagedTime(float DeltaTime)
{
	if (ActionState == EActionState::EAS_Engaged)
	{
		EngagedTime += DeltaTime;
		if (EngagedTime > EngagedThreshold)
		{
			ActionState = EActionState::EAS_Unoccupied;
			CombatEnemy = nullptr;
		}
		
		if (!IsEnemyAlive()) ActionState = EActionState::EAS_Unoccupied;

		if (Attributes && !Attributes->IsAlive())
		{
			ActionState = EActionState::EAS_Dead;
			CombatEnemy = nullptr;
		}
	}
}

void AGrey::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGrey::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGrey::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGrey::Jump);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGrey::Attack);
		EnhancedInputComponent->BindAction(SpecialAttackAction, ETriggerEvent::Triggered, this, &AGrey::SpecialAttack);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, & AGrey::InteractPressed);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AGrey::Dodge);
		EnhancedInputComponent->BindAction(BlockPressedAction, ETriggerEvent::Triggered, this, &AGrey::GuardPressed);
		EnhancedInputComponent->BindAction(BlockReleasedAction, ETriggerEvent::Triggered, this, &AGrey::GuardReleased);
		EnhancedInputComponent->BindAction(TakePotionAction, ETriggerEvent::Triggered, this, &AGrey::TakePotion);
	}
}

void AGrey::SetHUDHealth()
{
	if (LustreWidget && Attributes)
	{
		LustreWidget->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AGrey::CheckRollDirection()
{
	CharacterDirection = UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation());

	if (CharacterDirection >= -45.f && CharacterDirection < 45.f)
		RollName = FName("Forward");
	else if (CharacterDirection >= -135.f && CharacterDirection < -45.f)
		RollName = FName("Left");
	else if (CharacterDirection >= 45.f && CharacterDirection < 135.f)
		RollName = FName("Right");
	else
		RollName = FName("Back");
}

float AGrey::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	CombatEnemy = Cast<AEnemy>(DamageCauser->GetOwner());
	return DamageAmount;
}

