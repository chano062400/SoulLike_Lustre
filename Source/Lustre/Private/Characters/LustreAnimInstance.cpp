// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LustreAnimInstance.h"
#include "Characters/Grey.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/Enemy.h"
#include "Lustre/Public/Characters/CharacterTypes.h"


void ULustreAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	LustreCharacter = Cast<AGrey>(TryGetPawnOwner());
	
	LustreEnemy = Cast<AEnemy>(TryGetPawnOwner());

	if (LustreCharacter)
	{
		LustreCharacterMovement = LustreCharacter->GetCharacterMovement();
	}
}

void ULustreAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (LustreCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(LustreCharacterMovement->Velocity);
		
		IsFalling = LustreCharacterMovement->IsFalling();
		
		Velocity = LustreCharacter->GetVelocity();
	}
}

void ULustreAnimInstance::AnimNotify_EnableParry()
{
	LustreCharacter->GuardState = EGuardState::EGS_Parryable;
}

void ULustreAnimInstance::AnimNotify_DisableParry()
{
	LustreCharacter->GuardState = EGuardState::EGS_Guarding;
}
