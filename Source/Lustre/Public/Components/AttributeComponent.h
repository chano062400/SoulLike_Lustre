// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUSTRE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float SpecialAttackCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float RegenStaminaCost = 8.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Potion = 5;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 MaxPotion = 15;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxHealth;

public:
	void ReceiveDamage(float Damage);

	float GetHealthPercent();
	
	float GetStaminaPercent();
	
	bool IsAlive();

	void Heal(float HealthToHeal);

	void AddSouls(int32 NumberOfSouls);
	
	void AddGold(int32 NumboerOfGold);

	void UseStamina(float StaminaCost);

	void UsePotion(int32 PotionAmount);
	
	void RegenStamina(float DeltaTime);


	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetPotion() const { return Potion; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetSpecialAttackCost() const { return SpecialAttackCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
