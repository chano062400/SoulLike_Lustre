
#include "Characters/BaseCharacter.h"
#include "Item/Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Item/Weapon/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Characters/CharacterTypes.h"
#include "Item/Weapon/TwoHandWeapon.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"

ABaseCharacter::ABaseCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled)
{
	if ((EquippedWeapon && EquippedWeapon->GetWeaponBox()))
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::PlayHitReactMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::Die()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::SpecialAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SpecialAttackMontage)
	{
		AnimInstance->Montage_Play(SpecialAttackMontage);
	}

}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector(); //캐릭터 위치 벡터를 정규화한다.
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z); 
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); // 맞은 위치의 벡터를 정규화한다.

	const double CosTheta = FVector::DotProduct(Forward, ToHit); // 벡터의 외적을 이용해 Cos Theta를 구한다.
	double Theta = FMath::Acos(CosTheta); // 역코사인을 이용해 Theta를 구한다.

	Theta = FMath::RadiansToDegrees(Theta); // Theta를 라디안에서 각도로 변환시킨다.

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit); // 벡터의 내적을 이용해 벡터의 법선벡터가 위인지 아래인지 구한다. 
	if (CrossProduct.Z < 0) // 법선벡터가 아래라면 
	{
		Theta *= -1.f; // Theta는 음수가 된다.
	}

	FName Section = FName();

	if (Theta >= -45.f && Theta < 45.f)  // 앞에서 맞았을 때
		Section = FName("Back");
	else if (Theta >= -135.f && Theta < -45.f) // 왼쪽에서 맞았을 때
		Section = FName("Right");
	else if (Theta >= 45.f && Theta < 135.f) // 오른쪽에서 맞았을 때
		Section = FName("Left");
	else
		Section = FName("Front"); // 뒤에서 맞았을 때

	PlayHitReactMontage(Section); // 피격방향에 맞는 애니메이션을 취한다.
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1 ;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

void ABaseCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && DefaultWeaponClass)
	{
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);

		DefaultWeapon->Equip(GetMesh(), this, this);

		EquippedWeapon = DefaultWeapon;

	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Attack()
{

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else Die();
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

float ABaseCharacter::GetHealth()
{
	return Attributes && Attributes->GetHealth();
}

