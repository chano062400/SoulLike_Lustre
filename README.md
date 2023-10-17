# SoulLike_Lustre
포트폴리오 목적으로 제작한 소울라이크류 게임입니다.

●엔진 : Unreal Engine 5 5.0.3

●제작기간 : 2023.04.10 ~ 2023.05.28 (약 한달 반)

●개발규모 : 팀프로젝트(4인)

●담당부분 : Level 제작 외 모든 부분 담당 

# 포트폴리오 영상
https://www.youtube.com/watch?v=Be1NwqZVF2U
[![Video Label](http://img.youtube.com/vi/Be1NwqZVF2U/0.jpg)](https://youtu.be/Be1NwqZVF2U)

# 핵심코드

# BaseCharacter

● 피격, 공격, Damage처리 등 Enemy와 Player에게 필요한 기능들을 담당.



GetHit_Implementation (Interface 함수) - 피격시 실행하는 함수.

```cpp
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
```

DirectionalHitReact - 벡터의 내적,외적을 이용해 피격위치에 따른 애니메이션을 실행하는 함수.

```cpp
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
```

SpawnDefaultWeapon - 기본 무기를 게임 시작시 장착하는 함수

```cpp
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
```

# CharacterTypes 

● enum class를 통해 Enemy와 주인공 캐릭터의 State를 저장.

```cpp
#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedWeapon UMETA(DisplayName = "Equipped Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Engaged UMETA(DisplayName = "Engaged"),
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_MAX UMETA(DisplayName = "Default MAX")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")

	
};

UENUM(BlueprintType)
enum class EGuardState : uint8
{
	EGS_NoGuard UMETA(DisplayName = "No Guarding"),
	EGS_Guarding UMETA(DisplayName = "Guarding"),
	EGS_Parryable UMETA(DisplayName = "Parrable"),

	EGS_MAX UMETA(DisplayName = "Default MAX")
};

UENUM(BlueprintType)
enum class ERollState : uint8
{
	ERS_NoRolling UMETA(DisplayName = "No Rolling"),
	ERS_Rolling UMETA(DisplayName = "Rolling"),

	ERS_MAX UMETA(DisplayName = "Default MAX")
};

```

# Grey - 주인공 캐릭터 클래스.

● InputComponent

● ActionState, GuardState등 State에 따른 함수 처리

● Attack, Special Attack, Guard, Parrying 등 전투 기능

CheckEngagedTime - 전투 상태를 체크하는 함수 

```cpp
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

		if (Attributes && !Attributes->IsAlive()) // Die()가 실행돼도 ActionState가 넘어가지 않아서 여기서 처리.
		{
			ActionState = EActionState::EAS_Dead;
			CombatEnemy = nullptr;
		}
	}
}
```

전투시 구르기는 Direction에 따라 다른 애니메이션을 실행.
CheckRollDirection -  Direction을 구하여 SectionName을 설정하는 함수.

```cpp
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
```

```cpp
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
```

PlayTakePotionMontage - TakePotionMontage를 실행하는 함수. 

```cpp
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
			if (bInterrupted && CharacterMovement) // TakePotionMontage 실행중 피격시
			{
				ActionState = EActionState::EAS_Engaged;
				CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
				EquippedWeapon->ItemMeshShield->SetVisibility(true);
				IsTakingPotion = false;
			}
			else // 방해받지 않고 Montage가 끝났을 시
			{
				FinishTakePotion();
			}
		});
	AnimInstance->Montage_SetEndDelegate(MontageEnd, TakePotionMontage);
}
```

```cpp
void AGrey::FinishTakePotion()
{
	if (Attributes && CharacterMovement)
	{
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking); // Montage가 끝나야 움직일 수 있음.
		
		Attributes->Heal(HealAmount);
		
		SetHUDHealth();
		
		EquippedWeapon->ItemMeshShield->SetVisibility(true);
		
		Potion->SetVisibility(false);
		
		IsTakingPotion = false;
	}
}
```

Die - Hp가 0이 되어 State가 Dead가 되었을때 실행하는 함수.

```cpp
void AGrey::Die()
{
	ActionState = EActionState::EAS_Dead;
	
	Tags.Add(FName("Dead"));
	
	CanMove = false;

	if (CombatEnemy) CombatEnemy = nullptr;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->bOrientRotationToMovement = false; // 죽고나서 캐릭터가 움직이지 않도록.
	
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
```

SetActionState - Tick함수에 두어 frame마다 ActionState를 체크하고 State에 맞는 기능을 실행하는 함수.

```cpp
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
```
# Enemy 

● Patrol, Chase, Attack, Engaged 등 State에 따른 함수 처리

CheckCombatTarget - CombatTarget을 체크하고 상태에 따른 기능을 실행하는 함수.

```cpp
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
```

Die - Hp가 0이 되어 State가 Dead가 되었을때 실행하는 함수.

```cpp
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
```

PlayRandomMontageSection - Random한 AttackMontage Section을 가져오는 함수.

```cpp
int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1 ;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}
```

# Weapon

● Enemy와 Character가 장착하는 무기의 기본 클래스.

● BoxTrace를 통한 충돌, Apply Damage 처리.

WeaponBoxOverlap 

● Hit, Attack 대상에 따라 기능을 실행하는 함수. 

● Hit 대상이 Character이고 Attack 대상이 Enemy라면 State에 따라 Guard와 Parrying 기능을 실행.

```cpp
void AWeapon::WeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsSameType(OtherActor)) return;
	FHitResult BoxHit;
	WeaponBoxTrace(BoxHit);

	AEnemy* AttackerEnemy = Cast<AEnemy>(GetOwner());

	AEnemy* HitEnemy = Cast<AEnemy>(BoxHit.GetActor());

	AGrey* AttackerCharacter = Cast<AGrey>(GetOwner());

	AGrey* HitCharacter = Cast<AGrey>(BoxHit.GetActor());

	if (AttackerEnemy && HitCharacter && HitEnemy == nullptr) // Enemy가 공격했을 경우.
	{
		if (IsSameType(BoxHit.GetActor())) return;

		if (HitCharacter->GetGuardState() == EGuardState::EGS_NoGuard) //가드하지 않았을 경우.
		{
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			ExecuteGetHit(BoxHit);
		}
		else if (HitCharacter->GetGuardState() == EGuardState::EGS_Guarding) // 가드했지만 패링 타이밍에 맞지 않는 경우.
		{
			if (IsOnFrontEnemy(HitCharacter)) // 적이 앞에 있어야 가드가 가능.
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage * 0.5f, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				HitCharacter->GuardSuccess(BoxHit.ImpactPoint);
			}
			else
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				ExecuteGetHit(BoxHit);
			}
		}
		else if (HitCharacter->GetGuardState() == EGuardState::EGS_Parryable && AttackerEnemy) // 패링 타이밍에 맞게 가드했을 경우.
		{
			if (IsOnFrontEnemy(HitCharacter)) // 적이 앞에 있어야 패링이 가능.
			{
				HitCharacter->CameraShake();
				HitCharacter->ParryingSuccess(BoxHit.ImpactPoint);
				AttackerEnemy->ParryingSuccess();
			}
			else
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				ExecuteGetHit(BoxHit);
			}
		}
	}
	else if (HitCharacter == nullptr && HitEnemy && AttackerCharacter) // Enemy를 공격했을 경우.
	{
		if (IsSameType(BoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		AttackerCharacter->CameraShake();
	}

}
```

IsOnFrontEnemy 

● Guard와 Parrying은 모두 적이 앞에 있어야 실행이 가능한데, 적이 앞에 있는지 판별하는 함수.

```cpp
bool AWeapon::IsOnFrontEnemy(AGrey* HitCharacter)
{
	const FVector Forward = HitCharacter->GetActorForwardVector();

	const FVector ToEnemy = (GetOwner()->GetActorLocation() - HitCharacter->GetActorLocation()).GetSafeNormal();

	double CosTheta = FVector::DotProduct(Forward, ToEnemy);

	if (CosTheta >= 0.f) return true;
	else return false; 
}
```

CreateFields - FieldSystem을 이용해 GeometryCollection Actor에 충돌을 일으켜 부시는 함수.

(블루프린트로 구현)

![scsac](https://github.com/chano062400/SoulLike_Lustre/assets/144113194/33bc0142-e3f8-4da4-8832-d5e5d9f8d5aa)

