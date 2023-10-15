// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LustreWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void ULustreWidget::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void ULustreWidget::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void ULustreWidget::SetGold(int32 Gold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void ULustreWidget::SetSouls(int32 Soul)
{
	if (SoulText)
	{
		SoulText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Soul)));
	}
}

void ULustreWidget::SetPotionText(int32 PotionAmount)
{
	if (PotionText)
	{
		PotionText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PotionAmount)));
	}
}

void ULustreWidget::ShowInteractMeesage()
{
	if (HintText)
	{
		HintText->SetVisibility(ESlateVisibility::Visible);

	}
}

void ULustreWidget::HideInteractMessage()
{
	if (HintText)
	{
		HintText->SetVisibility(ESlateVisibility::Hidden);

	}
}
