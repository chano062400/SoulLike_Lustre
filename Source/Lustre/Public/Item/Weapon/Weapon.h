// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"

class USKeletalMeshComponent;
class USceneComonent;
class UBoxComponent;
class AGrey;
class ULustreWidget;

/**
 * 
 */
UCLASS()
class LUSTRE_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void BeginPlay() override;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual	void WeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual bool IsSameType(AActor* OtherActor);

	virtual void ExecuteGetHit(FHitResult& BoxHit);

	virtual void WeaponBoxTrace(FHitResult& BoxHit);

	bool IsOnFrontEnemy(AGrey* HitCharacter);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> ShieldBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> WeaponTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> WeaponTraceEnd;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ShieldTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ShieldTraceEnd;

	TObjectPtr<ULustreWidget> LustreWidget;
	
	TObjectPtr<AGrey> Character;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	UPROPERTY(EditAnywhere)
	float TwoHandSpecialAttackDamage = 70.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

public:

	TArray<AActor*> IgnoreActors;
	
	void Equip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator);

	void AttachMeshToSocket(USceneComponent* InParent);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector WeaponBoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector ShieldBoxTraceExtent = FVector(5.f);
	
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE UBoxComponent* GetShieldBox() const { return ShieldBox; }
};
