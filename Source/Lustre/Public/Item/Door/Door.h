// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class LUSTRE_API ADoor : public AItem
{
	GENERATED_BODY()

		
public:

	ADoor();
	
	virtual void BeginPlay() override;

protected:

	FTimeline Timeline;

	UPROPERTY(EditAnywhere)
	UCurveFloat* CurveFloat;

	bool bIsDoorClosed = true;

	UPROPERTY(EditAnywhere)
	float DoorRotateAngle = 90.f;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Tick(float DeltaTime);

	UFUNCTION()
		void OpenDoor(float Value);

private:

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> DoorFrame;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> Door;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> Door2;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USphereComponent> PortalSphere;


public:

	void OnInteract();
};
