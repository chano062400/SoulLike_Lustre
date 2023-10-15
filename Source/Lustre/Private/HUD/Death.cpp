// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Death.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UDeath::ShowAnim()
{
	PlayAnimation(FadeIn);
}


