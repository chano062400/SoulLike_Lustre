// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure/Treasure.h"
#include "Characters/Grey.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/SphereComponent.h"
#include "HUD/LustreWidget.h"


void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddGold(this);
	}
	
	SpawnPickupSound();
	
	Destroy();
	
}
