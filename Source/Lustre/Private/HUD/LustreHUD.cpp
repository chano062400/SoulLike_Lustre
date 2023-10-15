// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LustreHUD.h"
#include "HUD/LustreWidget.h"
#include "HUD/Death.h"

void ALustreHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ALustreHUD::PreInitializeComponents()
{
	if (TObjectPtr<UWorld> world = GetWorld())
	{
		APlayerController* Controller = world->GetFirstPlayerController();
		if (Controller && LustreWidgetClass)
		{
			LustreWidget = CreateWidget<ULustreWidget>(Controller, LustreWidgetClass);
			
			LustreWidget->AddToViewport();
		}
	}
}

void ALustreHUD::PlayDeathUI()
{
	if (UWorld* world = GetWorld())
	{
		APlayerController* Controller = world->GetFirstPlayerController();
		if (Controller && DeathClass)
		{
			Death = CreateWidget<UDeath>(Controller, DeathClass);
			
			Death->ShowAnim();
			
			Death->AddToViewport();
			
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			
			Controller->SetInputMode(InputMode);
			
			Controller->SetShowMouseCursor(true);
		}
	}
}
