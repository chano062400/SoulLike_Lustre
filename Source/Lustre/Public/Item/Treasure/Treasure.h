	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Treasure.generated.h"

class ULustreWidge;

/**
 * 
 */
UCLASS()
class LUSTRE_API ATreasure : public AItem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
		int32 Gold;

	UPROPERTY()
	TObjectPtr<ULustreWidget> LustreWidget;

	FORCEINLINE int32 GetGold() const { return Gold; }

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
