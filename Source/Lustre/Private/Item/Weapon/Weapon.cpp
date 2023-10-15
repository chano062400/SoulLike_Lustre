// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Weapon.h"
#include "Characters/Grey.h"
#include "Characters/CharacterTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "HUD/LustreWidget.h"
#include "Lustre/Public/Enemy/Enemy.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(ItemMeshSword);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	ShieldBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldBox"));
	ShieldBox->SetupAttachment(ItemMeshShield);
	ShieldBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	WeaponTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon TraceStart"));
	WeaponTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon TraceEnd"));
	WeaponTraceStart->SetupAttachment(ItemMeshSword);
	WeaponTraceEnd->SetupAttachment(ItemMeshSword);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::WeaponBoxOverlap);

}

void AWeapon::Equip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent);
	ItemState = EItemstate::EIS_Equipped;
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	ItemMeshSword->AttachToComponent(InParent, AttachmentRules, FName("RightHandSocket"));
	ItemMeshShield->AttachToComponent(InParent, AttachmentRules, FName("LeftHandSocket"));
}


void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::WeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsSameType(OtherActor)) return;
	FHitResult BoxHit;
	WeaponBoxTrace(BoxHit);

	AEnemy* AttackerEnemy = Cast<AEnemy>(GetOwner());

	AEnemy* HitEnemy = Cast<AEnemy>(BoxHit.GetActor());

	AGrey* AttackerCharacter = Cast<AGrey>(GetOwner());

	AGrey* HitCharacter = Cast<AGrey>(BoxHit.GetActor());

	if (AttackerEnemy && HitCharacter && HitEnemy == nullptr) // Enemy가 공격했을 경우.
	{
		if (IsSameType(BoxHit.GetActor())) return;

		if (HitCharacter->GetGuardState() == EGuardState::EGS_NoGuard) //가드하지 않았을 경우.
		{
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			ExecuteGetHit(BoxHit);
		}
		else if (HitCharacter->GetGuardState() == EGuardState::EGS_Guarding) // 가드했지만 패링 타이밍에 맞지 않는 경우.
		{
			if (IsOnFrontEnemy(HitCharacter)) // 적이 앞에 있어야 가드가 가능.
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage * 0.5f, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				HitCharacter->GuardSuccess(BoxHit.ImpactPoint);
			}
			else
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				ExecuteGetHit(BoxHit);
			}
		}
		else if (HitCharacter->GetGuardState() == EGuardState::EGS_Parryable && AttackerEnemy) // 패링 타이밍에 맞게 가드했을 경우.
		{
			if (IsOnFrontEnemy(HitCharacter)) // 적이 앞에 있어야 패링이 가능.
			{
				HitCharacter->CameraShake();
				HitCharacter->ParryingSuccess(BoxHit.ImpactPoint);
				AttackerEnemy->ParryingSuccess();
			}
			else
			{
				UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				ExecuteGetHit(BoxHit);
			}
		}
	}
	else if (HitCharacter == nullptr && HitEnemy && AttackerCharacter) // Enemy를 공격했을 경우.
	{
		if (IsSameType(BoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		AttackerCharacter->CameraShake();
	}

}

bool AWeapon::IsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
	}
}

void AWeapon::WeaponBoxTrace(FHitResult& BoxHit)
{
	const FVector Start = WeaponTraceStart->GetComponentLocation();
	const FVector End = WeaponTraceEnd->GetComponentLocation();
	
	TArray<AActor*> ActorsToIgnore;
	
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}


	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, WeaponBoxTraceExtent, WeaponBox->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1
		, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);

	if (GetOwner()->ActorHasTag(FName("Enemy")))
	{
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}

}

bool AWeapon::IsOnFrontEnemy(AGrey* HitCharacter)
{
	const FVector Forward = HitCharacter->GetActorForwardVector();

	const FVector ToEnemy = (GetOwner()->GetActorLocation() - HitCharacter->GetActorLocation()).GetSafeNormal();

	double CosTheta = FVector::DotProduct(Forward, ToEnemy);

	if (CosTheta >= 0.f) return true;
	else return false; 
}

