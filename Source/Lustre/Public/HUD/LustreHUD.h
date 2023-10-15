// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LustreHUD.generated.h"

class ULustreWidget;
class UDeath;
/**
 * 
 */
UCLASS()
class LUSTRE_API ALustreHUD : public AHUD
{
	GENERATED_BODY()

		virtual void BeginPlay() override;
private:

	UPROPERTY(EditDefaultsOnly, Category = Slash)
		TSubclassOf<ULustreWidget> LustreWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = Death)
		TSubclassOf<UDeath> DeathClass;

	UPROPERTY()
	TObjectPtr<ULustreWidget> LustreWidget;

	UPROPERTY()
	TObjectPtr<UDeath> Death;

public:
	FORCEINLINE ULustreWidget* GetLustreWidget() const { return LustreWidget; }
	FORCEINLINE UDeath* GetDeathUI() const { return Death; }

	virtual void PreInitializeComponents() override;

	void PlayDeathUI();
};

