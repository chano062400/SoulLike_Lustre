#include "HUD/MainMenu.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"



void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &ThisClass::NewGameButtonClicked);
	}

	if (LoadGameButton)
	{
		LoadGameButton->OnClicked.AddDynamic(this, &ThisClass::LoadGameButtonClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::ExitButtonClicked);
	}

}

void UMainMenu::NewGameButtonClicked()
{
	UGameplayStatics::OpenLevel(this,FName("/Game/Lustre/Grey/Map/Stage1-1"));
	this->RemoveFromRoot();
	if (UWorld* world = GetWorld())
	{
		APlayerController* Controller = world->GetFirstPlayerController();
		if (Controller)
		{
			FInputModeGameOnly InputMode;
			Controller->SetInputMode(InputMode);
		}
	}
}

void UMainMenu::LoadGameButtonClicked()
{
}

void UMainMenu::ExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
}
