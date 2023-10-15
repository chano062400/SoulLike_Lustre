#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Item/Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Item/Soul.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"



AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	ExecutionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Execution Widget"));
	ExecutionWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 45.f;
	PawnSensing->SetPeripheralVisionAngle(90.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitiallizeEnemy();

	Tags.Add(FName("Enemy"));

	GetMesh()->HideBoneByName(FName("weapon_l"), EPhysBodyOp::PBO_None);

	GetMesh()->HideBoneByName(FName("weapon_r"), EPhysBodyOp::PBO_None);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsDead()) return;
	
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	
	if (CanExecution())
	{
		ExecutionWidget->SetVisibility(true);
	}
	else
	{
		ExecutionWidget->SetVisibility(false);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	
	CombatTarget = EventInstigator->GetPawn();
	
	ChaseTarget();
	
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);

	if(!IsDead()) ShowHealthBar();
	
	ClearPatrolTimer();
	
	ClearAttackTimer();
	
	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	
	StopAttackMontage();
}

void AEnemy::ParryingSuccess()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (ParryingSuccessMontage && AnimInstance)
	{
		AnimInstance->Montage_Stop(10.f, AttackMontage);
		
		AnimInstance->Montage_Play(ParryingSuccessMontage);
		
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool AEnemy::CanExecution()
{
	return Attributes && Attributes->GetHealth() < 30.f;
}

void AEnemy::Die()
{
	Super::Die();
	
	EnemyState = EEnemyState::EES_Dead;
	
	PlayDeathMontage();
	
	ClearAttackTimer();
	
	HideWidget();
	
	DisableCapsule();
	
	SetLifeSpan(DeathLifeSpan);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);

	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
	}
}

void AEnemy::Attack()
{
	if (CombatTarget == nullptr) return;

	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}

	EnemyState = EEnemyState::EES_Engaged;
	
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		InInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::InitiallizeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	
	MoveToTarget(PatrolTarget);
	
	HideWidget();
	
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		
		LoseInterest();
		
		if(!IsEngaged()) StartPatrolling();
	}
	else if (InOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		
		if (!IsEngaged()) ChaseTarget();
	}
	else if (CanAttack())
	{
		ClearAttackTimer();
		
		StartAttackTimer();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::HideWidget()
{
	if (HealthBarWidget && ExecutionWidget)
	{
		HealthBarWidget->SetVisibility(false);

		ExecutionWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	
	HideWidget();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::InOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::InInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	
	FAIMoveRequest MoveRequest;
	
	MoveRequest.SetGoalActor(Target);
	
	MoveRequest.SetAcceptanceRadius(150.f);
	
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && DefaultWeaponClass)
	{
		AWeapon* DefaulfWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);

		DefaulfWeapon->Equip(GetMesh(), this, this);

		EquippedWeapon = DefaulfWeapon;
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking&&
		SeenPawn->ActorHasTag(FName("Grey"));
	
	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::StopAttackMontage()
{
	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace)
	{
		AnimInstace->Montage_Stop(1, AttackMontage);
	}
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}


