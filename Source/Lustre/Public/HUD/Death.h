// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Death.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class LUSTRE_API UDeath : public UUserWidget
{
	GENERATED_BODY()

private:


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReStartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReStart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MainMenu;
	
	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeIn;


public:
	void ShowAnim();

};

