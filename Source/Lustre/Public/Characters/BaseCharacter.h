
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UInputAction;
class UAnimMontage;
class UAttributeComponent;
class ATwoHandWeapon;

UCLASS()
class LUSTRE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

	void PlayHitSound(const FVector& ImpactPoint);
	
	void SpawnHitParticles(const FVector& ImpactPoint);
	
	void DisableCapsule();
	
	bool IsAlive();

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	
	virtual void SpawnDefaultWeapon();

	virtual void SpecialAttack();

	virtual bool CanAttack();

	virtual void PlayHitReactMontage(FName SectionName);
	
	virtual int32 PlayAttackMontage();
	
	virtual int32 PlayDeathMontage();

	virtual void Attack();

	virtual void Die();

	virtual void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void SpecialRangeAttack();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;
	
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = Combat)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<UAnimMontage> SpecialAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere)
	double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = VisualEffect)
	TObjectPtr<UParticleSystem> GuardParticles;
	
	UPROPERTY(EditAnywhere, Category = VisualEffect)
	TObjectPtr<UParticleSystem> ParryingParticles;

	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffect)
	TObjectPtr<UParticleSystem> HitParticles;

public:
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float GetHealth();
};
