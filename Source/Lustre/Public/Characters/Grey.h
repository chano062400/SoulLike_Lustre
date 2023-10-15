// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Grey.generated.h"


class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;
class UBoxComponent;
class USceneComponent;
class AItem;
class AWeapon;
class ULustreWidget;
class ASoul;
class ATreasure;
class UCameraShakeBase;
class UPaperSpriteComponent;
class USceneCaptureComponent2D;
class AEnemy;
class UDeath;
class UAudioComponent;

UCLASS()
class LUSTRE_API AGrey : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	AGrey();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

	void InitializeWidget();

	void CheckEngagedTime(float DeltaTime);

	virtual void SetOverlappingItem(AItem* Item) override;

	virtual void SpecialAttack() override;



	/* Input */

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Input)
		UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Input)
		UInputAction* MoveAction;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Input)
		UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* SpecialAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* BlockPressedAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* BlockReleasedAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Input)
		UInputAction* TakePotionAction;

		UCharacterMovementComponent* CharacterMovement;

	/* AnimMontages */

	UPROPERTY(EditDefaultsOnly, Category = Montage)
		UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
		UAnimMontage* DodgeMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montage)
		UAnimMontage* BlockParryMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montage)
		UAnimMontage* TakePotionMontage;

	/* Items */

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItems;

	void TakePotion();

	void FinishTakePotion();

	void InteractPressed();

	void AddSouls(ASoul* Soul) override;

	void AddGold(ATreasure* Gold) override;

	virtual void SpawnDefaultWeapon();

	bool IsTakingPotion = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Potion;
	
	/* States */

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/* Combat */

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AEnemy> CombatEnemy;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	virtual void Attack();

	void Dodge();

	void GuardPressed();

	void GuardReleased();

	void PlayAttackMontages();

	void PlayTakePotionMontage();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual void Die() override;

	void RegenStamina(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDodgeMontage();

	bool HasEnoughStamina();

	void SetActionState();

	FRotator GetLookAtEnemyYaw();

	bool IsEnemyAlive();

	void LookAtCombatEnemy(float DeltaTime);

	void SetHUDHealth();

	bool IsPlayingCombatSound = false;

	bool IsSpecialAttacking = false;

	int AttackCount =0;

	bool IsAttacking = false;

	bool SaveAttacking;

	bool CanMove = true;

	bool IsRunning = false;

	bool IsJumping = false;

	UPROPERTY(EditAnywhere)
	float InterpSpeed = 60.f;

	UPROPERTY(EditAnywhere)
	float EngagedTime;

	UPROPERTY(EditAnywhere)
	float EngagedThreshold = 5.f;

	UPROPERTY(EditAnywhere)
	float KnockBackDistance = 75.f;

	UPROPERTY(EditAnywhere)
	float HealAmount = 50.f;

	UFUNCTION(BlueprintCallable)
	void SaveAttack();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();

	UFUNCTION(BlueprintCallable)
	void CanMoving();

	UFUNCTION(BlueprintCallable)
	void Invincible();

	UFUNCTION(BlueprintCallable)
	void NotInvincible();

	UFUNCTION(BlueprintCallable)
	void CanJumping();

	UFUNCTION(BlueprintCallable)
	void CanSpecialAttack();

	/* Sounds */

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> GuardSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ParryingSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> CombatSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> DieSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> HealSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CombatSoundComponent;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> MiniMapCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> CharacterSprite;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> MiniMapCapture;

	UPROPERTY()
	TObjectPtr<UDeath> Death;

	UPROPERTY()
	TObjectPtr<ULustreWidget> LustreWidget;

public:	

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EGuardState GuardState = EGuardState::EGS_NoGuard;

	void GuardSuccess(FVector& ImpactPoint);

	void ParryingSuccess(FVector& ImpactPoint);

	void CameraShake();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE EGuardState GetGuardState() const { return GuardState; }

};