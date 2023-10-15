// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LustreWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class LUSTRE_API ULustreWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	
	void SetStaminaBarPercent(float Percent);
	
	void SetGold(int32 Gold);
	
	void SetSouls(int32 Soul);
	
	void SetPotionText(int32 PotionAmount);

	void ShowInteractMeesage();
	
	void HideInteractMessage();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HintText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PotionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MiniMap;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PotionImage;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PotionBorder;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ShieldImage;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ShieldBorder;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SwordImage;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SwordBorder;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BlankImage;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Border;
	

};
