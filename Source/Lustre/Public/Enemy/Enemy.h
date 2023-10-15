// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class AAIController;
class UWidgetComponent;
class ASoul;
class AController;

UCLASS()
class LUSTRE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void SpawnSoul();
	
	virtual void Attack() override;
	
	virtual bool CanAttack() override;
	
	virtual void HandleDamage(float DamageAmount) override;
	
	UFUNCTION(BlueprintCallable)
		virtual void Die() override;

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	bool IsInFOV = false;
private:

	void InitiallizeEnemy();
	
	void CheckPatrolTarget();
	
	void CheckCombatTarget();
	
	void PatrolTimerFinished();
	
	void HideWidget();
	
	void ShowHealthBar();
	
	void LoseInterest();
	
	void StartPatrolling();
	
	void ChaseTarget();
	
	bool IsOutsideCombatRadius();
	
	bool InOutsideAttackRadius();
	
	bool InInsideAttackRadius();
	
	bool IsChasing();
	
	bool IsAttacking();
	
	bool IsDead();
	
	bool IsEngaged();
	
	void ClearPatrolTimer();
	
	void StartAttackTimer();
	
	void ClearAttackTimer();
	
	bool InTargetRange(AActor* Target, double Radius);
	
	void MoveToTarget(AActor* Target);
	
	AActor* ChoosePatrolTarget();
	
	void StopAttackMontage();
	
	virtual void SpawnDefaultWeapon();

	virtual int32 PlayDeathMontage() override;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> ExecutionWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASoul> SoulClass;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<UAnimMontage> ParryingSuccessMontage;

	FTimerHandle PatrolTimer;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Destroyed() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual void ParryingSuccess();

	UFUNCTION(BlueprintCallable)
	bool CanExecution();
};
