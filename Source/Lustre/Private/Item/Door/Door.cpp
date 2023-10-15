// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Door/Door.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ADoor::ADoor()
{
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorFrame);

	Door2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door2"));
	Door2->SetupAttachment(DoorFrame);

	PortalSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PortalSphere"));
	PortalSphere->SetupAttachment(DoorFrame);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ThisClass::OpenDoor);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	PortalSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	PortalSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void ADoor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ADoor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ADoor::Tick(float DeltaTime)
{
	Timeline.TickTimeline(DeltaTime);
}

void ADoor::OpenDoor(float Value)
{
	FRotator Rot = FRotator(0.f, DoorRotateAngle * Value, 0.f);
	FRotator Rot2 = FRotator(0.f, DoorRotateAngle * -Value, 0.f);

	Door->SetRelativeRotation(Rot2);
	Door2->SetRelativeRotation(Rot);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	DoorFrame->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorFrame->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Door->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Door->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Door2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Door2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);


}

void ADoor::OnInteract()
{
	if (bIsDoorClosed)
	{
		Timeline.Play();
	}
	else
		Timeline.Reverse();

	bIsDoorClosed = !bIsDoorClosed;
}
