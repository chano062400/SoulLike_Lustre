// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "Characters/Grey.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/LustreWidget.h"


// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	ItemMeshSword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	ItemMeshSword->SetupAttachment(GetRootComponent());
	ItemMeshSword->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMeshSword->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemMeshShield = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShieldMesh"));
	ItemMeshShield->SetupAttachment(GetRootComponent());
	ItemMeshShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMeshShield->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(ItemMeshSword);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}


void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ItemState == EItemstate::EIS_NotEquipped)
	{
		RunningTime += DeltaTime;
		float SinMove = FMath::Sin(RunningTime * 2) * SinRange;
		AddActorWorldOffset(FVector(0.f, 0.f, SinMove));
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickupEffect()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

