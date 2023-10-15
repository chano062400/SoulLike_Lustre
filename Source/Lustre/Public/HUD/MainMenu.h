// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UTextBlock;
class UButton;
class UBorder;
class UVerticalBox;
class UImage;
/**
 * 
 */
UCLASS()
class LUSTRE_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> NewGame;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> LoadGame;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ExitGame;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UImage> TItleImage;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> NewGameButton;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> LoadGameButton;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> ExitButton;

	UFUNCTION()
	void NewGameButtonClicked();

	UFUNCTION()
	void LoadGameButtonClicked();

	UFUNCTION()
	void ExitButtonClicked();

};
