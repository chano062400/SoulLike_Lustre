// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LustreAnimInstance.generated.h"

class AGrey;
class AEnemy;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class LUSTRE_API ULustreAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	TObjectPtr<AGrey> LustreCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = Character)
	TObjectPtr<AEnemy> LustreEnemy;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	TObjectPtr<UCharacterMovementComponent> LustreCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;
	
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;
	
	UFUNCTION()
	void AnimNotify_EnableParry();
	
	UFUNCTION()
	void AnimNotify_DisableParry();

};
