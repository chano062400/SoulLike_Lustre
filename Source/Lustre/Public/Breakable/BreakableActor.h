// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Treasure/Treasure.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;

UCLASS()
class LUSTRE_API ABreakableActor : public AActor , public IHitInterface
{
	GENERATED_BODY()
	
public:	

	ABreakableActor();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
		TArray<TSubclassOf<ATreasure>> TreasureClasses;

public:	

	bool bBroken = false;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

};
