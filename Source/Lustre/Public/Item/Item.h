// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class ULustreWidget;

enum class EItemstate : uint8
{
	EIS_NotEquipped,
	EIS_Equipped,
};

UCLASS()
class LUSTRE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

protected:

	virtual void BeginPlay() override;

	EItemstate ItemState = EItemstate::EIS_NotEquipped;

	TObjectPtr<ULustreWidget> LustreWidget;

public:	

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Scene;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ItemMeshSword;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ItemMeshShield;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ItemEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickupEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> PickupSound;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickupEffect();

	virtual void SpawnPickupSound();

	UPROPERTY(EditAnywhere)
	float RunningTime = 0;

	UPROPERTY(EditAnywhere)
	float SinRange = 1;

};
