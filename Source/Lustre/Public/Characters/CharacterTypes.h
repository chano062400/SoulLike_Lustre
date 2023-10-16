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
